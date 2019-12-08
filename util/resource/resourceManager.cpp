#include "resourceManager.h"

std::unordered_map<ResourceType, Resource*> ResourceManager::defaultResources = {};
std::unordered_map<std::string, ResourceManager::CountedResource> ResourceManager::resources = {};

ResourceManager::ResourceManager() {

}

ResourceManager::~ResourceManager() {
	ResourceManager::close();
}