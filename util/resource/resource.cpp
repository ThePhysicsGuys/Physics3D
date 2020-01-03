#include "resource.h"

#include "resourceManager.h"

void Resource::setName(const std::string& name) {
	ResourceManager::onResourceNameChange(this, name);
}

void Resource::setPath(const std::string& path) {
	ResourceManager::onResourcePathChange(this, path);
}

std::string Resource::getName() const {
	return name;
}

std::string Resource::getPath() const {
	return path;
}

Resource::Resource(const std::string& path) {
	this->name = path;
	this->path = path;
}

Resource::Resource(const std::string& name, const std::string& path) {
	this->name = name;
	this->path = path;
}