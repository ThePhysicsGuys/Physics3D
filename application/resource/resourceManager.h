#pragma once

#include <typeinfo>

#include "resource.h"

class ResourceManager {
	friend Resource;

private:
	struct CountedResource {
		Resource* value;
		int count;
	};

	static std::unordered_map<ResourceType, Resource*> defaultResources;
	static std::unordered_map<std::string, CountedResource> resources;

	static void onResourceNameChange(Resource* changedResource, const std::string& newName) {
		auto iterator = ResourceManager::resources.find(changedResource->getName());

		if (iterator == ResourceManager::resources.end()) {
			Log::error("The changed resource is not found (%s)", changedResource->getName().c_str());
			return;
		} else {
			if (ResourceManager::resources.find(newName) != ResourceManager::resources.end()) {
				Log::error("Resource name has already been taken (%s)", newName.c_str());
				return;
			} else {
				CountedResource countedResource = iterator->second;
				countedResource.value->name = newName;
				ResourceManager::resources.emplace(newName, countedResource);
				ResourceManager::resources.erase(iterator);
			}
		}
	}

	static void onResourcePathChange(Resource* changedResource, const std::string& newPath) {
		auto iterator = ResourceManager::resources.find(changedResource->getName());

		if (iterator == ResourceManager::resources.end()) {
			Log::error("The changed resource is not found (%s)", changedResource->getName().c_str());
			return;
		} else {
			auto& resource = iterator->second.value;

			if (resource->getName() == resource->getPath()) {
				onResourceNameChange(changedResource, newPath);	
			} else {
				//? Load new resource
			}

			resource->path = newPath;
		}
	}

	template<typename T>
	static T* getDefaultResource() {
		//Log::subject("DEFAULT");
		//Log::debug("Getting default resource: (%s)", T::getStaticTypeName().c_str());
		auto iterator = ResourceManager::defaultResources.find(T::getStaticType());

		if (iterator != ResourceManager::defaultResources.end()) {
			//Log::debug("Found default resource: (%s)", T::getStaticTypeName().c_str());
			return static_cast<T*>(iterator->second);
		} else {
			//Log::debug("Default resource not found: (%s), trying to load default resource", T::getStaticTypeName().c_str());
			Resource* defaultResource = ResourceManager::add<T>(T::getDefaultPath(), "default_" + T::getStaticTypeName());

			if (defaultResource) {
				//Log::debug("Loaded default resource; (%s)", T::getStaticTypeName().c_str());
				ResourceManager::defaultResources[T::getStaticType()] = defaultResource;
				return static_cast<T*>(defaultResource);
			} else {
				//Log::debug("Default resource not loaded: (%s)", T::getStaticTypeName().c_str());
				return nullptr;
			}
		}
	}

	ResourceManager();
	~ResourceManager();

public:
	template<typename T> 
	static T* get(const std::string& name) {
		//Log::subject s("GET");
		//Log::debug("Getting resource: (%s)", name.c_str());

		auto iterator = ResourceManager::resources.find(name);

		if (iterator != ResourceManager::resources.end()) {
			//Log::debug("Found resource: (%s)", name.c_str());

			return static_cast<T*>(iterator->second.value);
		} else {
			//Log::warn("Resource not loaded: (%s), using default resource", name.c_str());
			return ResourceManager::getDefaultResource<T>();
		}
	}

	template<typename T>
	static T* add(const std::string& name, const std::string& path) {
		Log::subject s("ADD");
		//Log::debug("Adding resource: (%s, %s)", name.c_str(), path.c_str());

		auto iterator = ResourceManager::resources.find(name);

		if (iterator != ResourceManager::resources.end()) {
			//Log::debug("Found resource: (%s, %s)", name.c_str(), path.c_str());
			auto& resource = (*iterator).second;
			resource.count++; 
			return static_cast<T*>(resource.value);
		} else {
			//Log::debug("Resource not found: (%s, %s), trying to load resource", name.c_str(), path.c_str());

			T* resource = T::getAllocator().load(name, path);

			if (resource == nullptr) {
				Log::warn("Resource not loaded: (%s, %s)", name.c_str(), path.c_str());
				return nullptr;
			} else {
				//Log::debug("Loaded resource: (%s, %s)", name.c_str(), path.c_str());

				CountedResource countedResource = { resource, 1 };
				ResourceManager::resources.emplace(name, countedResource);

				return resource;
			}
		}
	}

	template<typename T>
	static T* add(const std::string& path) {
		return add<T>(path, path);
	}

	static bool exists(const std::string& name) {
		auto iterator = ResourceManager::resources.find(name);
		return iterator != ResourceManager::resources.end();
	}

	template<typename T>
	static std::vector<T*> getResourcesOfClass() {
		std::vector<T*> list;

		for (auto iterator : ResourceManager::resources) {
			if (typeid(T) == typeid(iterator.second.value))
				list.push_back(static_cast<T*>(iterator.second.value));
		}

		return list;
	}

	static std::vector<Resource*> getResourcesOfType(ResourceType type) {
		std::vector<Resource*> list;
		
		for (auto iterator : ResourceManager::resources) {
			if (iterator.second.value->getType() == type)
				list.push_back(iterator.second.value);
		}

		return list;
	}

	static std::vector<Resource*> getResources() {
		std::vector<Resource*> list;

		for (auto iterator : ResourceManager::resources) {
			list.push_back(iterator.second.value);
		}

		return list;
	}
};