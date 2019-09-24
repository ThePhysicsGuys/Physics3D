#include "resource.h"

#include "core.h"

#include "resourceManager.h"

inline void Resource::setName(const std::string& name) {
	ResourceManager::onResourceNameChange(this, name);
}

inline void Resource::setPath(const std::string& path) {
	ResourceManager::onResourcePathChange(this, path);
}

inline std::string Resource::getName() const {
	return name;
}

inline std::string Resource::getPath() const {
	return path;
}

Resource::Resource(std::string path) {
	this->name = path;
	this->path = path;
}

Resource::Resource(std::string name, std::string path) {
	this->name = name;
	this->path = path;
}