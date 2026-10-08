# API reference

## Script

Available after `Script.requireExtension("JSON")`.

| Symbol | Returns | Notes |
|--------|---------|-------|
| `JSON.decode(text)` | value or `undefined` | argument via `toString`; objects → `Object`, arrays → `Array`; `undefined` on any error, never throws; first value only, rest of text ignored |
| `JSON.encode(value)` | String | compact; `undefined` / functions / holes → `null`; `NaN` / `±Infinity` → strings; strings via `Convert.toStringSource` (`\xHH` for non-ASCII) |
| `JSON.encodeWithIndentation(value[, step])` | String | like `encode`, one item per line, tab indent, `": "`, `\n` line ends; `step` = starting level (default `0`, invalid → `0`) |

### Decode edge cases

| Expression | Result |
|------------|--------|
| `JSON.decode("{\"a\":1}")` | `{a: 1}` |
| `JSON.decode("[]")`, `JSON.decode("{}")` | `[]`, `{}` |
| `JSON.decode("42")`, `JSON.decode("1.25")` | `42`, `1.25` |
| `JSON.decode("0x10")`, `JSON.decode("-0xFF")`, `JSON.decode("007")` | `16`, `-255`, `7` |
| `JSON.decode("-1")`, `JSON.decode("[1,-1,-2.5]")` | `-1`, `[1, -1, -2.5]` |
| `JSON.decode("1e3")`, `JSON.decode("[1.5e-3,2E+2,-1e2]")` | `1000`, `[0.0015, 200, -100]` |
| `JSON.decode("[1e]")`, `JSON.decode("[1e+]")`, `JSON.decode("-")`, `JSON.decode("[-true]")` | `undefined` (incomplete number) |
| `JSON.decode("+1")`, `JSON.decode(".5")` | `undefined` |
| `JSON.decode("true")`, `"false"`, `"null"` | `true`, `false`, `null` |
| `JSON.decode("[true,false,null]")` | `[true, false, null]` |
| `JSON.decode("truex")`, `JSON.decode("[tru]")` | `undefined` |
| `JSON.decode("'x'")`, `JSON.decode("{'a':1}")` | `"x"`, `{a: 1}` |
| `JSON.decode("{a:1}")` | `undefined` (unquoted key) |
| `JSON.decode("[1,2,]")`, `JSON.decode("[1 2]")`, `JSON.decode("[,,1]")` | `[1, 2]`, `[1, 2]`, `[1]` |
| `JSON.decode("/* c */ [1, // x\n 2]")` | `[1, 2]` |
| `JSON.decode("[1] garbage")` | `[1]` |
| `JSON.decode("{\"a\":1,\"a\":2}")` | `{a: 2}` |
| `"\u0041"`, `"\u00e9"`, `"\ud83d\ude00"` in the text | `A`, `é` (2 bytes), 😀 (4 bytes) |
| `"\x41"`, `"\101"`, `"\v"` in the text | `A`, `A`, vertical tab |
| `"\q"` in the text | `undefined` |
| `JSON.decode("")`, `JSON.decode("  ")` | `undefined` |
| `JSON.decode("[1,2")`, `JSON.decode("{\"a\":}")` | `undefined` |
| `JSON.decode("NaN")`, `JSON.decode("undefined")` | `undefined` |
| `JSON.decode("12345678901234567890")` | `12345678901234567168` (double precision) |
| `JSON.decode(123)` | `123` |
| `JSON.decode()`, `JSON.decode({})` | `undefined` |

### Encode edge cases

