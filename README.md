# Quantum Script Extension JSON

Quantum Script extension
- Decoding JSON text into script values:
`JSON.decode` returns objects, arrays, strings, numbers, booleans and `null`,
or `undefined` when the text is not valid.
- Encoding script values into JSON text:
`JSON.encode` writes compact text, `JSON.encodeWithIndentation` writes one
item per line indented with tabs.
- For configuration files, release metadata and the output of tools and web
APIs.

```javascript
Script.requireExtension("JSON");

JSON;
JSON.decode(x);
JSON.encode(x);
JSON.encodeWithIndentation(x,step);
```

Built on `quantum-script`, part of the XYO C++ SDK; also available in
fabricare build scripts and magnet.

`JSON.encode` writes non-ASCII text as `\xHH`, which other JSON parsers do
not accept; see [limitations](docs/script-api.md#limitations).

## Documentation

- [Overview](docs/README.md) - purpose and design
- [Getting started](docs/getting-started.md) - build, load from a script, hosts, register in a C++ host
- [Script API](docs/script-api.md) - accepted syntax, value mapping, escapes, key order, limitations, recipes
- [C++ API](docs/cpp-api.md) - registration, DLL entry point, build defines, how it works
- [API reference](docs/reference.md)

A Claude Code skill for this extension is in
[.claude/skills/quantum-script--json](.claude/skills/quantum-script--json/SKILL.md).

## License

Copyright (c) 2016-2026 Grigore Stefan
Licensed under the [MIT](LICENSE) license.
