//
// Quantum Script Extension JSON
//
// Copyright (c) 2020-2022 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_EXTENSION_JSON_COPYRIGHT_HPP
#define QUANTUM_SCRIPT_EXTENSION_JSON_COPYRIGHT_HPP

#define QUANTUM_SCRIPT_EXTENSION_JSON_COPYRIGHT            "Copyright (c) Grigore Stefan"
#define QUANTUM_SCRIPT_EXTENSION_JSON_PUBLISHER            "Grigore Stefan"
#define QUANTUM_SCRIPT_EXTENSION_JSON_COMPANY              QUANTUM_SCRIPT_EXTENSION_JSON_PUBLISHER
#define QUANTUM_SCRIPT_EXTENSION_JSON_CONTACT              "g_stefan@yahoo.com"
#define QUANTUM_SCRIPT_EXTENSION_JSON_FULL_COPYRIGHT       QUANTUM_SCRIPT_EXTENSION_JSON_COPYRIGHT " <" QUANTUM_SCRIPT_EXTENSION_JSON_CONTACT ">"

#ifndef XYO_RC

#ifndef QUANTUM_SCRIPT_EXTENSION_JSON__EXPORT_HPP
#include "quantum-script-extension-json--export.hpp"
#endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace JSON {
				namespace Copyright {
					QUANTUM_SCRIPT_EXTENSION_JSON_EXPORT const char *copyright();
					QUANTUM_SCRIPT_EXTENSION_JSON_EXPORT const char *publisher();
					QUANTUM_SCRIPT_EXTENSION_JSON_EXPORT const char *company();
					QUANTUM_SCRIPT_EXTENSION_JSON_EXPORT const char *contact();
					QUANTUM_SCRIPT_EXTENSION_JSON_EXPORT const char *fullCopyright();
				};
			};
		};
	};
};

#endif
#endif
