// Quantum Script Extension JSON
// Copyright (c) 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

JSON.encode = function(x) {
	var encode_ = self;
	var encodeObject = function(o) {
		var retV = "{";
		var coma = "";
		for(var k in o) {
			retV += coma + Convert.toStringSource(k) + ":" + encode_(o[k]);
			coma = ",";
		};
		return retV + "}";
	};
	var encodeArray = function(o) {
		var retV = "[";
		var coma = "";
		for(var k in o) {
			retV += coma + encode_(o[k]);
			coma = ",";
		};
		return retV + "]";
	};
	if(Script.isNull(x)) {
		return "null";
	};
	if(Script.isBoolean(x)) {
		return Convert.toString(x);
	};
	if(Script.isNumber(x)) {
		if(Script.isNaN(x)) {
			return "\"NaN\"";
		};
		if(Script.isInfinite(x)) {
			if(Script.signBit(x)) {
				return "\"-Infinity\"";
			} else {
				return "\"Infinity\"";
			};
		};
		return Convert.toString(x);
	};
	if(Script.isString(x)) {
		return Convert.toStringSource(x);
	};
	if(Script.isArray(x)) {
		return encodeArray(x);
	};
	if(Script.isObject(x)) {
		return encodeObject(x);
	};
	if(Script.isAssociativeArray(x)) {
		return encodeObject(x);
	};
	return "null";
};

JSON.encodeWithIndentation = function(x, step) {
	var encode_ = self;
	var ident_ = "";
	step = Convert.toNumber(step);
	if(Script.isNaN(step) || Script.isInfinite(step) || Script.signBit(step)) {
		step = 0;
	};
	for(var k = 0; k < step; ++k) {
		ident_ += "\t";
	};
	var encodeObject = function(o) {
		var retV = "{\n";
		var coma = "";
		for(var k in o) {
			retV += coma + ident_ + "\t" + Convert.toStringSource(k) + ": " + encode_(o[k], step + 1);
			coma = ",\n";
		};
		return retV + "\n" + ident_ + "}";
	};
	var encodeArray = function(o) {
		var retV = "[\n";
		var coma = "";
		for(var k in o) {
			retV += coma + ident_ + "\t" + encode_(o[k], step + 1);
			coma = ",\n";
		};
		return retV + "\n" + ident_ + "]";
	};
	if(Script.isNull(x)) {
		return "null";
	};
	if(Script.isBoolean(x)) {
		return Convert.toString(x);
	};
	if(Script.isNumber(x)) {
		if(Script.isNaN(x)) {
			return "\"NaN\"";
		};
		if(Script.isInfinite(x)) {
			if(Script.signBit(x)) {
				return "\"-Infinity\"";
			} else {
				return "\"Infinity\"";
			};
		};
		return Convert.toString(x);
	};
	if(Script.isString(x)) {
		return Convert.toStringSource(x);
	};
	if(Script.isArray(x)) {
		return encodeArray(x);
	};
	if(Script.isObject(x)) {
		return encodeObject(x);
	};
	if(Script.isAssociativeArray(x)) {
		return encodeObject(x);
	};
	return "null";
};


