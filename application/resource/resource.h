#pragma once

#pragma region ResourceAllocator

//! ResourceAllocator
template<typename T>
class ResourceAllocater {
public:
	virtual T* load(std::string filename) = 0;
};

#pragma endregion

#pragma region Resource

//! Resource
enum class ResourceType {
	None = 0,
	Texture,
	Shader,
	OBJ
};

class ResourceManager;

class Resource {
	friend ResourceManager;

private:
	std::string path;
	std::string name;

	inline void setName(const std::string& name);
	inline void setPath(const std::string& path);

protected:
	Resource(std::string path);
	Resource(std::string name, std::string path);

public:
	inline virtual ResourceType getType() const = 0;
	inline virtual std::string getTypeName() const = 0;

	inline std::string getName() const;
	inline std::string getPath() const;

	template<typename T>
	static ResourceAllocater<T>* getAllocator() {
		return T::getAllocator();
	}
};

#define DEFINE_RESOURCE(type, path) \
	inline static std::string getDefaultPath() { return path; } \
	inline static ResourceType getStaticType() { return ResourceType::type; } \
	inline virtual std::string getTypeName() const override { return #type; } \
	inline virtual ResourceType getType() const override { return getStaticType(); }

#pragma endregion