//
// Quantum Script Extension JSON
//
// Copyright (c) 2020-2021 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quantum-script-extension-json-license.hpp"
#include "quantum-script-extension-json.hpp"
#ifndef QUANTUM_SCRIPT_EXTENSION_JSON_NO_VERSION
#include "quantum-script-extension-json-version.hpp"
#endif

#include "quantum-script-variablenull.hpp"
#include "quantum-script-variableboolean.hpp"
#include "quantum-script-variablenumber.hpp"
#include "quantum-script-variablestring.hpp"

#include "quantum-script-extension-json.src"

//#define QUANTUM_SCRIPT_VM_DEBUG_RUNTIME

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace JSON {

				using namespace XYO;
				using namespace Quantum::Script;

				static bool jsonParseObject(Token &token, TPointer<Variable> &result);

				static bool jsonParseArray(Token &token, TPointer<Variable> &result) {
					if (token.is1("[")) {
						uint32_t index = 0;
						result = VariableArray::newVariable();
						while (!token.isEof()) {
							token.reset();
							if (token.is1("]")) {
								return true;
							};
							if (token.is1(",")) {
								continue;
							};
							if (token.isString()) {
								result->setPropertyByIndex(index++,VariableString::newVariable(token.value));
								continue;
							};
							if (token.isNumber()) {
								Number valueNumber;
								if(sscanf( token.value, QUANTUM_SCRIPT_FORMAT_NUMBER_INPUT, &valueNumber)!=1){
									valueNumber = 0;
								};
								result->setPropertyByIndex(index++,VariableNumber::newVariable(valueNumber));
								continue;
							};
							if (token.isSymbolX("true")) {
								result->setPropertyByIndex(index++,VariableBoolean::newVariable(true));
								continue;
							};
							if (token.isSymbolX("false")) {
								result->setPropertyByIndex(index++,VariableBoolean::newVariable(false));
								continue;
							};
							if (token.isSymbolX("null")) {
								result->setPropertyByIndex(index++,VariableNull::newVariable());
								continue;
							};
							if(token.checkIs1("{")) {
								TPointer<Variable> value_;
								if(jsonParseObject(token, value_)) {
									result->setPropertyByIndex(index++, value_);
									continue;
								};
								break;
							};
							if(token.checkIs1("[")) {
								TPointer<Variable> value_;
								if(jsonParseArray(token, value_)) {
									result->setPropertyByIndex(index++, value_);
									continue;
								};
								break;
							};
							break;
						};
					};
					return false;
				};


				static bool jsonParseObject(Token &token, TPointer<Variable> &result) {
					if (token.is1("{")) {
						result = VariableObject::newVariable();
						while (!token.isEof()) {
							token.reset();
							if (token.is1("}")) {
								return true;
							};
							if (token.is1(",")) {
								continue;
							};
							if (token.isString()) {
								String key_ = token.value;
								if (token.is1(":")) {
									token.reset();
									if (token.isString()) {
										TPointer<Variable> keyV = VariableString::newVariable(key_);
										result->setPropertyByVariable(keyV,VariableString::newVariable(token.value));
										continue;
									};
									if (token.isNumber()) {
										TPointer<Variable> keyV = VariableString::newVariable(key_);
										Number valueNumber;
										if(sscanf(token.value, QUANTUM_SCRIPT_FORMAT_NUMBER_INPUT, &valueNumber) != 1){
											valueNumber = 0;
										};
										result->setPropertyByVariable(keyV,VariableNumber::newVariable(valueNumber));
										continue;
									};
									if (token.isSymbolX("true")) {
										TPointer<Variable> keyV = VariableString::newVariable(key_);
										result->setPropertyByVariable(keyV,VariableBoolean::newVariable(true));
										continue;
									};
									if (token.isSymbolX("false")) {
										TPointer<Variable> keyV = VariableString::newVariable(key_);
										result->setPropertyByVariable(keyV,VariableBoolean::newVariable(false));
										continue;
									};
									if (token.isSymbolX("null")) {
										TPointer<Variable> keyV = VariableString::newVariable(key_);
										result->setPropertyByVariable(keyV,VariableNull::newVariable());
										continue;
									};
									if(token.checkIs1("{")) {
										TPointer<Variable> value_;
										if(jsonParseObject(token, value_)) {
											TPointer<Variable> keyV = VariableString::newVariable(key_);
											result->setPropertyByVariable(keyV, value_);
											continue;
										};
										break;
									};
									if(token.checkIs1("[")) {
										TPointer<Variable> value_;
										if(jsonParseArray(token, value_)) {
											TPointer<Variable> keyV = VariableString::newVariable(key_);
											result->setPropertyByVariable(keyV, value_);
											continue;
										};
										break;
									};
								};
							};
							break;
						};
					};
					return false;
				};

				static bool jsonDecodeX(Token &token, TPointer<Variable> &result) {
					if (token.read()) {
						if(token.checkIs1("{")) {
							return jsonParseObject(token, result);
						};
						if(token.checkIs1("[")) {
							return jsonParseArray(token, result);
						};
						if (token.isString()) {
							result = VariableString::newVariable(token.value);
							return true;
						};
						if (token.isNumber()) {
							Number valueNumber;
							if(sscanf(token.value, QUANTUM_SCRIPT_FORMAT_NUMBER_INPUT, &valueNumber) != 1){
								valueNumber = 0;
							};
							result = VariableNumber::newVariable(valueNumber);
							return true;
						};
						if (token.isSymbolX("true")) {
							result = VariableBoolean::newVariable(true);
							return true;
						};
						if (token.isSymbolX("false")) {
							result = VariableBoolean::newVariable(false);
							return true;
						};
						if (token.isSymbolX("null")) {
							result = VariableNull::newVariable();
							return true;
						};
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
							if(!jsonDecodeX(token, retV)) {
								in.close();
								return Context::getValueUndefined();
							};
						};
						in.close();
					};
					return retV;
				};


				static TPointer<Variable> jsonDecode(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- json-decode\n");
#endif
					return jsonDecode_((arguments->index(0))->toString());
				};

				void registerInternalExtension(Executive *executive) {
					executive->registerInternalExtension("JSON", initExecutive);
				};

				void initExecutive(Executive *executive, void *extensionId) {

					String info = "JSON\r\n";
					info << License::shortContent();

					executive->setExtensionName(extensionId, "JSON");
					executive->setExtensionInfo(extensionId, info);
#ifndef QUANTUM_SCRIPT_EXTENSION_JSON_NO_VERSION
					executive->setExtensionVersion(extensionId, Extension::JSON::Version::versionWithBuild());
#endif
					executive->setExtensionPublic(extensionId, true);


					executive->compileStringX("var JSON={};");
					executive->setFunction2("JSON.decode(x)", jsonDecode);
					executive->compileStringX(extensionJSONSource);
				};


			};
		};
	};
};

#ifdef XYO_COMPILE_DYNAMIC_LIBRARY
extern "C" QUANTUM_SCRIPT_EXTENSION_JSON_EXPORT void quantumScriptExtension(Quantum::Script::Executive *executive, void *extensionId) {
	Quantum::Script::Extension::JSON::initExecutive(executive, extensionId);
};
#endif

