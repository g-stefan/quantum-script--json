---
name: quantum-script--json
description: >-
  How to use the Quantum Script JSON extension (quantum-script--json),
  loaded with Script.requireExtension("JSON"): JSON.decode(text) (JSON text
  -> Object / Array / String / Number / Boolean / null; returns undefined on
  any error, never throws; JSON numbers with sign, fraction and exponent;
  lenient: single quotes, // and /* */ comments, missing / trailing commas,
  0x hex, leading zeros, text after the first value ignored; \uXXXX and
  surrogate pairs -> UTF-8), JSON.encode(value) (compact; undefined / functions / holes ->
  null, NaN / Infinity -> strings "NaN" / "Infinity", strings via
  Convert.toStringSource so non-ASCII and control bytes become \xHH - not
  standard JSON, large / small numbers in exponent form 1e+21 / 1e-06, no
  cycle detection) and JSON.encodeWithIndentation(value, step) (one item per
  line, tab indent, ": ", "\n" line ends, empty containers on two lines);
  plain objects encode in symbol order, not insertion order
  (AssociativeArray keeps insertion order); round trip limits; reading and
  writing JSON files with Shell; which hosts have it (quantum-script dynamic
  SDK, fabricare build scripts preloaded, magnet; NOT the static
  quantum-script); the C++ side (registerInternalExtension, initExecutive,
  quantumScriptExtension entry point, Library.js compiled to
  Library.Source.cpp by file-to-cs, quantum-script--json.static,
  XYO_QUANTUMSCRIPT_EXTENSION_JSON_LIBRARY). Use when writing or reviewing
  Quantum Script, fabricare or magnet code that reads or writes JSON, C++
  code that includes <XYO/QuantumScript.Extension/JSON.hpp>, a
  fabricare.json depending on "quantum-script--json", or when working
  inside the quantum-script--json repository.
---

# quantum-script--json

JSON extension of Quantum Script (see the `quantum-script` skill for the
language and its differences from JavaScript; its rules apply — no
hoisting, `typeof(x)` needs parentheses, `&&` / `||` return booleans,
statements and blocks end with `;`). Purpose: **convert JSON text to
Quantum Script values and back**, for configuration files, release
metadata and tool / API output.

Full documentation: `docs/` in the quantum-script--json repository
(`X:\Storage\XYO\Gitea\CPP\quantum-script--json\docs` on this machine):
README (purpose), getting-started (build, load, hosts, C++ registration),
**script-api** (accepted syntax, value mapping, escapes, key order,
limitations, recipes), cpp-api, reference (edge case tables). When in doubt
read `source/XYO/QuantumScript.Extension/JSON/Library.cpp` (decode) and
`Library.js` (encode), or run it: `quantum-script --run="..."`.

## Script API

```javascript
Script.requireExtension("JSON");

var v = JSON.decode("{\"a\":1,\"b\":[true,null]}");   // Object; undefined on error, never throws
JSON.encode(v);                                        // {"a":1,"b":[true,null]}
JSON.encodeWithIndentation(v);                         // multi-line, tabs, "\n"
JSON.encodeWithIndentation(v, 1);                      // start at indent level 1
```

## Hard rules

1. **`decode` returns `undefined` on error**, never throws and gives no
   message. Always check `Script.isUndefined(result)`.
2. **Numbers**: `[-] digits [. digits] [e|E [+|-] digits]` as in JSON
   (`-1`, `-2.5`, `1e3`, `1.5E-3`), plus `0x` hex (`-0xFF`), leading zeros
   (`007`) and `3.`. Not numbers: `+1`, `.5`; incomplete ones (`-`, `1e`,
   `1e+`, `0x`) make the result `undefined`. `encode` output (`-5`,
   `1e+21`, `1e-06`) decodes back.
3. **Keywords** `true` / `false` / `null` work anywhere, also as the whole
   text (`JSON.decode("null")` is `null`); `truex`, `tru` → `undefined`.
4. **Strict dispatch**: the first character of a value selects its type; a
   value that fails half way makes the whole result `undefined` (no
   fallback: `[tr[1]]` → `undefined`).
5. **Lenient input accepted**: `'single'` quotes (also keys), `//` and
   `/* */` comments, missing / extra / trailing commas, escapes
   `\v \' \xHH` and octal; anything after the first value is
   ignored. Keys must be quoted (`{a:1}` → `undefined`). Duplicate keys:
   last wins. `\uXXXX` (and surrogate pairs) → UTF-8 bytes.
