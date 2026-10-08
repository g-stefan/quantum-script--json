# Script API

Everything the extension defines after `Script.requireExtension("JSON")`.
`JSON` is a plain object holding three functions; it has no constructor,
no options and no state. `JSON.decode` is native (C++), `JSON.encode` and
`JSON.encodeWithIndentation` are script functions compiled when the
extension loads.

A Quantum Script string is a sequence of bytes (UTF-8 by convention).

## `JSON.decode(text)`

Parses JSON text and returns the value. On any error it returns
**`undefined`**; it never throws.

```javascript
var v = JSON.decode("{\"a\":1,\"b\":[1,2,3],\"c\":{\"d\":null}}");
v.a;                                    // 1
v.b.length;                             // 3
v.c.d;                                  // null
typeof(JSON.decode("[]"));              // "Array"
typeof(JSON.decode("{}"));              // "Object"
JSON.decode("\"hi\\n\"");               // "hi" + new line
JSON.decode("42");                      // 42
JSON.decode("{\"a\":}");                // undefined (error)
```

Always check the result:

```javascript
var config = JSON.decode(text);
if (Script.isUndefined(config)) {
	throw new Error("invalid JSON");
};
```

The argument is converted with `toString` first: `JSON.decode(123)` is
`123`, `JSON.decode()` and `JSON.decode({})` are `undefined` (`"undefined"`
and `"Object"` are not JSON).

### Value mapping

| JSON | Quantum Script |
|------|----------------|
| `{ ... }` | `Object` (plain object, keys are strings) |
| `[ ... ]` | `Array` |
| `"text"`, `'text'` | `String` (UTF-8 bytes) |
| `12`, `2.5`, `0x1F` | `Number` |
| `true` / `false` | `Boolean` |
| `null` | `null` |

Duplicate keys: the last one wins (`{"a":1,"a":2}` → `{a: 2}`).

### Accepted syntax

The reader is the Quantum Script tokenizer, so it is more lenient than the
JSON standard in some places and stricter in others.

Accepted beyond standard JSON:

| Input | Result |
|-------|--------|
| `'single quoted'` strings and keys | same as double quoted |
| `// line` and `/* block */` comments | ignored |
| `[1,2,]`, `{"a":1,}` (trailing comma) | `[1, 2]`, `{a: 1}` |
| `[1 2 3]`, `{"a":1 "b":2}` (missing commas) | `[1, 2, 3]`, `{a: 1, b: 2}` |
| `[,,1]`, `{"a":1,,"b":2}` (extra commas) | `[1]`, `{a: 1, b: 2}` |
| `0x10`, `-0xFF` | `16`, `-255` |
| `007`, `-01.5` | `7`, `-1.5` |
| `3.` | `3` |
| `[1] trailing text` | `[1]` (anything after the first value is ignored) |
| escapes `\v`, `\'`, `\xHH`, octal `\101` | vertical tab, `'`, byte `HH`, byte `0101` |

Standard escapes: `\"`, `\\`, `\/`, `\b`, `\f`, `\n`, `\r`, `\t` and
`\uXXXX`. `\uXXXX` is converted to UTF-8, and a surrogate pair
`\uD83D\uDE00` becomes one 4-byte UTF-8 character. Raw UTF-8 in strings is
kept as it is. An unknown escape (`\q`) is an error.

Not accepted (the whole `decode` returns `undefined`):

| Input | Why |
|-------|-----|
| `+1`, `.5` | not numbers (a number starts with `-` or a digit) |
| `-`, `[-true]`, `1e`, `[1e+]`, `0x` | incomplete number |
| `truex`, `[tru]`, `nullx` | not a keyword |
| `{a:1}`, `{1:2}` | keys must be quoted strings |
| `NaN`, `Infinity`, `undefined` | not JSON values |
| `""`, only spaces | no value |
| `[1,2`, `{"a":1` | not closed |

Numbers follow the JSON grammar: an optional `-`, digits, an optional
fraction `.digits` and an optional exponent `e` / `E`, optional `+` / `-`,
digits (`-1`, `-2.5`, `1e3`, `1.5E-3`, `-1e+2`). They are read with
`sscanf("%lf")` into a double: integers above 2^53 lose precision
(`12345678901234567890` → `12345678901234567168`).

