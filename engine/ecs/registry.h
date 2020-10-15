#pragma once

#include <set>
#include <queue>
#include <vector>
#include <cstdint>
#include <type_traits>
#include <unordered_map>
#include "../util/intrusivePointer.h"

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

	//-------------------------------------------------------------------------------------//
	// Types                                                                               //
	//-------------------------------------------------------------------------------------//

public:
	using registry_type = Registry<Entity>;
	using traits_type = registry_traits<Entity>;
	using entity_type = typename traits_type::entity_type;
	using component_type = typename traits_type::component_type;


	//-------------------------------------------------------------------------------------//
	// Member types                                                                        //
	//-------------------------------------------------------------------------------------//

private:
	struct entity_compare { bool operator()(const entity_type& left, const entity_type& right) const noexcept { return self(left) < self(right); } };

public:
	using entity_set = std::set<entity_type, entity_compare>;
	using entity_queue = std::queue<entity_type>;
	using entity_map = std::unordered_map<entity_type, Ref<RefCountable>>;
	using component_vector = std::vector<entity_map*>;

	using component_map_iterator = decltype(std::declval<entity_map>().begin());
	using entity_map_iterator = decltype(std::declval<entity_set>().begin());

	// Null entity
	entity_type null_entity = static_cast<entity_type>(0u);


	//-------------------------------------------------------------------------------------//
	// Helper structs                                                                      //
	//-------------------------------------------------------------------------------------//

private:
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

	template<typename>
	struct is_template_type : std::false_type {};

	template<template<typename...> typename Type, typename... Components>
	struct is_template_type<Type<Components...>> : std::true_type {};

	static constexpr entity_type self(const entity_type& entity) noexcept {
		return static_cast<entity_type>(entity & traits_type::entity_mask);
	}

	static constexpr entity_type parent(const entity_type& entity) noexcept {
		return static_cast<entity_type>(entity >> traits_type::parent_shift) & traits_type::entity_mask;
	}

	static constexpr entity_type merge(const entity_type& parent, const entity_type& entity) noexcept {
		return (static_cast<entity_type>(parent) << traits_type::parent_shift) | static_cast<entity_type>(entity);
	}

	
	//-------------------------------------------------------------------------------------//
	// Members                                                                             //
	//-------------------------------------------------------------------------------------//

private:
	entity_set entities;
	component_vector components;

	entity_queue id_queue;
	entity_type id_counter = null_entity;

	constexpr entity_type nextID() noexcept {
		return ++id_counter;
	}


	//-------------------------------------------------------------------------------------//
	// View iterators                                                                      //
	//-------------------------------------------------------------------------------------//

public:
	struct view_iterator_end {};

	struct component_iterator : component_map_iterator {
	public:
		component_iterator() = default;

		explicit component_iterator(component_map_iterator&& iterator) : component_map_iterator(std::move(iterator)) {}

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

		bool operator==(Iterator iterator) const {
			return current == iterator;
		}
	};


	//-------------------------------------------------------------------------------------//
	// View types                                                                          //
	//-------------------------------------------------------------------------------------//

private:
	// Separate wrapper type to ensure zero cost when overloading view(view_type<T>)
	template<typename T>
	struct view_type {};

public:
	template<typename... Components>
	struct conjunction {
		template<typename Component>
		static Ref<Component> get(Registry<Entity>* registry, const entity_type& entity) {
			component_type index = component_index<Entity, Component>::index();
			entity_map* map = registry->components[index];

			auto component_iterator = map->find(self(entity));

			return intrusive_cast<Component>(component_iterator->second);
		}
	};

	/*template<typename... Components>
	struct disjunction {
		template<typename Component>
		static Ref<Component> get(Registry<Entity>* registry, const entity_type& entity) {
			return registry->get<Component>(entity);
		}
	};*/

	struct no_type {
		template<typename Component>
		static Ref<Component> get(Registry<Entity>* registry, const entity_type& entity) {
			return registry->get<Component>(entity);
		}
	};


	//-------------------------------------------------------------------------------------//
	// Basic view                                                                          //
	//-------------------------------------------------------------------------------------//