6. **`encode` escapes like C, not JSON.** `"`, `\`, `\n`, `\r`, `\t` as
   usual; every byte outside printable ASCII (UTF-8 text, control chars)
   becomes `\xHH` (`"ă"` → `"\xC4\x83"`), and a hex digit right after
   such an escape too. `JSON.decode` reads it back exactly, but browsers,
   Python, jq, ... reject `\x`. Do not promise standard JSON to other
   software when strings may be non-ASCII.
7. **`encode` value mapping**: `undefined`, functions, array holes,
   unknown values → `null` (object members are kept as `"k":null`, not
   omitted); `NaN` / `Infinity` / `-Infinity` → **strings**; numbers via
   `Convert.toString` (`5000.00` → `5000`); `AssociativeArray` → object;
   `new Error("x")` → `{"message":"x"}`. No cycle detection: a
   self-referencing object never finishes.
8. **Key order = symbol order.** Plain objects (including decoded ones)
   enumerate keys in the order the key *names* were first created in the
   engine, not insertion order: a known name (`build`, `message`, names used
   earlier) moves to the front. Rewriting a file can reorder keys. Use
   `new AssociativeArray()` (`m["k"] = v`) when output order matters;
   `decode` never returns `AssociativeArray`.
9. **`encodeWithIndentation`**: one member per line, one tab per level,
   `"key": value`, `\n` line ends (add `"\n"` at file end yourself), empty
   `{}` / `[]` written as `{` blank line `}`. `step` = starting level; the
   first line is not indented; invalid / negative `step` → `0`.
10. Numbers are doubles: integers above 2^53 lose precision; keep IDs as
    strings.
11. **Hosts**: dynamic `quantum-script` (loads `quantum-script--json.dll`),
    **fabricare build scripts** (internal, already required) and **magnet**
    (internal) have it. The **static** `quantum-script`
    (`win64-msvc-2026.static`) does not: `Unable to open "JSON"`.
12. One engine per thread: each thread requires `JSON` itself; functions
    are stateless.

## Recipes

```javascript
Script.requireExtension("Shell");
Script.requireExtension("JSON");

function jsonReadFile(fileName) {                  // undefined if missing or invalid
	var text = Shell.fileGetContents(fileName);
	if (Script.isNil(text)) {
		return undefined;
	};
	return JSON.decode(text);
};

function jsonWriteFile(fileName, value) {
	return Shell.filePutContents(fileName, JSON.encodeWithIndentation(value) + "\n");
};

var info = jsonReadFile("version.json");
if (Script.isUndefined(info)) {
	throw new Error("version.json missing or invalid");
};
info["quantum-script--json"].build = "7";          // non-identifier keys: obj["key"]
jsonWriteFile("version.json", info);

var size = Script.isUndefined(info.size) ? 0 : info.size;   // default: no `||`

for (var key in info) {                            // object keys
	// info[key]
};

var ordered = new AssociativeArray();              // fixed key order in output
ordered["version"] = "5.9.0";
ordered["build"] = "6";
JSON.encode(ordered);                              // {"version":"5.9.0","build":"6"}

var copy = JSON.decode(JSON.encode(original));     // deep copy (limits: rule 7)
```

## C++

```cpp
#include <XYO/QuantumScript.Extension/Console.hpp>
#include <XYO/QuantumScript.Extension/JSON.hpp>
using namespace XYO::QuantumScript;

void initExecutive(Executive *executive) {                 // host init callback
	Extension::Console::registerInternalExtension(executive);
	Extension::JSON::registerInternalExtension(executive);    // scripts still requireExtension("JSON")
};
```

- fabricare.json dependency: `"quantum-script--json"` (`dll-or-lib`: DLL
  on dynamic platforms, static lib on `*.static` platforms); it pulls in
  `quantum-script` and `quantum-script--console`. Statically linked tools
  on a dynamic platform (fabricare) use `"quantum-script--json.static"`,
  which defines `XYO_QUANTUMSCRIPT_EXTENSION_JSON_LIBRARY` for dependents.
- DLL entry point `extern "C" quantumScriptExtension(Executive *, void *)`
  only with `XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY` and without
  `XYO_QUANTUMSCRIPT_EXTENSION_JSON_LIBRARY`. Static hosts must register
  the extension as internal.
- No separate C++ JSON API: call the script function with
  `functionApply` (get `JSON` from `Context::getGlobalObject()`, then
  `decode` / `encode`) after `Script.requireExtension("JSON")` ran.

## Working in this repository

- `JSON.decode` is C++ (`JSON/Library.cpp`: `jsonDecodeX` →
  `jsonParseValue`, which dispatches on the first character to
  `jsonParseObject`, `jsonParseArray`, `Token::isString`,
  `jsonParseNumber`, `jsonIsKeyword`). Numbers and keywords use their own
  readers, not `Token::isNumber` / `isSymbolX` (script grammar: no sign, no
  exponent). `JSON.encode` and `encodeWithIndentation` are script in
  `JSON/Library.js`.
- **Edit `Library.js`, never `Library.Source.cpp`**: `fabricare make` runs
  `fabricare/make.prepare.js`, which regenerates it with `file-to-cs`
  (`librarySource`), and `initExecutive` compiles it with `compileStringX`.
- Build: `fabricare make`, `fabricare test` (runs `test/test.01`, which
  registers Console and JSON as internal and runs `test/test.01.js`:
  `check(name, text, expected)` compares `JSON.encode(JSON.decode(text))`,
  `"undefined"` for errors, and throws on mismatch; add a check for every
  decode change), `fabricare install` (see the `fabricare`
  skill). `quantum-script` and `quantum-script--console` must be installed
  first. With the SDK installed scripts can be run directly:
  `quantum-script test/test.01.js`.
- Behaviour changes (`\uXXXX` in encode, key order, decode rules) change fabricare release scripts and magnet scripts output; update
  `README.md`, `docs/script-api.md`, `docs/reference.md`, `test/test.01.js`
  and this skill together.
- Code style: tabs (width 8), `.clang-format`, CRLF, statements and blocks
  end with `};`, camelCase. SPDX header: MIT for `source/` and `docs/`,
  Unlicense for `test/`, `fabricare/` and `.claude/` (see `.reuse/dep5`).
