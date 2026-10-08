# C++ API

For hosts that embed Quantum Script and for maintainers of the extension.
Read the `quantum-script` repository's `docs/embedding.md` and
`docs/writing-extensions.md` first: native functions, `Variable` and
`TPointer` work the same way here.

## Headers and namespace

```cpp
#include <XYO/QuantumScript.Extension/JSON.hpp>   // Library.hpp

using namespace XYO::QuantumScript;
```

Namespace: `XYO::QuantumScript::Extension::JSON`. Export macro:
`XYO_QUANTUMSCRIPT_EXTENSION_JSON_EXPORT`:

| Define | Effect |
|--------|--------|
| `XYO_QUANTUMSCRIPT_EXTENSION_JSON_INTERNAL` (or `QUANTUM_SCRIPT__JSON_INTERNAL`, set by fabricare while building the DLL) | export macro = `XYO_PLATFORM_LIBRARY_EXPORT` |
| none | export macro = `XYO_PLATFORM_LIBRARY_IMPORT` (consumers of the DLL) |
| `XYO_QUANTUMSCRIPT_EXTENSION_JSON_LIBRARY` (set for dependents of `quantum-script--json.static`) | export macro empty, no `quantumScriptExtension` entry point (sources compiled into another library) |
| `XYO_PLATFORM_COMPILE_STATIC` (static platforms) | `XYO_PLATFORM_LIBRARY_EXPORT` / `IMPORT` are empty |

The `quantumScriptExtension` entry point is compiled only when
`XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY` is defined and
`XYO_QUANTUMSCRIPT_EXTENSION_JSON_LIBRARY` is not.

## Registering the extension

```cpp
void Extension::JSON::registerInternalExtension(Executive *executive);
void Extension::JSON::initExecutive(Executive *executive, void *extensionId);
```