public:
	template<typename ViewType, typename BeginType, typename EndType = BeginType>
	class basic_view {
	private:
		Registry<Entity>* registry;
		BeginType start;
		EndType stop;

	public:
		basic_view() = default;
		basic_view(Registry<Entity>* registry, const BeginType& start, const EndType& stop) : registry(registry), start(start), stop(stop) {}

		BeginType begin() const {
			return start;
		}

		EndType end() const {
			return stop;
		}

		template<typename Component>
		Ref<Component> get(const entity_type& entity) {
			return ViewType::template get<Component>(this->registry, entity);
		}
	};

	entity_map_iterator begin() noexcept {
		return entities.begin();
	}

	entity_map_iterator end() noexcept {
		return entities.end();
	}


	//-------------------------------------------------------------------------------------//
	// Views helper Functions                                                              //
	//-------------------------------------------------------------------------------------//

private:
	template<typename... Components>
	std::enable_if_t<sizeof...(Components) == 0> extract_smallest_component(component_type& smallest_component, std::size_t& smallest_size, std::vector<component_type>& other_components) {}

	template<typename Component, typename... Components>
	void extract_smallest_component(component_type& smallest_component, std::size_t& smallest_size, std::vector<component_type>& other_components) noexcept {
		component_type current_component = component_index<Entity, Component>::index();
		const std::size_t current_size = this->components[current_component]->size();

		if (current_size < smallest_size) {
			other_components.push_back(smallest_component);
			smallest_component = current_component;
			smallest_size = current_size;
		} else
			other_components.push_back(current_component);

		extract_smallest_component<Components...>(smallest_component, smallest_size, other_components);
	}

	template<typename... Components>
	std::enable_if_t<sizeof...(Components) == 0> insert_entities(entity_set& entities) {}

	template<typename Component, typename... Components>
	void insert_entities(entity_set& entities) noexcept {
		std::size_t component = component_index<Entity, Component>::index();
		component_iterator first(this->components[component]->begin());
		component_iterator last(this->components[component]->end());

		entities.insert(first, last);

		insert_entities<Components...>(entities);
	}

	template<typename Iterator, typename Filter>
	using filtered_view_iterator = view_iterator<Iterator, view_iterator_end, Filter>;

	template<typename ViewType, typename Iterator, typename Filter>
	using filtered_basic_view = basic_view<ViewType, filtered_view_iterator<Iterator, Filter>, view_iterator_end>;

	template<typename ViewType, typename Iterator, typename Filter>
	filtered_basic_view<ViewType, Iterator, Filter> filter_view(const Iterator& first, const Iterator& last, const Filter& filter) noexcept {
		filtered_view_iterator<Iterator, Filter> start(first, last, filter);
		view_iterator_end stop;
		return filtered_basic_view<ViewType, Iterator, Filter>(this, start, stop);
	}

	template<typename ViewType, typename Iterator>
	auto default_view(const Iterator& first, const Iterator& last) noexcept {
		return filter_view<ViewType>(first, last, [] (const Iterator&) { return false; });
	}


	//-------------------------------------------------------------------------------------//
	// Mutators                                                                            //
	//-------------------------------------------------------------------------------------//