`true`, `false` and `null` are recognized anywhere, also as the whole text
(`JSON.decode("true")` is `true`).

## `JSON.encode(value)`

Returns compact JSON text: no spaces, no new lines.

```javascript
JSON.encode({a: 1, b: [true, null], c: "x"});   // {"a":1,"b":[true,null],"c":"x"}
JSON.encode([]);                                // []
JSON.encode("say \"hi\"");                      // "say \"hi\""
JSON.encode(2.5);                               // 2.5
```

| Value | Written as |
|-------|------------|
| `null` | `null` |
| `true` / `false` | `true` / `false` |
| number | `Convert.toString(x)`: `5000.00` → `5000`, `1/3` → `0.3333333333333333`, `-0` → `0` |
| `NaN`, `Infinity`, `-Infinity` | the **strings** `"NaN"`, `"Infinity"`, `"-Infinity"` |
| string | `Convert.toStringSource(x)`, see [string escapes](#string-escapes) |
| `Array` | `[...]`, every index from `0` to `length - 1`; holes are `null` |
| `Object` | `{...}`, every enumerable property (`for in`), keys as strings |
| `AssociativeArray` | `{...}`, keys in insertion order |
| `undefined`, function, anything else | `null` (also as an object member: `{a: undefined}` → `{"a":null}`) |
| `new String("x")`, `new Number(3)` | `"x"`, `3` |
| `new Error("x")` | `{"message":"x"}` |

There is no cycle detection: encoding an object that contains itself
(`o.me = o`) never finishes.

### String escapes

Strings are written with the engine's C style escaping
(`Convert.toStringSource`):

| Byte | Written as |
|------|------------|
| `"` | `\"` |
| `\` | `\\` |
| new line, carriage return, tab | `\n`, `\r`, `\t` |
| printable ASCII `0x20`-`0x7E` (including `'` and `/`) | as is |
| any other byte: control characters, **every byte of a UTF-8 character** | `\xHH` (upper case hex) |
| a hex digit right after a `\xHH` | also `\xHH`, so the escape is not read as longer |

```javascript
JSON.encode("ă");        // "\xC4\x83"
JSON.encode("é1a");      // "\xC3\xA9\x31\x61"
JSON.encode("a\u0001");  // "a\x01"
```

The output is pure ASCII and `JSON.decode` reads it back exactly
(`JSON.decode(JSON.encode(s)) == s` for every string), but `\xHH` is **not
valid standard JSON**: other JSON parsers (browsers, Python, jq, ...)
reject it. See [limitations](#limitations).

### Number formatting

Numbers are written by `Convert.toString`, the same text as `"" + x`.
Very large and very small numbers use exponent form:

```javascript
JSON.encode([100000000000000000000, 1000000000000000000000, 0.000001]);
// [100000000000000000000,1e+21,1e-06]
```

`JSON.decode` reads both forms back. Use `Convert.numberFormat(x, decimals)`
and store the text as a string when the exact form matters.

### Key order

Plain objects enumerate their keys in the engine's **symbol order**: the
order in which each key name was first used anywhere in the engine, not the
order of insertion in this object. In a fresh script that is usually the
insertion order, but a key name already known to the engine (from the
standard library, from earlier code, or from earlier decoded data) moves to
the front:

```javascript
JSON.encode(JSON.decode("{\"zeta\":1,\"alpha\":2}"));   // {"zeta":1,"alpha":2}
var o = {};
o.zz = 1;
o.message = 2;
JSON.encode(o);                                        // {"message":2,"zz":1}  "message" was already a symbol
```

When the order of keys in the output matters, build an `AssociativeArray`,
which encodes in insertion order:

```javascript
var m = new AssociativeArray();
m["version"] = "5.9.0";
m["build"] = "6";
JSON.encode(m);                         // {"version":"5.9.0","build":"6"}
```

`JSON.decode` always returns plain objects, never `AssociativeArray`s.

## `JSON.encodeWithIndentation(value[, step])`

Same values and rules as `JSON.encode`, written for people:

- every member and element on its own line;
- one tab per level;
- `": "` (colon and space) between key and value;
- lines separated by `\n` (not `\r\n`);
- empty containers are written over two lines: `{` blank line `}`.

```javascript
Console.writeLn(JSON.encodeWithIndentation({a: 1, b: [1, {c: 2}], e: {}}));
```

```
{
	"a": 1,
	"b": [
		1,
		{
			"c": 2
		}
	],
	"e": {

	}
}
```

`step` (optional, default `0`) is the starting indentation level: members
are indented `step + 1` tabs and the closing bracket `step` tabs. The first
line is never indented, so the result can be placed after `"key": ` in
text that is already indented. `step` is converted with `Convert.toNumber`;
`NaN`, infinite or negative values count as `0`.

```javascript
JSON.encodeWithIndentation({a: [1]}, 1);
// {
// 		"a": [
// 			1
// 		]
// 	}
```

Scalars are written as with `encode`: `JSON.encodeWithIndentation(5)` is
`5`.

The output of both encoders is accepted by `JSON.decode`.

## Round trip

For data made of objects, arrays, strings, booleans, `null` and finite
numbers:

```javascript
var copy = JSON.decode(JSON.encode(value));               // equal data
var copy2 = JSON.decode(JSON.encodeWithIndentation(value));
```

What changes:

- `undefined`, functions and holes become `null`;
- `AssociativeArray`s come back as plain objects;
- `NaN` / `Infinity` come back as the strings `"NaN"` / `"Infinity"`;
- key order may change (symbol order);
- numbers keep the precision of their text form (`Convert.toString`, 16
  significant digits).

```javascript
JSON.decode(JSON.encode({a: -5}));                        // {a: -5}
JSON.decode(JSON.encode([1000000000000000000000]));       // [1e+21]
```

## Limitations

| Limitation | Effect | Work-around |
|------------|--------|-------------|
| `encode` writes `\xHH` for non-ASCII and control bytes | not standard JSON; other tools reject it | keep text ASCII for files other software reads, or post-process |
| `NaN` / `Infinity` become strings | type changes on round trip | convert back with `Convert.toNumber` |
| No error message | `decode` only returns `undefined` | validate small pieces to locate the problem |
| Integers above 2^53 | precision lost | keep them as strings |

Files written by fabricare and other XYO tools (version numbers as strings,
ASCII text) are within these limits.

## Recipes

Read a JSON file, `undefined` when missing or invalid:

```javascript
Script.requireExtension("Shell");
Script.requireExtension("JSON");

function jsonReadFile(fileName) {
	var text = Shell.fileGetContents(fileName);
	if (Script.isNil(text)) {
		return undefined;
	};
	return JSON.decode(text);
};
```

Write a JSON file:

```javascript
function jsonWriteFile(fileName, value) {
	return Shell.filePutContents(fileName, JSON.encodeWithIndentation(value) + "\n");
};
```

Read a value with a default:

```javascript
function jsonGet(object, key, defaultValue) {
	if (Script.isNil(object)) {
		return defaultValue;
	};
	if (Script.isUndefined(object[key])) {
		return defaultValue;
	};
	return object[key];
};
```

(`object.key || defaultValue` does not work in Quantum Script: `||`
returns a boolean.)

Iterate over an object and an array:

```javascript
var info = JSON.decode("{\"files\":[\"a.zip\",\"b.zip\"],\"size\":{\"a.zip\":10,\"b.zip\":20}}");
for (var name of info.files) {
	Console.writeLn(name + " " + info.size[name]);
};
for (var key in info.size) {
	Console.writeLn(key);
};
```

Deep copy of plain data:

```javascript
var copy = JSON.decode(JSON.encode(original));
```

Parse the JSON output of a command:

```javascript
Script.requireExtension("ProcessInteractive");
var releases = JSON.decode(ProcessInteractive.run("tool --output json"));
if (Script.isUndefined(releases)) {
	throw new Error("unexpected output");
};
```

Functions must be defined before they are called: Quantum Script has no
hoisting.
