# Getting started

## 1. Build and install

The extension is built with [fabricare](https://github.com/g-stefan/fabricare),
the build tool used by all XYO C++ projects. `quantum-script` (and everything
below it: `xyo-system`, `xyo-encoding`, ...) and `quantum-script--console`
must be installed to the SDK first. `file-to-cs` must be on `PATH` (it is
part of the SDK). From the repository root:

```bash
fabricare make       # build into output/
fabricare test       # build and run test/test.01 (run make first)
fabricare install    # copy output/{bin,include,lib} to ~/.fabricare/<platform>
fabricare clean      # remove output/ and temp/
```

`fabricare make` first runs `fabricare/make.prepare.js`, which converts
`JSON/Library.js` (the script half of the extension) into
`JSON/Library.Source.cpp` with `file-to-cs`. Edit `Library.js`, never the
generated file.

`fabricare.json` declares three projects:

| Project | Kind | Purpose |
|---------|------|---------|
| `quantum-script--json` | `dll-or-lib`: shared library in a dynamic build, static library in a static build | the extension |
| `quantum-script--json.static` | static library with static CRT, category `make` | for statically linked tools built on a dynamic platform (fabricare); dependents get `XYO_QUANTUMSCRIPT_EXTENSION_JSON_LIBRARY` defined |
| `test.01` | executable, category `test` | runs `test/test.01.js` with the extension registered as internal |

After `fabricare install`, `quantum-script--json.dll` (Windows) /
`libquantum-script--json.so` (Linux) sits in the SDK `bin` folder next to
`quantum-script.exe`, which is where `Script.requireExtension("JSON")` finds
it.

## 2. Use it from a script

```javascript
Script.requireExtension("Console");
Script.requireExtension("JSON");

var data = JSON.decode("{\"name\":\"xyo\",\"version\":\"5.9.0\",\"tags\":[\"cpp\",\"script\"]}");
if (Script.isUndefined(data)) {
	throw new Error("invalid JSON");
};
Console.writeLn(data.name);               // xyo
Console.writeLn(data.tags[1]);            // script

data.build = 6;
Console.writeLn(JSON.encode(data));
// {"build":6,"name":"xyo","version":"5.9.0","tags":["cpp","script"]}
Console.writeLn(JSON.encodeWithIndentation(data));
// {
// 	"build": 6,
// 	"name": "xyo",
// 	...
// }
```

Run it with:

```bash
quantum-script hello-json.js
```

`build` is written first although it was added last: plain objects
enumerate keys in the engine's symbol order, and `build` is a name the
engine already knew. See [Script API, key order](script-api.md#key-order);
use an `AssociativeArray` when the order matters.

`Script.requireExtension("JSON")` looks for an external
`quantum-script--json` library first (the file as named, then every include
path folder: next to the interpreter, next to the script), then for an
internal extension registered by the host. Loading twice does nothing. A
missing extension throws `Unable to open "JSON"`.

## 3. Read and write a file

```javascript
Script.requireExtension("Shell");
Script.requireExtension("JSON");

var text = Shell.fileGetContents("version.json");
var info = Script.isNil(text) ? undefined : JSON.decode(text);
if (Script.isUndefined(info)) {
	throw new Error("version.json missing or invalid");
};
info["quantum-script--json"].build = "7";
Shell.filePutContents("version.json", JSON.encodeWithIndentation(info));
```

Keys that are not identifiers (`quantum-script--json`) are read with
`object["key"]`. `Shell.fileGetContents` returns `undefined` for a missing
file. Rewriting a file this way can change the order of its keys (here
`build` is written before `version`).

## 4. Which hosts have it

| Host | `JSON` available | How |
|------|------------------|-----|
| `quantum-script` (dynamic SDK, `win64-msvc-2026/bin`) | yes | loads `quantum-script--json.dll` |
| `quantum-script` (static SDK, `win64-msvc-2026.static/bin`) | **no** | a static executable cannot load extension DLLs: `Unable to open "JSON"` |
| `fabricare` build scripts | yes, already required | registered as internal (`quantum-script--json.static`); fabricare's `Library.js` calls `Script.requireExtension("JSON")` at start-up |
| `magnet` scripts | yes | registered as internal by `quantum-script--magnet` |
| your own C++ host | if you register it | see section 5 |

### fabricare

In fabricare build scripts `JSON` is already loaded. fabricare's own release
scripts use it to read Gitea / GitHub release lists
(`JSON.decode(jsonString)`) and to write the per release `*.json` files
(`Shell.filePutContents(file, JSON.encodeWithIndentation(json))`).

## 5. Register it in a C++ host

A host that embeds Quantum Script makes `JSON` available as an internal
extension by registering it in the init callback (this is what
`test/test.01.cpp` does):

```cpp
#include <XYO/QuantumScript.hpp>
#include <XYO/QuantumScript.Extension/Console.hpp>
#include <XYO/QuantumScript.Extension/JSON.hpp>

using namespace XYO::QuantumScript;

void initExecutive(Executive *executive) {
	Extension::Console::registerInternalExtension(executive);
	Extension::JSON::registerInternalExtension(executive);
};

int main(int cmdN, char *cmdS[]) {
	if (ExecutiveX::initExecutive(cmdN, cmdS, initExecutive)) {
		if (!ExecutiveX::executeString(
		        "Script.requireExtension(\"Console\");"
		        "Script.requireExtension(\"JSON\");"
		        "Console.writeLn(JSON.encode({a: [1, 2]}));")) {
			printf("%s\n", (ExecutiveX::getError()).value());
			printf("%s", (ExecutiveX::getStackTrace()).value());
		};
		ExecutiveX::endProcessing();
	};
	return 0;
};
```

Registering only makes the extension *available*: scripts still call
`Script.requireExtension("JSON")`. With the DLL build of the engine an
external `quantum-script--json.dll` found on the include path wins over the
internal one for `requireExtension`; use
`Script.requireInternalExtension("JSON")` to force the internal one.

In the host's `fabricare.json`:

```json
{
	"name": "my-host",
	"make": "exe",
	"sourcePath": "XYO/MyHost",
	"dependency": [
		"quantum-script--json"
	]
}
```

`quantum-script--json` depends on `quantum-script` and
`quantum-script--console`; fabricare resolves them transitively. A
statically linked tool built on a dynamic platform depends on
`quantum-script--json.static` instead (as fabricare itself does).

## 6. Static builds

The project is `dll-or-lib`: on a static platform (for example
`win64-msvc-2026.static`, which sets `XYO_PLATFORM_COMPILE_STATIC`) the same
project builds a static library. The export macros become empty and the
`quantumScriptExtension` DLL entry point is left out (it is compiled only
with `XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY`). Defining
`XYO_QUANTUMSCRIPT_EXTENSION_JSON_LIBRARY` has the same effect when the
sources are compiled into another library; the `quantum-script--json.static`
project passes this define to its dependents.

A static host must register the extension with `registerInternalExtension`
(section 5): external DLLs cannot be loaded into a host that does not use
the engine DLL.

## 7. Threads

Each thread that runs scripts has its own engine, so every thread loads the
extension itself with `Script.requireExtension("JSON")`. The functions keep
no state: they are safe to call from any thread that loaded the extension.
Values decoded in one thread belong to that thread's engine.
