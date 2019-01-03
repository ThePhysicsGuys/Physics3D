#include "mathUtil.h"

std::ostream& operator<<(std::ostream& os, const CFrame& cframe) {
	os << "CFrame(" << cframe.position << ", " << cframe.rotation << ")";
	return os;
}

std::string str(const CFrame& cframe) {
	std::stringstream ss;
	ss << cframe;
	return ss.str();
}