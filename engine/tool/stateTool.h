#pragma once

#include "tool.h"

namespace P3D::Engine {

typedef char ToolStatus;

class StateTool : public Tool {
private:
	ToolStatus toolStatus = 0;

protected:
	inline void setToolStatus(ToolStatus toolStatus) {
		this->toolStatus = toolStatus;
	}

	inline StateTool() : Tool() {}

public:
	inline ToolStatus getToolStatus() const {
		return toolStatus;
	};
};

};