| Expression | Result |
|------------|--------|
| `JSON.encode(null)` | `null` |
| `JSON.encode(undefined)`, `JSON.encode()`, `JSON.encode(function(){})` | `null` |
| `JSON.encode({a: undefined, b: 1})` | `{"a":null,"b":1}` |
| `a = []; a[2] = 1; JSON.encode(a)` (holes) | `[null,null,1]` |
| `JSON.encode([])`, `JSON.encode({})` | `[]`, `{}` |
| `JSON.encode(5000.00)`, `JSON.encode(0 - 0)` | `5000`, `0` |
| `JSON.encode(1/3)` | `0.3333333333333333` |
| `JSON.encode(1000000000000000000000)`, `JSON.encode(0.000001)` | `1e+21`, `1e-06` |
| `JSON.encode(-5)` | `-5` |
| `JSON.encode(0/0)`, `JSON.encode(1/0)`, `JSON.encode(-1/0)` | `"NaN"`, `"Infinity"`, `"-Infinity"` |
| `JSON.encode("a\tb\r\n")` | `"a\tb\r\n"` |
| `JSON.encode("it's a/b")` | `"it's a/b"` |
| `JSON.encode("ă")` | `"\xC4\x83"` |
| `JSON.encode("é1a")` | `"\xC3\xA9\x31\x61"` (hex digits after `\xHH` escaped too) |
| `JSON.encode({1: "a"})` | `{"1":"a"}` |
| `JSON.encode(associativeArray)` | object, insertion order |
| `JSON.encode(new String("x"))`, `JSON.encode(new Number(3))` | `"x"`, `3` |
| `JSON.encode(new Error("x"))` | `{"message":"x"}` |
| `JSON.encodeWithIndentation({})`, `([])` | `{` + `\n\n` + `}`, `[` + `\n\n` + `]` |
| `JSON.encodeWithIndentation(5)` | `5` |
| `JSON.encodeWithIndentation(x, "2")`, `(x, -3)` | step `2`, step `0` |
| object containing itself | never finishes |

### Errors

| Message | Cause |
|---------|-------|
| `Unable to open "JSON"` | the extension library was not found and no internal one is registered (for example in the static `quantum-script`) |

`JSON.decode` reports errors by returning `undefined`; `JSON.encode` and
`JSON.encodeWithIndentation` do not throw for any value.

## Hosts

| Host | `JSON` |
|------|--------|
| `quantum-script`, dynamic SDK | yes (DLL) |
| `quantum-script`, static SDK | no |
| fabricare build scripts | yes, preloaded (internal, `quantum-script--json.static`) |
| magnet | yes, internal |

## C++

Namespace `XYO::QuantumScript::Extension::JSON`, umbrella header
`<XYO/QuantumScript.Extension/JSON.hpp>`.

### Library (`JSON/Library.hpp`)

| Symbol | Notes |
|--------|-------|
| `void registerInternalExtension(Executive *executive)` | register `"JSON"` as an internal extension |
| `void initExecutive(Executive *executive, void *extensionId)` | extension init, run by the engine |
| `extern "C" void quantumScriptExtension(Executive *, void *)` | DLL entry point (not in static builds) |

### Metadata

| Symbol | Notes |
|--------|-------|
| `Version::version()`, `Version::build()`, `Version::versionWithBuild()`, `Version::datetime()` | from `version.json` |
| `Copyright::copyright()`, `Copyright::publisher()`, `Copyright::company()`, `Copyright::contact()` | |
| `License::license()`, `License::shortLicense()` | MIT text |

`Version`, `Copyright` and `License` exist in every XYO library: qualify them
(`Extension::JSON::Version::versionWithBuild()`).

### Build configuration

| Name | Meaning |
|------|---------|
| `quantum-script--json` | fabricare project, `dll-or-lib` |
| `quantum-script--json.static` | fabricare project, static library with static CRT, for static tools on a dynamic platform |
| `test.01` | fabricare test project, runs `test/test.01.js` |
| `fabricare/make.prepare.js` | `file-to-cs`: `Library.js` → `Library.Source.cpp` (`librarySource`) |
| `XYO_QUANTUMSCRIPT_EXTENSION_JSON_EXPORT` | export / import macro |
| `XYO_QUANTUMSCRIPT_EXTENSION_JSON_INTERNAL` | defined while building the DLL (from `QUANTUM_SCRIPT__JSON_INTERNAL`) |
| `XYO_QUANTUMSCRIPT_EXTENSION_JSON_LIBRARY` | static build: empty export macro, no DLL entry point |
| `XYO_QUANTUMSCRIPT_DEBUG_RUNTIME` | trace line `- json-decode` |
