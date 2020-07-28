#include <cstdint>
#include <map>
#include <set>
#include <stack>
#include <vector>
#include <functional>
#include <type_traits>

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
	using component_type = std::uint16_t;

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
	using entity_type = std::uint8_t;
	using version_type = std::uint8_t;
	using component_type = std::uint16_t;

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
	using entity_type = std::uint16_t;
	using version_type = std::uint8_t;
	using component_type = std::uint16_t;

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
	using entity_type = std::uint32_t;
	using version_type = std::uint8_t;
	using component_type = std::uint16_t;

	static constexpr entity_type entity_mask = 0xFFFFFFF;
	static constexpr version_type version_mask = 0xFF;
	static constexpr std::size_t parent_shift = 36u;
	static constexpr std::size_t self_shift = 8u;
};

template<typename Entity>
inline constexpr auto version(const Entity& entity) -> typename registry_traits<Entity>::version_type {
	using traits_type = registry_traits<Entity>;
	using version_type = typename traits_type::version_type;
	return static_cast<version_type>(entity) & traits_type::version_mask;
}

template<typename Entity>
inline constexpr auto self(const Entity& entity) -> typename registry_traits<Entity>::entity_type {
	using traits_type = registry_traits<Entity>;
	using entity_type = typename traits_type::entity_type;
	return static_cast<entity_type>(entity >> traits_type::self_shift) & traits_type::entity_mask;
}

template<typename Entity>
inline constexpr auto parent(const Entity& entity) -> typename registry_traits<Entity>::entity_type {
	using traits_type = registry_traits<Entity>;
	using entity_type = typename traits_type::entity_type;
	return static_cast<entity_type>(entity >> traits_type::parent_shift) & traits_type::entity_mask;
}