public:
	/**
	* Creates a new entity with an empty parent and adds it to the registry
	*/
	[[nodiscard]] entity_type create() noexcept {
		entity_type id;
		if (id_queue.empty()) {
			id = merge(null_entity, nextID());
		} else {
			entity_type lastID = id_queue.front();

			id = merge(null_entity, lastID);

			id_queue.pop();
		}

		entities.insert(id);

		return id;
	}

	/**
	* Removes the given entity from the registry
	*/
	void destroy(const entity_type& entity) noexcept {
		if (entity == null_entity)
			return;

		auto entities_iterator = entities.find(entity);
		if (entities_iterator != entities.end()) {
			entities.erase(entities_iterator);
			id_queue.push(self(entity));

			for (auto map : components) {
				auto component_iterator = map->find(self(entity));
				if (component_iterator != map->end()) {
					delete component_iterator->second.get();
					map->erase(component_iterator);
				}
			}
		}
	}

	/**
	* Instantiates a component using the given type and arguments and adds it to the given entity
	*/
	template<typename Component, typename... Args>
	Ref<Component> add(const entity_type& entity, Args&&... args) noexcept {
		auto entities_iterator = entities.find(entity);
		if (entities_iterator == entities.end())
			return Ref<Component>();

		Ref<Component> component = make_intrusive(new Component(std::forward<Args>(args)...));
		component_type index = component_index<Entity, Component>::index();

		while (index >= components.size())
			components.push_back(new entity_map());

		entity_map* map = components[index];
		auto result = map->insert(std::make_pair(self(entity), intrusive_cast<RefCountable>(component)));

		if (!result.second)
			result.first->second = intrusive_cast<RefCountable>(component);

		return component;
	}

	/**
	* Removes the components of the given type from the given entity
	*/
	template<typename Component>
	void remove(const entity_type& entity) noexcept {
		auto entities_iterator = entities.find(entity);
		if (entities_iterator == entities.end())
			return;

		component_type index = component_index<Entity, Component>::index();
		if (index >= components.size())
			return;

		entity_map* map = components[index];
		auto component_iterator = map->find(self(entity));
		if (component_iterator == map->end())
			return;

		map->erase(component_iterator);
	}


	//-------------------------------------------------------------------------------------//
	// Getters                                                                               //
	//-------------------------------------------------------------------------------------//

	/**
	* Returns the component of the given type from the given entity, nullptr if no such component exists
	*/
	template<typename Component>
	[[nodiscard]] Ref<Component> get(const entity_type& entity) noexcept {
		auto entities_iterator = entities.find(entity);
		if (entities_iterator == entities.end())
			return Ref<Component>();

		component_type index = component_index<Entity, Component>::index();
		if (index >= components.size())
			return Ref<Component>();

		entity_map* map = components[index];
		auto component_iterator = map->find(self(entity));
		if (component_iterator == map->end())
			return Ref<Component>();

		return intrusive_cast<Component>(component_iterator->second);
	}

	/**
	* Returns the component of the given type from the given entity, or creates one using the provides arguments and returns it
	*/
	template<typename Component, typename... Args>
	[[nodiscard]] Ref<Component> getOrAdd(const entity_type& entity, Args&&... args) {
		auto entities_iterator = entities.find(entity);
		if (entities_iterator == entities.end())
			return Ref<Component>();

		component_type index = component_index<Entity, Component>::index();
		while (index >= components.size())
			components.push_back(new entity_map());

		entity_map* map = components[index];
		auto component_iterator = map->find(self(entity));
		if (component_iterator == map->end()) {
			Ref<Component> component = make_intrusive(new Component(std::forward<Args>(args)...));

			map->insert(std::make_pair(self(entity), intrusive_cast<RefCountable>(component)));

			return component;
		}

		return intrusive_cast<Component>(component_iterator->second);
	}

	/**
	* Returns the component of the given type from the given entity, the default value if no such component exists, note that the component will be copied
	*/
	template<typename Component>
	[[nodiscard]] Component getOr(const entity_type& entity, const Component& component) {
		Ref<Component> result = get<Component>(entity);
		if (result == nullptr)
			return component;
		else
			return *result;
	}

	/**
	* Returns whether the given entity has a component of the given type
	*/
	template<typename Component>
	[[nodiscard]] bool has(const entity_type& entity) noexcept {
		auto entities_iterator = entities.find(entity);
		if (entities_iterator == entities.end())
			return false;

		component_type index = component_index<Entity, Component>::index();
		if (index >= components.size())
			return false;

		entity_map* map = components[index];
		auto component_iterator = map->find(self(entity));
		if (component_iterator == map->end())
			return false;

		return true;
	}

	/**
	* Returns whether the registry contains the given entity
	*/
	[[nodiscard]] bool contains(const entity_type& entity) noexcept {
		return entities.find(entity) != entities.end();
	}


	//-------------------------------------------------------------------------------------//
	// Parent & self                                                                       //
	//-------------------------------------------------------------------------------------//    

	/**
	* Returns the parent of the given entity
	*/
	[[nodiscard]] constexpr entity_type getParent(const entity_type& entity) {
		return parent(entity);
	}

	/**
	* Returns the self id of the given entity
	*/
	[[nodiscard]] constexpr entity_type getSelf(const entity_type& entity) {
		return self(entity);
	}

	/**
	* Sets the parent of the given entity to the given parent, returns the updated entity if successful, returns the old entity if the parent does not exist, return the null entity if the entity does not exists
	*/
	[[nodiscard]] entity_type setParent(const entity_type& entity, const entity_type& parent) noexcept {
		auto entity_iterator = entities.find(entity);
		if (entity_iterator == entities.end())
			return null_entity;

		if (self(parent) != null_entity) {
			auto parent_iterator = entities.find(parent);
			if (parent_iterator == entities.end())
				return entity;
		}

		entity_type newEntity = merge(self(parent), self(entity));
		auto hint_iterator = entity_iterator;
		++hint_iterator;
		entities.erase(entity_iterator);
		entities.insert(hint_iterator, newEntity);

		return newEntity;
	}

	/**
	* Returns the children of the given parent entity
	*/
	[[nodiscard]] auto getChildren(const entity_type& entity) noexcept {
		entity_map_iterator first = entities.begin();
		entity_map_iterator last = entities.end();

		auto filter = [this, entity] (const entity_map_iterator& iterator) {
			if (parent(*iterator) != entity)
				return true;

			return false;
		};

		if (!contains(entity))
			return filter_view<no_type>(last, last, filter);

		return filter_view<no_type>(first, last, filter);
	}


	//-------------------------------------------------------------------------------------//
	// Views                                                                               //
	//-------------------------------------------------------------------------------------//

	/**
	* Returns an iterator which iterates over all entities having all the given components
	*/
