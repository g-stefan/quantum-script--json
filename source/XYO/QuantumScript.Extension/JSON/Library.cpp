// Quantum Script Extension JSON
// Copyright (c) 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/QuantumScript.Extension/JSON/Library.hpp>
#include <XYO/QuantumScript.Extension/JSON/Copyright.hpp>
#include <XYO/QuantumScript.Extension/JSON/License.hpp>
#include <XYO/QuantumScript.Extension/JSON/Version.hpp>

#include <XYO/QuantumScript.Extension/JSON/Library.Source.cpp>

namespace XYO::QuantumScript::Extension::JSON {

	static bool jsonParseValue(Token &token, TPointer<Variable> &result);

	static bool jsonIsDigit(char ch) {
		return (ch >= '0') && (ch <= '9');
	};

	static bool jsonIsHexDigit(char ch) {
		return jsonIsDigit(ch) || ((ch >= 'A') && (ch <= 'F')) || ((ch >= 'a') && (ch <= 'f'));
	};

	static bool jsonIsSymbolChar(char ch) {
		return (ch == '_') || ((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z')) || jsonIsDigit(ch);
	};

	// [-] digits [. digits] [(e|E) [+|-] digits], or [-] 0x hexdigits
	static bool jsonParseNumber(Token &token, TPointer<Variable> &result) {
		String value;
		Number valueNumber;

		if (token.input->inputChar == '-') {
			value << '-';
			token.input->read();
		};
		if (!jsonIsDigit(token.input->inputChar)) {
			return false;
		};
		if (token.input->inputChar == '0') {
			value << '0';
			token.input->read();
			if ((token.input->inputChar == 'x') || (token.input->inputChar == 'X')) {
				value << 'x';
				token.input->read();
				if (!jsonIsHexDigit(token.input->inputChar)) {
					return false;
				};
				while (jsonIsHexDigit(token.input->inputChar)) {
					value << token.input->inputChar;
					token.input->read();
				};
			};
		};
		while (jsonIsDigit(token.input->inputChar)) {
			value << token.input->inputChar;
			token.input->read();
		};
		if (token.input->inputChar == '.') {
			value << '.';
			token.input->read();
			while (jsonIsDigit(token.input->inputChar)) {
				value << token.input->inputChar;
				token.input->read();
			};
		};
		if ((token.input->inputChar == 'e') || (token.input->inputChar == 'E')) {
			value << 'e';
			token.input->read();
			if ((token.input->inputChar == '+') || (token.input->inputChar == '-')) {
				value << token.input->inputChar;
				token.input->read();
			};
			if (!jsonIsDigit(token.input->inputChar)) {
				return false;
			};
			while (jsonIsDigit(token.input->inputChar)) {
				value << token.input->inputChar;
				token.input->read();
			};
		};
		token.ignoreSpaceOrComment();

		if (sscanf(value, XYO_QUANTUMSCRIPT_FORMAT_NUMBER_INPUT, &valueNumber) != 1) {
			return false;
		};
		result = VariableNumber::newVariable(valueNumber);
		return true;
	};

	// true / false / null, also as the last characters of the input
	static bool jsonIsKeyword(Token &token, const char *name) {
		int k;
		for (k = 0; name[k] != 0; ++k) {
			if (token.input->inputChar != name[k]) {
				return false;
			};
			token.input->read();
		};
		if (jsonIsSymbolChar(token.input->inputChar)) {
			return false;
		};
		token.ignoreSpaceOrComment();
		return true;
	};

	static bool jsonParseArray(Token &token, TPointer<Variable> &result) {
		if (token.is1("[")) {
			uint32_t index = 0;
			result = VariableArray::newVariable();
			while (!token.isEof()) {
				if (token.is1("]")) {
					return true;
				};
				if (token.is1(",")) {
					continue;
				};
				TPointer<Variable> value_;
				if (!jsonParseValue(token, value_)) {
					break;
				};
				result->setPropertyByIndex(index++, value_);
			};
		};
		return false;
	};

	static bool jsonParseObject(Token &token, TPointer<Variable> &result) {
		if (token.is1("{")) {
			result = VariableObject::newVariable();
			while (!token.isEof()) {
				if (token.is1("}")) {
					return true;
				};
				if (token.is1(",")) {
					continue;
				};
				if ((token.input->inputChar != '"') && (token.input->inputChar != '\'')) {
					break;
				};
				token.reset();
				if (!token.isString()) {
					break;
				};
				String key_ = token.value;
				if (!token.is1(":")) {
					break;
				};
				TPointer<Variable> value_;
				if (!jsonParseValue(token, value_)) {
					break;
				};
				TPointer<Variable> keyV = VariableString::newVariable(key_);
				result->setPropertyByVariable(keyV, value_);
			};
		};
		return false;
	};

	// the first character selects the value type, a value that does not parse is an error
	static bool jsonParseValue(Token &token, TPointer<Variable> &result) {
		char ch = token.input->inputChar;
		if (ch == '{') {
			return jsonParseObject(token, result);
		};
		if (ch == '[') {
			return jsonParseArray(token, result);
		};
		if ((ch == '"') || (ch == '\'')) {
			token.reset();
			if (token.isString()) {
				result = VariableString::newVariable(token.value);
				return true;
			};
			return false;
		};
		if ((ch == '-') || jsonIsDigit(ch)) {
			return jsonParseNumber(token, result);
		};
		if (ch == 't') {
			if (jsonIsKeyword(token, "true")) {
				result = VariableBoolean::newVariable(true);
				return true;
			};
			return false;
		};
		if (ch == 'f') {
			if (jsonIsKeyword(token, "false")) {
				result = VariableBoolean::newVariable(false);
				return true;
			};
			return false;
		};
		if (ch == 'n') {
			if (jsonIsKeyword(token, "null")) {
				result = VariableNull::newVariable();
				return true;
			};
			return false;
		};
		return false;
	};

	static bool jsonDecodeX(Token &token, TPointer<Variable> &result) {
		if (token.read()) {
			return jsonParseValue(token, result);
		};
		return false;
	};

	static TPointer<Variable> jsonDecode_(String data_) {
		Input input;
		Token token;
		MemoryRead in;
		uint32_t sourceLineNumber;
		TPointer<Variable> retV = Context::getValueUndefined();

		if (in.open((char *)data_.value(), data_.length())) {
			if (input.init(&in)) {
				token.input = &input;
				token.sourceLineNumber = &sourceLineNumber;
				if (!jsonDecodeX(token, retV)) {
					in.close();
					return Context::getValueUndefined();
				};
			};
			in.close();
		};
		return retV;
	};

	static TPointer<Variable> jsonDecode(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- json-decode\n");
#endif
		return jsonDecode_((arguments->index(0))->toString());
	};

	void registerInternalExtension(Executive *executive) {
		executive->registerInternalExtension("JSON", initExecutive);
	};

	void initExecutive(Executive *executive, void *extensionId) {

		String info = "JSON\r\n";
		info << License::shortLicense().c_str();

		executive->setExtensionName(extensionId, "JSON");
		executive->setExtensionInfo(extensionId, info);
		executive->setExtensionVersion(extensionId, Extension::JSON::Version::versionWithBuild());
		executive->setExtensionPublic(extensionId, true);

		executive->compileStringX("var JSON={};");
		executive->setFunction2("JSON.decode(x)", jsonDecode);
		executive->compileStringX(librarySource);
	};

};

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_JSON_LIBRARY
#	ifdef XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY
extern "C" XYO_QUANTUMSCRIPT_EXTENSION_JSON_EXPORT void quantumScriptExtension(XYO::QuantumScript::Executive *executive, void *extensionId) {
	XYO::QuantumScript::Extension::JSON::initExecutive(executive, extensionId);
};
#	endif
#endif
