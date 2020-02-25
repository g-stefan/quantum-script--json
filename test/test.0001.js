// Public domain
// http://unlicense.org/
Script.requireExtension("Console");
Script.requireExtension("JSON");

var x = JSON.decode(JSON.encode( {
	test_: 12345
}));

if(x["test_"] == 12345) {
	Console.writeLn("-> test 0001 ok");
} else {
	Console.writeLn("-> test 0001 fail");
};
