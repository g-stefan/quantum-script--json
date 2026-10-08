// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

Script.requireExtension("Console");
Script.requireExtension("JSON");

Console.writeLn(JSON.encodeWithIndentation(JSON.decode("{\"item\":\"Some Item!\",\"work\":\"Some Work!\"}")));

function check(name, text, expected) {
	var value = JSON.decode(text);
	var result = Script.isUndefined(value) ? "undefined" : JSON.encode(value);
	if (result != expected) {
		Console.writeLn("decode [" + text + "] => " + result + ", expected " + expected);
		throw "test " + name + " failed";
	};
};

// numbers
check("01", "-1", "-1");
check("02", "[1,-1,-2.5]", "[1,-1,-2.5]");
check("03", "{\"a\":-5}", "{\"a\":-5}");
check("04", "1e3", "1000");
check("05", "[1.5e-3,2E+2,-1e2]", "[0.0015,200,-100]");
check("06", "0x10", "16");
check("07", "-0x10", "-16");
check("08", "[1e]", "undefined");
check("09", "[1e+]", "undefined");
check("10", "-", "undefined");
check("11", "[-]", "undefined");
check("12", "[-true]", "undefined");
check("13", "[+1]", "undefined");

// keywords
check("14", "true", "true");
check("15", "false", "false");
check("16", "null", "null");
check("17", "[true,false,null]", "[true,false,null]");
check("18", "{\"a\":true}", "{\"a\":true}");
check("19", "truex", "undefined");
check("20", "[tru]", "undefined");
check("21", "[tr[1]]", "undefined");

// structure
check("22", " { \"a\" : [ 1 , { \"b\" : \"x\" } ] } ", "{\"a\":[1,{\"b\":\"x\"}]}");
check("23", "[]", "[]");
check("24", "{}", "{}");
check("25", "[1,2", "undefined");
check("26", "{\"a\":}", "undefined");
check("27", "{a:1}", "undefined");
check("28", "", "undefined");
check("29", "/* c */ [1, // x\n 2]", "[1,2]");
check("30", "\"x\"", "\"x\"");

// round trip
var value = {a: -5, b: [-1.5, 1000000000000000000000, 0.0000001], c: true};
var text = JSON.encode(value);
if (JSON.encode(JSON.decode(text)) != text) {
	throw "test 31 failed";
};
