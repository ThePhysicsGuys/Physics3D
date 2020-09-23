#pragma once

#include <cstdint>
#include <map>
#include <set>
#include <queue>
#include <vector>
#include <functional>
#include <type_traits>

namespace P3D::Engine {

template<typename, typename = void>
struct registry_traits;

template<typename Type>
struct registry_traits<Type, std::enable_if_t<std::is_enum_v<Type>>> : registry_traits<std::underlying_type_t<Type>> {};


/**
 * entity = 4 parent bits, 4 self bits
 */
template<>
struct registry_traits<std::uint8_t> {
	using entity_type = std::uint8_t;
	using component_type = std::uint16_t;

	static constexpr entity_type entity_mask = 0xF;
	static constexpr std::size_t parent_shift = 4u;
};

/**
 * entity = 8 parent bits, 8 self bits
 */
template<>
struct registry_traits<std::uint16_t> {
	using entity_type = std::uint8_t;
	using component_type = std::uint16_t;

	static constexpr entity_type entity_mask = 0xFF;
	static constexpr std::size_t parent_shift = 8u;
};

/**
 * entity = 16 parent bits, 1§ self bits
 */
template<>
struct registry_traits<std::uint32_t> {
	using entity_type = std::uint16_t;
	using component_type = std::uint16_t;

	static constexpr entity_type entity_mask = 0xFFFF;
	static constexpr std::size_t parent_shift = 16u;
};

/**
 * entity = 32 parent bits, 32 self bits
 */
template<>
struct registry_traits<std::uint64_t> {
	using entity_type = std::uint32_t;
	using component_type = std::uint16_t;

	static constexpr entity_type entity_mask = 0xFFFFFFFF;
	static constexpr std::size_t parent_shift = 32u;
};

template<typename Entity>
inline constexpr auto self(const Entity& entity) noexcept -> typename registry_traits<Entity>::entity_type {
	using traits_type = registry_traits<Entity>;
	using entity_type = typename traits_type::entity_type;
	return static_cast<entity_type>(entity & traits_type::entity_mask);
}

template<typename Entity>
inline constexpr auto parent(const Entity& entity) noexcept -> typename registry_traits<Entity>::entity_type {
	using traits_type = registry_traits<Entity>;
	using entity_type = typename traits_type::entity_type;
	return static_cast<entity_type>(entity >> traits_type::parent_shift) & traits_type::entity_mask;
}

template<typename Entity>
inline constexpr auto merge(const typename registry_traits<Entity>::entity_type& parent, const typename registry_traits<Entity>::entity_type& entity) noexcept -> Entity {
	using traits_type = registry_traits<Entity>;
	return (static_cast<Entity>(parent) << traits_type::parent_shift) | static_cast<Entity>(entity);
}

template<typename Type>
struct type_index {
	static Type next() noexcept {
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

	static component_type count() noexcept {
		static const component_type value = type_index<component_type>::next();
		return value;
	}
};

template<typename Entity>
class Registry {
private:
	struct entity_compare {
		bool operator()(const Entity& left, const Entity& right) const noexcept {
			return self(left) < self(right);
		}
	};

	template<typename... Components>
	struct unique_components;

	template<typename Component>
	struct unique_components<Component> {};

	template<typename C1, typename C2, typename... Components>
	struct unique_components<C1, C2, Components...> :
		unique_components<C1, C2>,
		unique_components<C1, Components...>,
		unique_components<C2, Components...> {
	};

	template<typename C1, typename C2>
	struct unique_components<C1, C2> {
		static_assert(!std::is_same_v<C1, C2>, "Types must be unique");
	};

public:
	using registry_type = Registry<Entity>;
	using traits_type = registry_traits<Entity>;
	using entity_type = typename traits_type::entity_type;
	using component_type = typename traits_type::component_type;

	using entity_set = std::set<Entity, entity_compare>;
	using entity_queue = std::queue<entity_type>;
	using entity_map = std::map<entity_type, void*>;
	using component_vector = std::vector<entity_map*>;

	using component_map_iterator = decltype(std::declval<entity_map>().begin());
	using entity_map_iterator = decltype(std::declval<entity_set>().begin());