- `registerInternalExtension` registers `"JSON"` as an internal extension;
  call it from the host's init callback (see
  [Getting started](getting-started.md#5-register-it-in-a-c-host)).
- `initExecutive` is the extension's init function, run by the engine when a
  script first requires `JSON` in a thread. It sets the extension name, info
  (`"JSON"` plus the short license text), version and public flag, then:

  ```cpp
  executive->compileStringX("var JSON={};");
  executive->setFunction2("JSON.decode(x)", jsonDecode);
  executive->compileStringX(librarySource);   // Library.js: encode, encodeWithIndentation
  ```

  Do not call it directly.
- The DLL build also exports
  `extern "C" void quantumScriptExtension(Executive *, void *)`, which
  forwards to `initExecutive`; it is what `Script.requireExtension` looks up
  in `quantum-script--json.dll`.

## How it works

### `JSON.decode` (C++, `JSON/Library.cpp`)

`jsonDecode` converts its first argument with `toString()` and calls
`jsonDecode_`, which wraps the text in a `MemoryRead` stream, an `Input` and
a Quantum Script `Token` (the script compiler's tokenizer), then:

| Function | Role |
|----------|------|
| `jsonDecodeX(token, result)` | `token.read()` the first character (white space and comments skipped), then `jsonParseValue` |
| `jsonParseValue(token, result)` | the first character selects the type: `{` object, `[` array, `"` / `'` string, `-` / digit number, `t` / `f` / `n` keyword; anything else, or a value that does not parse, is an error |
| `jsonParseObject(token, result)` | `{`, then loop: skip `,`, `}` ends; a quoted key, `:`, then `jsonParseValue`; stored with `setPropertyByVariable` |
| `jsonParseArray(token, result)` | `[`, then loop: skip `,`, `]` ends; `jsonParseValue`, stored with `setPropertyByIndex(index++)` |
| `jsonParseNumber(token, result)` | `[-] digits [. digits] [e/E [+/-] digits]` or `[-] 0x hexdigits`, read character by character from `token.input`, converted with `sscanf(value, XYO_QUANTUMSCRIPT_FORMAT_NUMBER_INPUT, ...)` (`%lf`) |
| `jsonIsKeyword(token, name)` | matches `true` / `false` / `null`, also at the end of the input; fails when an identifier character follows (`truex`) |

Each returns `false` on an unexpected character; `jsonDecode_` then
returns `Context::getValueUndefined()`. No exception is thrown. Because
the value type is chosen from its first character, a value that fails
half way never falls back to another type (`[tr[1]]` is an error, not
`[[1]]`).

The rest of the behaviour described in
[Script API](script-api.md#accepted-syntax) follows from `Token`
(`quantum-script/source/XYO/QuantumScript/Token.cpp`):

- `Token::isString` accepts `"` and `'`, decodes `\uXXXX` (with surrogate
  pairs) to UTF-8 and also accepts the C escapes `\v`, `\'`, `\xHH`, octal;
- `ignoreSpaceOrComment` skips white space and `//`, `/* */` comments after
  every token;
- `,` is simply skipped, so missing, extra and trailing commas are accepted.

Numbers and keywords do not use `Token::isNumber` / `Token::isSymbolX`:
those follow the script language (no sign, no exponent, a keyword needs a
following character) and are shared with the script compiler.

### `JSON.encode` / `JSON.encodeWithIndentation` (script, `JSON/Library.js`)

Both are Quantum Script functions. They test the value with
`Script.isNull`, `isBoolean`, `isNumber` (`isNaN`, `isInfinite`,
`signBit` for the special values), `isString`, `isArray`, `isObject`,
`isAssociativeArray`, and recurse through `self` (the running function).
Strings and keys are written with `Convert.toStringSource`, which is
`String::encodeC()` from `xyo-encoding` (C escapes, `\xHH` for every byte
outside `0x20`-`0x7E`). Numbers are written with `Convert.toString`.

`fabricare/make.prepare.js` runs, before every `fabricare make`:

```
file-to-cs --touch=Library.cpp --file-in=Library.js --file-out=Library.Source.cpp --is-string --name=librarySource
```

`Library.Source.cpp` is generated (a `static const char librarySource[]`
array, `#include`d by `Library.cpp`): change `Library.js`, then build.

With `XYO_QUANTUMSCRIPT_DEBUG_RUNTIME` defined `jsonDecode` prints a trace
line `- json-decode`.

There is no C++ JSON API outside the script functions: C++ code that needs
JSON either runs a script (`ExecutiveX::executeString`) or calls the script
function through `functionApply`:

```cpp
TPointer<Variable> jsonObject = Context::getGlobalObject()->getPropertyBySymbol(Context::getSymbol("JSON"));
TPointer<Variable> decode = jsonObject->getPropertyBySymbol(Context::getSymbol("decode"));
TPointer<VariableArray> arguments(VariableArray::newArray());
arguments->index(0) = VariableString::newVariable("{\"a\":1}");
TPointer<Variable> value = decode->functionApply(Context::getValueUndefined(), arguments);
```

(after a script ran `Script.requireExtension("JSON")` in this thread).

## Notes for maintainers

- New functions: native ones are registered in `initExecutive` with
  `executive->setFunction2("JSON.name(args)", name)`; script ones go in
  `Library.js` as `JSON.name = function(...) { ... };`. Then update
  `README.md`, `docs/script-api.md`, `docs/reference.md`, the skill in
  `.claude/skills/quantum-script--json/` and `test/test.01.js`.
- Behaviour changes (for example writing `\uXXXX` instead of `\xHH`)
  change what existing files and scripts produce: fabricare's release
  scripts and magnet scripts depend on the current output. Document them in
  `docs/script-api.md` and the skill.
- `test/test.01.js` prints a decoded object with `encodeWithIndentation`,
  then `check(name, text, expected)` compares `JSON.encode(JSON.decode(text))`
  with the expected text (`"undefined"` for errors) for numbers, keywords
  and structure, and checks an encode / decode round trip. A failed check
  throws, so `fabricare test` fails. Add a check for every decode change.
- The `quantum-script--json.static` project must keep the same sources and
  version as `quantum-script--json` (`linkVersion`); fabricare links it.
