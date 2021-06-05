#pragma once

#include "../physics/cpuid.h"

#include "cmdParser.h"

#include <string>

namespace Util {
inline std::string printAndParseCPUIDArgs(const ParsedArgs& cmdArgs) {
	std::string message("Detected CPU Technologies: ");
	for(size_t i = 0; i < P3D::CPUIDCheck::TECHNOLOGY_COUNT; i++) {
		if(P3D::CPUIDCheck::hasTechnology(1U << i)) message.append(P3D::CPUIDCheck::NAMES[i]).append(" ");
	}

	bool disabledSomething = false;

	for(int techI = 0; techI < P3D::CPUIDCheck::TECHNOLOGY_COUNT; techI++) {
		if(cmdArgs.hasFlag(P3D::CPUIDCheck::NAMES[techI])) {
			P3D::CPUIDCheck::disableTechnology(1 << techI);
			message.append("\nDisabled technology -").append(P3D::CPUIDCheck::NAMES[techI]);
			disabledSomething = true;
		}
	}

	if(disabledSomething) {
		message.append("\nEnabled CPU Technologies: ");

		for(size_t i = 0; i < P3D::CPUIDCheck::TECHNOLOGY_COUNT; i++) {
			if(P3D::CPUIDCheck::hasTechnology(1U << i)) message.append(P3D::CPUIDCheck::NAMES[i]).append(" ");
		}
	}

	return message;
}

inline std::string printAndParseCPUIDArgs(int argc, const char** argv) {
	return printAndParseCPUIDArgs(ParsedArgs(argc, argv));
}
};