private:
	template<typename Component, typename... Components>
	[[nodiscard]] auto view(view_type<conjunction<Component, Components...>>) noexcept {
		constexpr unique_components<Component, Components...>_;

		std::vector<component_type> other_components;
		other_components.reserve(sizeof...(Components));

		component_type smallest_component = component_index<Entity, Component>::index();
		std::size_t smallest_size = this->components[smallest_component]->size();

		extract_smallest_component<Components...>(smallest_component, smallest_size, other_components);

		entity_map* map = this->components[smallest_component];
		component_iterator first(map->begin());
		component_iterator last(map->end());

		auto filter = [this, other_components] (const component_iterator& iterator) {
			for (component_type component : other_components) {
				auto component_iterator = this->components[component]->find(iterator->first);
				if (component_iterator == this->components[component]->end())
					return true;
			}

			return false;
		};

		return filter_view<conjunction<Component, Components...>>(first, last, filter);
	}

	/**
	* Returns an iterator which iterates over all entities having any of the given components
	*/
	/*template<typename... Components>
	[[nodiscard]] auto view(view_type<disjunction<Components...>>) noexcept {
		constexpr unique_components<Components...>_;

		entity_set entities = *new entity_set();
		insert_entities<Components...>(entities);

		auto filter = [] (const decltype(entities.begin())&) {
			return false;
		};

		return filter_view<disjunction<Components...>>>(entities.begin(), entities.end(), filter);
	}*/

public:
	/**
	* Returns an iterator which iterates over all entities which satisfy the view type
	*/
	template<typename... Type>
	[[nodiscard]] auto view() noexcept {
		if constexpr (sizeof...(Type) == 1)
			if constexpr (is_template_type<Type...>::value)
				return view(view_type<Type...>{});
			else
				return view(view_type<conjunction<Type...>>{});
		else
			return view(view_type<conjunction<Type...>>{});
	}
};

typedef Registry<std::uint8_t>  Registry8;
typedef Registry<std::uint16_t> Registry16;
typedef Registry<std::uint32_t> Registry32;
typedef Registry<std::uint64_t> Registry64;
};