	// Null
	entity_type null_entity = static_cast<entity_type>(0u);

	//? Functions
private:
	constexpr entity_type nextID() noexcept {
		return ++id_counter;
	}

	// Components
	entity_set entities;
	component_vector components;

	// ID
	entity_queue id_queue;
	entity_type id_counter = null_entity;


	//? Iterators
public:
	struct view_iterator_end {};

	struct component_iterator : component_map_iterator {
	public:
		component_iterator() = default;

		template<typename MapIterator>
		component_iterator(MapIterator&& iterator) : component_map_iterator(std::forward<MapIterator>(iterator)) {}

		auto& operator*() const {
			return component_map_iterator::operator*().first;
		}
	};

	template<typename Iterator, typename IteratorEnd, typename Filter>
	class view_iterator {
		friend class Registry<Entity>;

	private:
		Iterator current;
		Iterator end;
		Filter filter;

	public:
		view_iterator() = default;
		view_iterator(const Iterator& start, const Iterator& end, const Filter& filter) : current(start), end(end), filter(filter) {
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
		Registry<Entity>* registry;
		BeginType start;
		EndType stop;

	public:
		basic_view() = default;
		basic_view(Registry<Entity>* registry, const BeginType& start, const EndType& stop) : start(start), stop(stop), registry(registry) {}

		BeginType begin() const {
			return start;
		}

		EndType end() const {
			return stop;
		}

		/**
		* Returns the component of the given type from the given entity from the view, passing an entity or component which is not within the view results in undefined behaviour
		*/
		template<typename Component>
		Component& get(const Entity& entity) {
			component_type index = component_index<Entity, Component>::index();
			entity_map* map = registry->components[index];

			auto component_iterator = map->find(self(entity));

			return *static_cast<Component*>(component_iterator->second);
		}
	};

	//? Functions
private:
	template<typename... Components>
	typename std::enable_if_t<sizeof...(Components) == 0> collect(component_type& current_component, std::size_t& current_size, std::vector<component_type>& components) {}

	template<typename Component, typename... Components>
	void collect(component_type& current_component, std::size_t& current_size, std::vector<component_type>& components) noexcept {
		component_type index = component_index<Entity, Component>::index();

		std::size_t size = registry_type::components[index]->size();

		if (size < current_size) {
			components.push_back(current_component);
			current_component = index;
			current_size = size;
		} else {
			components.push_back(index);
		}

		collect<Components...>(current_component, current_size, components);
	}

	template<typename Filter, typename Iterator>
	using filtered_view_iterator = view_iterator<Iterator, view_iterator_end, Filter>;

	template<typename Filter, typename Iterator>
	using filtered_basic_view = basic_view<filtered_view_iterator<Filter, Iterator>, view_iterator_end>;

	template<typename Filter, typename Iterator>
	filtered_basic_view<Filter, Iterator> filter_view(const Iterator& first, const Iterator& last, const Filter& filter) noexcept {
		filtered_view_iterator<Filter, Iterator> start(first, last, filter);
		view_iterator_end stop;
		return filtered_basic_view<Filter, Iterator>(this, start, stop);
	}

	//? Functions
public:
	/**
	* Creates a new entity with an empty parent and adds it to the registry
	*/
	[[nodiscard]] Entity create() noexcept {
		Entity id;
		if (id_queue.empty()) {
			id = merge<Entity>(null_entity, nextID());
		} else {
			entity_type lastID = id_queue.front();

			id = merge<Entity>(null_entity, lastID);

			id_queue.pop();
		}

		entities.insert(id);

		return id;
	}

	/**
	* Removes the given entity from the registry
	*/
	void destroy(const Entity& entity) noexcept {
		auto entities_iterator = entities.find(entity);
		if (entities_iterator != entities.end()) {
			entities.erase(entities_iterator);
			id_queue.push(self(entity));

			for (auto map : components) {
				auto component_iterator = map->find(self(entity));
				if (component_iterator != map->end()) {
					delete component_iterator->second;
					map->erase(component_iterator);
				}
			}
		}
	}

