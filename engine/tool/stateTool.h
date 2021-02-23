#pragma once

#include "tool.h"

namespace P3D::Engine {

typedef char ToolStatus;

class StateTool : public Tool {
private:
	ToolStatus toolStatus = 0;

protected:
	void setToolStatus(ToolStatus toolStatus) {
		this->toolStatus = toolStatus;
	}

	StateTool() : Tool() {}

public:
	ToolStatus getToolStatus() const {
		return toolStatus;
	}
};

};