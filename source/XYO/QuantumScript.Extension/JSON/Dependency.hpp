// Quantum Script
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_JSON_DEPENDENCY_HPP
#define XYO_QUANTUMSCRIPT_EXTENSION_JSON_DEPENDENCY_HPP

#ifndef XYO_QUANTUMSCRIPT_HPP
#	include <XYO/QuantumScript.hpp>
#endif

// -- Export

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_JSON_INTERNAL
#	ifdef QUANTUM_SCRIPT__JSON_INTERNAL
#		define XYO_QUANTUMSCRIPT_EXTENSION_JSON_INTERNAL
#	endif
#endif

#ifdef XYO_QUANTUMSCRIPT_EXTENSION_JSON_INTERNAL
#	define XYO_QUANTUMSCRIPT_EXTENSION_JSON_EXPORT XYO_LIBRARY_EXPORT
#else
#	define XYO_QUANTUMSCRIPT_EXTENSION_JSON_EXPORT XYO_LIBRARY_IMPORT
#endif

#endif