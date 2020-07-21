#pragma once
	
#include <cstdint>
#include <map>
#include <stack>

namespace Engine {

template<typename, typename = void>
struct registry_traits;


template<typename Type>
struct registry_traits<Type, std::enable_if_t<std::is_enum_v<Type>>> : registry_traits<std::underlying_type_t<Type>> {};


/**
 * entity = 3 parent bits, 3 self bits, 2 version bits
 */
template<>
struct registry_traits<std::uint8_t> {
	using entity_type = std::uint8_t;
	using version_type = std::uint8_t;
	using componentmap_type = std::uint8_t;

	static constexpr entity_type entity_mask = 0x7;
	static constexpr version_type version_mask = 0x3;
	static constexpr std::size_t parent_shift = 5u;
	static constexpr std::size_t self_shift = 2u;
};

/**
 * entity = 6 parent bits, 6 self bits, 4 version bits
 */
template<>
struct registry_traits<std::uint16_t> {
	using entity_type = std::uint16_t;
	using version_type = std::uint8_t;
	using componentmap_type = std::uint16_t;

	static constexpr entity_type entity_mask = 0x3F;
	static constexpr version_type version_mask = 0xF;
	static constexpr std::size_t parent_shift = 10u;
	static constexpr std::size_t self_shift = 4u;
};

/**
 * entity = 12 parent bits, 12 self bits, 8 version bits
 */
template<>
struct registry_traits<std::uint32_t> {
	using entity_type = std::uint32_t;
	using version_type = std::uint8_t;
	using componentmap_type = std::uint32_t;

	static constexpr entity_type entity_mask = 0xFFF;
	static constexpr version_type version_mask = 0xFF;
	static constexpr std::size_t parent_shift = 20u;
	static constexpr std::size_t self_shift = 8u;
};

/**
 * entity = 28 parent bits, 28 self bits, 8 version bits
 */
template<>
struct registry_traits<std::uint64_t> {
	using entity_type = std::uint64_t;
	using version_type = std::uint8_t;
	using componentmap_type = std::uint64_t;

	static constexpr entity_type entity_mask = 0xFFFFFFF;
	static constexpr version_type version_mask = 0xFF;
	static constexpr std::size_t parent_shift = 36u;
	static constexpr std::size_t self_shift = 8u;
};

template<typename Entity>
constexpr auto integral(const Entity& entity) {
	return static_cast<typename registry_traits<Entity>::entity_type>(entity);
}

template<typename Entity>
inline constexpr auto version(const Entity& entity) {
	using traits_type = registry_traits<Entity>;
	using version_type = typename traits_type::version_type;
	return static_cast<version_type>(integral(entity)) & traits_type::version_mask;
}

template<typename Entity>
inline constexpr auto self(const Entity& entity) -> typename registry_traits<Entity>::entity_type {
	using traits_type = registry_traits<Entity>;
	return (integral(entity) >> traits_type::self_shift) & traits_type::entity_mask;
}

template<typename Entity>
inline constexpr auto parent(const Entity& entity) -> typename registry_traits<Entity>::entity_type {
	using traits_type = registry_traits<Entity>;
	return integral(entity) >> traits_type::parent_shift;
}

template<typename Entity, typename Version>
inline constexpr Entity merge(const Entity& parent, const Entity& entity, const Version& version) {
	using traits_type = registry_traits<Entity>;
	return (integral(parent) << traits_type::parent_shift)  | 
		   (integral(entity) << traits_type::self_shift) | 
		   static_cast<Entity>(version);
}

template<typename Type>
struct type_index {
	static Type next() {
		static Type value {};
		return value++;
	}
};

template<typename Entity, typename Component = void>
struct componentmap_index {
	using traits_type = registry_traits<Entity>;
	using componentmap_type = typename traits_type::componentmap_type;

	static char value() {
		static const char value = static_cast<char>(type_index<char>::next());
		return value;
	}

	static constexpr componentmap_type mask() {
		const componentmap_type value = static_cast<componentmap_type>(1u << componentmap_index<Entity, Component>::value());
		return value;
	}
};

template<typename Entity>
struct componentmap_index<Entity, void> {
	using traits_type = registry_traits<Entity>;
	using componentmap_type = typename traits_type::componentmap_type;

	static componentmap_type count() {
		static const componentmap_type value = type_index<componentmap_type>::next();
		return value;
	}
};


template<typename Entity>
struct Registry {
	using traits_type = registry_traits<Entity>;
	using entity_type = typename traits_type::entity_type;
	using version_type = typename traits_type::version_type;
	using componentmap_type = typename traits_type::componentmap_type;

	/*constexpr*/ entity_type null_entity = static_cast<entity_type>(0u);
	/*constexpr*/ version_type null_version = static_cast<version_type>(0u);
	/*constexpr*/ componentmap_type null_componentmap = static_cast<componentmap_type>(0u);

private:
	// Components
	std::map<entity_type, componentmap_type> entities;
	
	// ID
	std::stack<entity_type> id_stack;
	entity_type id_counter = null_entity;

	constexpr entity_type nextID() {
		return ++id_counter;
	}

	constexpr version_type nextVersion(const version_type& version) {
		return version + static_cast<version_type>(1u);
	}

public:
	Entity create() {
		Entity id;
		if (id_stack.empty()) {
			id = merge(null_entity, nextID(), null_version);
		} else {
			Entity lastID = id_stack.top();
			version_type lastVersion = version(lastID);
			version_type newVersion = nextVersion(lastVersion);

			id = merge(null_entity, self(lastID), newVersion);

			id_stack.pop();
		}

		entities.emplace(id, null_componentmap);

		return id;
	}
	
	void destroy(const Entity& entity) {
		auto iterator = entities.find(entity);

		if (iterator != entities.end()) {
			entities.erase(iterator);
			id_stack.push(entity);
		}
	}

	template<typename Component, typename... Args>
	void add(const Entity& entity, Args&&... args) {
		auto iterator = entities.find(entity);
		if (iterator != entities.end()) {
			Component* component = new Component(std::forward<Args>(args)...);
			componentmap_type mask = componentmap_index<Entity, Component>::mask();

			// Update component bitmap
			iterator->second |= mask;

			// todo add ptr to memory
		}
	}

	template<typename Component>
	void remove(const Entity& entity) {
		auto iterator = entities.find(entity);
		if (iterator != entities.end()) {
			componentmap_type mask = componentmap_index<Entity, Component>::mask();
			
			// Update component bitmap
			iterator->second = iterator->second & ~mask;

			// todo remove ptr to memory
		}
	}

};

typedef Registry<std::uint8_t>  Registry8;
typedef Registry<std::uint16_t> Registry16;
typedef Registry<std::uint32_t> Registry32;
typedef Registry<std::uint64_t> Registry64;

};