template<typename Entity>
inline constexpr auto merge(const typename registry_traits<Entity>::entity_type& parent, const typename registry_traits<Entity>::entity_type& entity, const typename registry_traits<Entity>::version_type& version) -> Entity {
	using traits_type = registry_traits<Entity>;
	return (static_cast<Entity>(parent) << traits_type::parent_shift) |
		(static_cast<Entity>(parent) << traits_type::self_shift) |
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
struct component_index {
	using traits_type = registry_traits<Entity>;
	using component_type = typename traits_type::component_type;

	static component_type index() {
		static const component_type value = type_index<component_type>::next();
		return value;
	}
};

template<typename Entity>
struct component_index<Entity, void> {
	using traits_type = registry_traits<Entity>;
	using component_type = typename traits_type::component_type;

	static component_type count() {
		static const component_type value = type_index<component_type>::next();
		return value;
	}
};

template<typename Entity>
class Registry {
	using registry_type = Registry<Entity>;
	using traits_type = registry_traits<Entity>;
	using entity_type = typename traits_type::entity_type;
	using version_type = typename traits_type::version_type;
	using component_type = typename traits_type::component_type;

	using entity_stack = std::stack<entity_type>;
	using entity_set = std::set<entity_type>;
	using entity_map = std::map<entity_type, void*>;
	using component_vector = std::vector<entity_map*>;

	using entity_iterator = decltype(std::declval<entity_map>().begin());

	entity_type null_entity = static_cast<entity_type>(0u);
	version_type null_version = static_cast<version_type>(0u);

private:
	// Components
	entity_set entities;
	component_vector components;

	// ID
	entity_stack id_stack;
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
			id = merge<Entity>(null_entity, nextID(), null_version);
		} else {
			Entity lastID = id_stack.top();
			version_type lastVersion = version(lastID);
			version_type newVersion = nextVersion(lastVersion);

			id = merge<Entity>(null_entity, self(lastID), newVersion);

			id_stack.pop();
		}

		entities.insert(id);

		return id;
	}

	void destroy(const Entity& entity) {
		auto entities_iterator = entities.find(entity);

		if (entities_iterator != entities.end()) {
			entities.erase(entities_iterator);
			id_stack.push(entity);


			// erase from components
			// add to destroy stack?
		}
	}

	template<typename Component, typename... Args>
	void add(const Entity& entity, Args&&... args) {
		auto entities_iterator = entities.find(self(entity));
		if (entities_iterator == entities.end())
			return;

		Component* component = new Component(std::forward<Args>(args)...);
		component_type index = component_index<Entity, Component>::index();

		while (index >= components.size())
			components.push_back(new entity_map());

		entity_map* map = components[index];
		if (map == nullptr)
			return;

		map->insert(std::pair<entity_type, void*>(self(entity), static_cast<void*>(component)));
	}

	template<typename Component>
	void remove(const Entity& entity) {
		auto entities_iterator = entities.find(entity);
		if (entities_iterator == entities.end())
			return;

		component_type index = component_index<Entity, Component>::index();
		if (index > components.size())
			return;

		entity_map* map = components[index];
		if (map == nullptr)
			return;

		auto component_iterator = map->find(self(entity));
		if (component_iterator == map->end())
			return;

		map->erase(component_iterator);
	}

	template<typename Component>
	Component* get(const Entity& entity) {
		auto entities_iterator = entities.find(entity);
		if (entities_iterator == entities.end())
			return nullptr;

		component_type index = component_index<Entity, Component>::index();
		if (index > components.size())
			return nullptr;

		entity_map* map = components[index];
		if (map == nullptr)
			return nullptr;

		auto component_iterator = map->find(self(entity));
		if (component_iterator == map->end())
			return nullptr;

		return static_cast<Component*>(component_iterator->second);
	}

	template<typename Component>
	bool has(const Entity& entity) {
		auto entities_iterator = entities.find(entity);
		if (entities_iterator == entities.end())
			return false;

		component_type index = component_index<Entity, Component>::index();
		if (index > components.size())
			return false;

		entity_map* map = components[index];
		if (map == nullptr)
			return false;

		auto component_iterator = map->find(self(entity));
		if (component_iterator == map->end())
			return false;

		return true;
	}


	struct view_iterator_end {};

	template<typename Iterator, typename IteratorEnd, typename Filter>
	class view_iterator {
		friend class Registry<Entity>;

	private:
		Iterator current;
		Iterator end;
		Filter filter;

	public:
		view_iterator() = default;
		view_iterator(const Registry<Entity>& registry, const Iterator& start, const Iterator& end, const Filter& filter) : current(start), end(end), filter(filter) {
			while (current != end && filter(current))
				++current;
		}

		view_iterator& operator++() {
			do {
				++current;
			} while (current != end && filter(current));

			return *this;
		}

		bool operator!=(IteratorEnd) const {
			return current != end;
		}

		decltype(*current)& operator*() const {
			return *current;
		}
	};

	template<typename BeginType, typename EndType = BeginType>
	class basic_view {
	private:
		BeginType start;
		EndType stop;

	public:
		basic_view() = default;
		basic_view(const BeginType& start, const EndType& stop) : start(start), stop(stop) {}

		BeginType begin() const {
			return start;
		}

		EndType end() const {
			return stop;
		}
	};

	template<typename... Components>
	typename std::enable_if_t<sizeof...(Components) == 0> collect(component_type& current_component, std::size_t& current_size, std::set<component_type>& components) {}

	template<typename Component, typename... Components>
	void collect(component_type& current_component, std::size_t& current_size, std::set<component_type>& components) {
		component_type index = component_index<Entity, Component>::index();

		if (index == current_component) {
			collect<Components...>(current_component, current_size, components);
			return;
		}

		std::size_t size = registry_type::components[index]->size();

		if (size < current_size) {
			components.insert(current_component);
			current_component = index;
			current_size = size;
		} else {
			components.insert(index);
		}

		collect<Components...>(current_component, current_size, components);
	}

	template<typename Filter>
	using filtered_view_iterator = view_iterator<entity_iterator, view_iterator_end, Filter>;

	template<typename Filter>
	using filtered_basic_view = basic_view<filtered_view_iterator<Filter>, view_iterator_end>;

	template<typename Filter>
	filtered_basic_view<Filter> filter_view(const entity_iterator& first, const entity_iterator& last, const Filter& filter) {
		filtered_view_iterator<Filter> start(*this, first, last, filter);
		view_iterator_end stop;
		return filtered_basic_view<Filter>(start, stop);
	}
	
	template<typename Component, typename... Components>
	auto view() {
		std::set<component_type> components;
		component_type current_component = component_index<Entity, Component>::index();
		std::size_t current_size = registry_type::components[current_component]->size();

		collect<Components...>(current_component, current_size, components);

		//components.reserve(sizeof...(Components));

		entity_map* map = registry_type::components[current_component];
		entity_iterator first = map->begin();
		entity_iterator last = map->end();

		std::function<bool(entity_iterator&)> filter = [] (entity_iterator& iterator) {
			return false;
		};

		return filter_view(first, last, filter);
	}

};

typedef Registry<std::uint8_t>  Registry8;
typedef Registry<std::uint16_t> Registry16;
typedef Registry<std::uint32_t> Registry32;
typedef Registry<std::uint64_t> Registry64;

};
