# Quantum Script Extension JSON — Documentation

`quantum-script--json` is the **JSON codec extension of Quantum Script**.
Loaded with `Script.requireExtension("JSON")`, it adds a `JSON` object to
scripts with three functions:

```javascript
JSON.decode(text);                      // JSON text -> value (undefined on error)
JSON.encode(value);                     // value -> compact JSON text
JSON.encodeWithIndentation(value);      // value -> JSON text, one item per line, tab indented
```

JSON is the text format used by configuration files, release metadata, web
APIs and command line tools to exchange structured data: objects
(`{"key": value}`), arrays (`[1, 2]`), strings, numbers, `true`, `false`
and `null`. This extension converts between that text and Quantum Script
values.

- **Decode.** `JSON.decode("{\"name\":\"xyo\",\"list\":[1,2.5,true,null]}")`
  returns an `Object` whose `list` is an `Array`. Objects become plain
  objects, arrays become arrays, strings, numbers, booleans and `null` keep
  their type. Invalid input returns `undefined`; `decode` never throws.
- **Encode.** `JSON.encode({a: 1, b: [true, null]})` returns
  `{"a":1,"b":[true,null]}`. Objects, `AssociativeArray`s and arrays are
  walked recursively; `undefined`, functions and array holes are written as
  `null`.
- **Pretty print.** `JSON.encodeWithIndentation(value)` writes the same data
  with one member per line, indented with tabs, as used for the
  `*.json` release files written by fabricare.
- **Lenient reader.** `decode` also accepts single quoted strings, `//` and
  `/* */` comments, missing and trailing commas, hexadecimal numbers and
  ignores any text after the first value.
- **Known limits.** `encode` writes non-ASCII bytes as `\xHH` (a Quantum
  Script / C escape, not standard JSON), and `NaN` / `Infinity` as strings.
  See [Script API, limitations](script-api.md#limitations) before
  exchanging data with other software.

```
scripts: quantum-script .js, fabricare build scripts, magnet, embedding hosts, ...
quantum-script--json      <-- this extension: JSON.decode / encode / encodeWithIndentation
quantum-script            (Executive, Variable, Context, Token, Convert.toStringSource)
quantum-script--console   (link dependency)
xyo-system, xyo-encoding, xyo-multithreading, xyo-data-structures, xyo-managed-memory, xyo-platform
```

## Why it exists

| Need | What `JSON` gives |
|------|-------------------|
| Read configuration and metadata files (`version.json`, release info, ...) | `JSON.decode(Shell.fileGetContents(file))` |
| Read API / tool output (Gitea, GitHub release lists, ...) | fabricare's release scripts decode it with `JSON.decode` |
| Write data other scripts read back | `JSON.encode` / `JSON.encodeWithIndentation` |
| Human readable, diff friendly files | `encodeWithIndentation`: one member per line, tabs |
| No dependency on a parser library | the reader reuses the Quantum Script tokenizer, the writer is a small script |

## Concepts at a glance

| Need | Use | Notes |
|------|-----|-------|
| Load the extension | `Script.requireExtension("JSON");` | already loaded in fabricare and magnet |
| Parse text | `var v = JSON.decode(text);` | `undefined` on error: check `Script.isUndefined(v)` |
| Read a file | `JSON.decode(Shell.fileGetContents("f.json"))` | `Shell` extension for files |
| Write compact text | `JSON.encode(v)` | no spaces, no new lines |
| Write readable text | `JSON.encodeWithIndentation(v)` | `\n` line ends, tab indent |
| Ordered keys | build an `AssociativeArray` | plain objects encode in symbol order, not insertion order |
| Non-ASCII text for other software | keep it in this ecosystem, or escape yourself | `encode` writes `\xHH` |

## Contents

| Document | What it covers |
|----------|----------------|
| [Getting started](getting-started.md) | Build and install, load the extension from a script, which hosts have it (quantum-script, fabricare, magnet), register it in a C++ host |
| [Script API](script-api.md) | Every function, accepted input, value mapping, string escapes, number formatting, key order, limitations, recipes |
| [C++ API](cpp-api.md) | `registerInternalExtension`, `initExecutive`, the DLL entry point, build defines, how decode and the script part of encode work |
| [API reference](reference.md) | Every script and C++ symbol and the edge cases on one page |

Quantum Script itself (the language, `Script.requireExtension`, embedding,
writing extensions) is documented in the `quantum-script` repository,
`docs/`.

## Source map

```
source/XYO/QuantumScript.Extension/JSON.hpp            umbrella header, include this from C++
source/XYO/QuantumScript.Extension/JSON.Amalgam.cpp    the whole extension in one translation unit
source/XYO/QuantumScript.Extension/JSON/
    Dependency.hpp                                     <XYO/QuantumScript.hpp>, export macro
    Library[.hpp/.cpp]                                 initExecutive, registerInternalExtension,
                                                       JSON.decode (C++ parser), DLL entry point
    Library.js                                         JSON.encode, JSON.encodeWithIndentation (script)
    Library.Source.cpp                                 Library.js as a C array, generated, do not edit
    Copyright / License / Version                      library metadata
    Library.rc, *.rh                                   Windows version resource
fabricare/make.prepare.js                              file-to-cs: Library.js -> Library.Source.cpp
test/test.01.cpp                                       C++ host registering Console and JSON as internal
test/test.01.js                                        decode checks (numbers, keywords, structure), round trip
```

## AI assistant skill

A Claude Code skill describing how to use this extension lives in
[`.claude/skills/quantum-script--json/`](../.claude/skills/quantum-script--json/SKILL.md).
It is picked up automatically inside this repository; copy the folder to
`~/.claude/skills/` to have it available in the projects that use `JSON`
(Quantum Script tools, fabricare scripts, magnet scripts).