	/**
	* Instantiates a component using the given type and arguments and adds it to the given entity
	*/
	template<typename Component, typename... Args>
	void add(const Entity& entity, Args&&... args) noexcept {
		auto entities_iterator = entities.find(entity);
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

	/**
	* Removes the components of the given type from the given entity
	*/
	template<typename Component>
	void remove(const Entity& entity) noexcept {
		auto entities_iterator = entities.find(entity);
		if (entities_iterator == entities.end())
			return;

		component_type index = component_index<Entity, Component>::index();
		if (index >= components.size())
			return;

		entity_map* map = components[index];
		if (map == nullptr)
			return;

		auto component_iterator = map->find(self(entity));
		if (component_iterator == map->end())
			return;

		map->erase(component_iterator);
	}

	/**
	* Returns the component of the given type from the given entity, nullptr if no such component exists
	*/
	template<typename Component>
	[[nodiscard]] Component* get(const Entity& entity) noexcept {
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

	/**
	* Returns whether the given entity has a component of the given type
	*/
	template<typename Component>
	[[nodiscard]] bool has(const Entity& entity) noexcept {
		auto entities_iterator = entities.find(entity);
		if (entities_iterator == entities.end())
			return false;

		component_type index = component_index<Entity, Component>::index();
		if (index >= components.size())
			return false;

		entity_map* map = components[index];
		if (map == nullptr)
			return false;

		auto component_iterator = map->find(self(entity));
		if (component_iterator == map->end())
			return false;

		return true;
	}

	/**
	* Returns whether the registry contains the given entity
	*/
	[[nodiscard]] bool contains(const Entity& entity) noexcept {
		return entities.find(entity) != entities.end();
	}

	/**
	* Returns the parent of the given entity
	*/
	[[nodiscard]] constexpr entity_type getParent(const Entity& entity) {
		return parent(entity);
	}

	/**
	* Returns the self id of the given entity
	*/
	[[nodiscard]] constexpr entity_type getSelf(const Entity& entity) {
		return self(entity);
	}

	/**
	* Sets the parent of the given entity to the given parent, returns the updated entity if successful, returns the old entity if the parent does not exist, return the null entity if the entity does not exists
	*/
	[[nodiscard]] Entity setParent(const Entity& entity, const Entity& parent) noexcept {
		auto entity_iterator = entities.find(entity);
		if (entity_iterator == entities.end())
			return null_entity;

		if (self(parent) != null_entity) {
			auto parent_iterator = entities.find(parent);
			if (parent_iterator == entities.end())
				return entity;
		}

		Entity newEntity = merge<Entity>(self(parent), self(entity));
		auto hint_iterator = entity_iterator;
		hint_iterator++;
		entities.erase(entity_iterator);
		entities.insert(hint_iterator, newEntity);

		return newEntity;
	}

	/**
	* Returns the children of the given parent entity
	*/
	[[nodiscard]] auto getChildren(const Entity& entity) noexcept {
		entity_map_iterator first = entities.begin();
		entity_map_iterator last = entities.end();

		auto filter = [this, entity] (const entity_map_iterator& iterator) {
			if (parent(*iterator) != entity)
				return true;

			return false;
		};

		if (!contains(entity))
			return filter_view(last, last, filter);

		return filter_view(first, last, filter);
	}

	/**
	* Returns an iterator which iterators over all entities having all the given components
	*/
	template<typename Component, typename... Components>
	[[nodiscard]] auto view() noexcept {
		constexpr unique_components<Component, Components...>_;

		std::vector<component_type> components;
		components.reserve(sizeof...(Components));

		component_type current_component = component_index<Entity, Component>::index();
		std::size_t current_size = registry_type::components[current_component]->size();

		collect<Components...>(current_component, current_size, components);

		entity_map* map = registry_type::components[current_component];
		component_iterator first(map->begin());
		component_iterator last(map->end());

		auto filter = [this, components] (const component_map_iterator& iterator) {
			for (component_type component : components) {
				auto component_iterator = this->components[component]->find(iterator->first);
				if (component_iterator == this->components[component]->end())
					return true;
			}

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