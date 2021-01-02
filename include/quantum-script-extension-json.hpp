//
// Quantum Script Extension JSON
//
// Copyright (c) 2020-2021 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_EXTENSION_JSON_HPP
#define QUANTUM_SCRIPT_EXTENSION_JSON_HPP

#ifndef QUANTUM_SCRIPT_HPP
#include "quantum-script.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_JSON__EXPORT_HPP
#include "quantum-script-extension-json--export.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_JSON_COPYRIGHT_HPP
#include "quantum-script-extension-json-copyright.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_JSON_LICENSE_HPP
#include "quantum-script-extension-json-license.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_JSON_VERSION_HPP
#include "quantum-script-extension-json-version.hpp"
#endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace JSON {

				using namespace Quantum::Script;

				QUANTUM_SCRIPT_EXTENSION_JSON_EXPORT void initExecutive(Executive *executive, void *extensionId);
				QUANTUM_SCRIPT_EXTENSION_JSON_EXPORT void registerInternalExtension(Executive *executive);

			};
		};
	};
};

#endif

