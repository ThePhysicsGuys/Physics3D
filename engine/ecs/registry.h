#pragma once

#include <set>
#include <queue>
#include <vector>
#include <cstdint>
#include <type_traits>
#include <unordered_map>
#include <typeinfo>
#include "../util/typetraits.h"
#include "../util/iteratorUtils.h"
#include "../util/stringUtil.h"
#include "../Physics3D/datastructures/smartPointers.h"

namespace P3D::Engine {

template <typename, typename = void>
struct registry_traits;

template <typename Type>
struct registry_traits<Type, std::enable_if_t<std::is_enum_v<Type>>> : registry_traits<std::underlying_type_t<Type>> {};


/**
 * entity = 8 parent bits, 8 self bits
 */
template <>
struct registry_traits<std::uint16_t> {
	using representation_type = std::uint16_t;
	using entity_type = std::uint8_t;
	using component_type = std::uint16_t;

	static constexpr entity_type entity_mask = 0xFF;
	static constexpr std::size_t parent_shift = 8u;
};

/**
 * entity = 16 parent bits, 16 self bits
 */
template <>
struct registry_traits<std::uint32_t> {
	using representation_type = std::uint32_t;
	using entity_type = std::uint16_t;
	using component_type = std::uint16_t;

	static constexpr entity_type entity_mask = 0xFFFF;
	static constexpr std::size_t parent_shift = 16u;
};

/**
 * entity = 32 parent bits, 32 self bits
 */
template <>
struct registry_traits<std::uint64_t> {
	using representation_type = std::uint64_t;
	using entity_type = std::uint32_t;
	using component_type = std::uint16_t;

	static constexpr entity_type entity_mask = 0xFFFFFFFF;
	static constexpr std::size_t parent_shift = 32u;
};
	
template <typename Entity>
class Registry {

	//-------------------------------------------------------------------------------------//
	// Contructor                                                                          //
	//-------------------------------------------------------------------------------------//
	
public:
	Registry() = default;
	~Registry() = default;
	Registry(Registry&&) = delete;
	Registry(const Registry&) = delete;
	Registry& operator=(const Registry&) = delete;
	Registry& operator=(Registry&&) = delete;
	
	//-------------------------------------------------------------------------------------//
	// Types                                                                               //
	//-------------------------------------------------------------------------------------//

public:
	using registry_type = Registry<Entity>;
	using traits_type = registry_traits<Entity>;
	using entity_type = typename traits_type::entity_type;
	using component_type = typename traits_type::component_type;
	using representation_type = typename traits_type::representation_type;


	//-------------------------------------------------------------------------------------//
	// Member types                                                                        //
	//-------------------------------------------------------------------------------------//

private:
	struct entity_compare { bool operator() (const representation_type& left, const representation_type& right) const noexcept { return self(left) < self(right); } };

public:
	using entity_set = std::set<representation_type, entity_compare>;
	using entity_queue = std::queue<entity_type>;
	using entity_map = std::unordered_map<entity_type, IRef<RC>>;
	using type_map = std::unordered_map<component_type, std::string>;
	using component_vector = std::vector<entity_map*>;

	using component_vector_iterator = decltype(std::declval<component_vector>().begin());
	using component_map_iterator = decltype(std::declval<entity_map>().begin());
	using entity_set_iterator = decltype(std::declval<entity_set>().begin());

	// Null entity
	inline static entity_type null_entity = static_cast<entity_type>(0u);


	//-------------------------------------------------------------------------------------//
	// Members                                                                             //
	//-------------------------------------------------------------------------------------//

private:
	entity_set entities;
	component_vector components;
	type_map type_mapping;

	entity_queue id_queue;
	entity_type id_counter = null_entity;

	constexpr entity_type nextID() noexcept {
		return ++id_counter;
	}


	//-------------------------------------------------------------------------------------//
	// Helper structs                                                                      //
	//-------------------------------------------------------------------------------------//

private:
	template <typename Type>
	struct type_index {
		static Type next() noexcept {
			static Type value {};
			return value++;
		}
	};

public:
	template <typename Component = void>
	struct component_index {
		using traits_type = registry_traits<Entity>;
		using component_type = typename traits_type::component_type;

		static component_type index() {
			static const component_type value = type_index<component_type>::next();
			return value;
		}
	};

private:
	static constexpr entity_type self(const representation_type& entity) noexcept {
		return static_cast<entity_type>(entity & traits_type::entity_mask);
	}

	static constexpr entity_type parent(const representation_type& entity) noexcept {
		return static_cast<entity_type>(entity >> traits_type::parent_shift) & traits_type::entity_mask;
	}

	static constexpr representation_type merge(const entity_type& parent, const entity_type& entity) noexcept {
		return (static_cast<representation_type>(parent) << traits_type::parent_shift) | static_cast<representation_type>(entity);
	}


	//-------------------------------------------------------------------------------------//
	// View iterators                                                                      //
	//-------------------------------------------------------------------------------------//

public:
	struct component_iterator : component_map_iterator {
	public:
		component_iterator() = default;

		explicit component_iterator(component_map_iterator&& iterator) : component_map_iterator(std::move(iterator)) {}

		auto& operator*() const {
			return component_map_iterator::operator*().first;
		}
	};

	
	//-------------------------------------------------------------------------------------//
	// View types                                                                          //
	//-------------------------------------------------------------------------------------//

private:
	template <typename Type>
	struct type {};

public:
	template <typename... Components>
	struct conj {
		template <typename Component>
		static IRef<Component> get(Registry<Entity>* registry, const entity_type& entity) {
			component_type index = registry->getComponentIndex<Component>();
			entity_map* map = registry->components[index];

			auto component_iterator = map->find(entity);

			return intrusive_cast<Component>(component_iterator->second);
		}
	};

	template <typename... Components>
	struct disj {
		template <typename Component>
		static IRef<Component> get(Registry<Entity>* registry, const entity_type& entity) {
			return registry->get<Component>(entity);
		}
	};

	template <typename ComponentA, typename ComponentB>
	struct exor {};

	template <typename T>
	struct neg;

	struct no_type {
		template <typename Component>
		static IRef<Component> get(Registry<Entity>* registry, const entity_type& entity) {
			return registry->get<Component>(entity);
		}
	};


	//-------------------------------------------------------------------------------------//
	// Basic view                                                                          //
	//-------------------------------------------------------------------------------------//

public:
	template <typename ViewType, typename BeginType, typename EndType = iterator_end>
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

		template <typename Component>
		IRef<Component> get(const entity_type& entity) {
			return ViewType::template get<Component>(this->registry, entity);
		}
	};

	entity_set_iterator begin() noexcept {
		return entities.begin();
	}

	entity_set_iterator end() noexcept {
		return entities.end();
	}


	//-------------------------------------------------------------------------------------//
	// Views helper Functions                                                              //
	//-------------------------------------------------------------------------------------//

private:
	template <typename... Components>
	std::enable_if_t<sizeof...(Components) == 0> init() {}
	
	template <typename... Components>
	std::enable_if_t<sizeof...(Components) == 0> extract_smallest_component(component_type& smallest_component, std::size_t& smallest_size, std::vector<component_type>& other_components) {}

	template <typename Component, typename... Components>
	void extract_smallest_component(component_type& smallest_component, std::size_t& smallest_size, std::vector<component_type>& other_components) noexcept {
		component_type current_component = getComponentIndex<Component>();
		const std::size_t current_size = this->components[current_component]->size();

		if (current_size < smallest_size) {
			other_components.push_back(smallest_component);
			smallest_component = current_component;
			smallest_size = current_size;
		} else
			other_components.push_back(current_component);

		extract_smallest_component<Components...>(smallest_component, smallest_size, other_components);
	}

	template <typename... Components>
	std::enable_if_t<sizeof...(Components) == 0> insert_entities(entity_set& entities) {}

	template <typename Component, typename... Components>
	void insert_entities(entity_set& entities) noexcept {
		std::size_t component = getComponentIndex<Component>();

		entity_map* map = this->components[component];
		if (map != nullptr) {
			component_iterator first(map->begin());
			component_iterator last(map->end());

			entities.insert(first, last);
		}

		insert_entities<Components...>(entities);
	}
	
	template <typename ViewType, typename Iterator, typename Filter>
	auto filter_view(const Iterator& first, const Iterator& last, const Filter& filter) noexcept {
		using iterator_type = filter_iterator<Iterator, iterator_end, Filter>;
		iterator_type start(first, last, filter);
		iterator_end stop;
		return basic_view<ViewType, iterator_type>(this, start, stop);
	}

	template <typename ViewType, typename Iterator, typename Transform>
	auto transform_view(const Iterator& first, const Iterator& last, const Transform& transform) noexcept {
		using iterator_type = transform_iterator<Iterator, iterator_end, Transform>;
		iterator_type start(first, last, transform);
		iterator_end stop;
		return basic_view<ViewType, iterator_type>(this, start, stop);
	}

	template <typename ViewType, typename Iterator, typename Filter, typename Transform>
	auto filter_transform_view(const Iterator& first, const Iterator& last, const Filter& filter, const Transform& transform) noexcept {
		using iterator_type = filter_transform_iterator<Iterator, iterator_end, Filter, Transform>;
		iterator_type start(first, last, filter, transform);
		iterator_end stop;
		return basic_view<ViewType, iterator_type>(this, start, stop);
	}
	
	template <typename ViewType, typename Iterator>
	auto default_view(const Iterator& first, const Iterator& last) noexcept {
		using iterator_type = default_iterator<Iterator, iterator_end>;
		iterator_type start(first, last);
		iterator_end stop;
		return basic_view<ViewType, iterator_type>(this, start, stop);
	}

	//-------------------------------------------------------------------------------------//
	// Filter matching                                                                     //
	//-------------------------------------------------------------------------------------//

	template <typename Component>
	struct match {
		constexpr std::enable_if_t<std::is_base_of_v<RC, Component>, bool> operator()(const entity_type& entity) {
			return has<Component>(entity);
		}
	};

	template <typename Component>
	struct match<neg<Component>> {
		constexpr bool operator()(const entity_type& entity) {
			return !match<Component>()(entity);
		}
	};

	template <typename Component>
	struct match<conj<Component>> {
		constexpr bool operator()(const entity_type& entity) {
			return match<Component>()(entity);
		}
	};

	template <typename Component>
	struct match<disj<Component>> {
		constexpr bool operator()(const entity_type& entity) {
			return match<Component>()(entity);
		}
	};

	template <typename Component, typename... Components>
	struct match<conj<Component, Components...>> {
		constexpr bool operator()(const entity_type& entity) {
			return match<Component>()(entity) && match<conj<Components...>>()(entity);
		}
	};

	template <typename Component, typename... Components>
	struct match<disj<Component, Components...>> {
		constexpr bool operator()(const entity_type& entity) {
			return match<Component>()(entity) || match<disj<Components...>>()(entity);
		}
	};

	template <typename ComponentA, typename ComponentB>
	struct match<exor<ComponentA, ComponentB>> {
		constexpr bool operator()(const entity_type& entity) {
			return match<ComponentA>()(entity) != match<ComponentB>()(entity);
		}
	};

	//-------------------------------------------------------------------------------------//
	// Mutators                                                                            //
	//-------------------------------------------------------------------------------------//

public:

	/**
	* Returns the index of the given component
	*/
	template <typename Component>
	[[nodiscard]] component_type getComponentIndex() {
		component_type index = component_index<Component>::index();
		if (index >= this->type_mapping.size()) {
			std::string fullName = Util::typeName<Component>();
			std::string camelCase = Util::demangle(fullName);
			std::string name = Util::decamel(camelCase);
			this->type_mapping.insert(std::make_pair(index, name));
		}

		while (index >= this->components.size())
			this->components.push_back(new entity_map());

		return index;
	}

	/**
	* Returns a string view of name of the component corresponding to the given component id
	*/
	[[nodiscard]] std::string_view getComponentName(const component_type& component) {
		return type_mapping.at(component);
	}

	/**
	 * Returns a string view of the name of the given component
	 */
	template <typename Component>
	[[nodiscard]] std::string_view getComponentName() {
		return this->type_mapping.at(getComponentIndex<Component>());
	}
	
	/**
	 * Initializes the component vector to create an order in the components
	 */
	template <typename Component, typename... Components>
	void init() {
		component_type index = getComponentIndex<Component>();

		init<Components...>();
	}
	
	/**
	 * Creates a new entity with an empty parent and adds it to the registry
	 */
	[[nodiscard]] entity_type create(const entity_type& parent = null_entity) noexcept {
		representation_type id;
		if (this->id_queue.empty()) {
			id = merge(parent, nextID());
		} else {
			entity_type lastID = this->id_queue.front();

			id = merge(parent, lastID);

			this->id_queue.pop();
		}

		this->entities.insert(id);

		return static_cast<entity_type>(id);
	}

	/**
	 * Removes the given entity from the registry, returns the null entity if the entity is successfully removed
	 */
	entity_type destroy(const entity_type& entity) noexcept {
		if (entity == null_entity)
			return null_entity;

		auto entities_iterator = this->entities.find(static_cast<representation_type>(entity));
		if (entities_iterator != this->entities.end()) {
			this->entities.erase(entities_iterator);
			id_queue.push(entity);

			for (entity_map* map : this->components) {
				auto component_iterator = map->find(entity);
				if (component_iterator != map->end()) {
					// the component's intrusive_ptr will clean up the component
					map->erase(component_iterator);
				}
			}

			return null_entity;
		}

		return entity;
	}

	/**
	 * Instantiates a component using the given type and arguments and adds it to the given entity
	 */
	template <typename Component, typename... Args>
	IRef<Component> add(const entity_type& entity, Args&&... args) noexcept {
		auto entities_iterator = this->entities.find(static_cast<representation_type>(entity));
		if (entities_iterator == this->entities.end())
			return IRef<Component>();

		IRef<Component> component = make_intrusive<Component>(std::forward<Args>(args)...);
		component_type index = getComponentIndex<Component>();

		entity_map* map = this->components[index];
		auto result = map->insert(std::make_pair(entity, intrusive_cast<RC>(component)));

		if (!result.second)
			result.first->second = intrusive_cast<RC>(component);

		return component;
	}

	/**
	 * Removes the component with the given component id from the given entity, returns whether the erasure was successful
	 */
	bool remove(const entity_type& entity, const component_type& index) noexcept {
		if (index >= this->components.size())
			return false;
		
		auto entities_iterator = this->entities.find(static_cast<representation_type>(entity));
		if (entities_iterator == this->entities.end())
			return false;

		entity_map* map = this->components[index];
		auto component_iterator = map->find(entity);
		if (component_iterator == map->end())
			return false;

		map->erase(component_iterator);
		// Todo check if really destroyed
		
		return true;
	}


	/**
	 * Removes the component of the given type from the given entity, returns whether the erasure was successful
	 */
	template <typename Component>
	bool remove(const entity_type& entity) noexcept {
		component_type index = getComponentIndex<Component>();

		return remove(entity, index);
	}
	
	//-------------------------------------------------------------------------------------//
	// Getters                                                                               //
	//-------------------------------------------------------------------------------------//

	/**
	 * Returns the component of the given type from the given entity, nullptr if no such component exists
	 */
	template <typename Component>
	[[nodiscard]] IRef<Component> get(const entity_type& entity) noexcept {
		if (entity == null_entity)
			return IRef<Component>();

		auto entities_iterator = this->entities.find(static_cast<representation_type>(entity));
		if (entities_iterator == this->entities.end())
			return IRef<Component>();

		component_type index = getComponentIndex<Component>();
		entity_map* map = this->components[index];
		auto component_iterator = map->find(entity);
		if (component_iterator == map->end())
			return IRef<Component>();

		return intrusive_cast<Component>(component_iterator->second);
	}

	/**
	 * Returns the component of the given type from the given entity, or creates one using the provides arguments and returns it.
	 */
	template <typename Component, typename... Args>
	[[nodiscard]] IRef<Component> getOrAdd(const entity_type& entity, Args&&... args) {
		auto entities_iterator = this->entities.find(static_cast<representation_type>(entity));
		if (entities_iterator == this->entities.end())
			return IRef<Component>();

		component_type index = getComponentIndex<Component>();;
		entity_map* map = this->components[index];
		auto component_iterator = map->find(entity);
		if (component_iterator == map->end()) {
			IRef<Component> component = make_intrusive<Component>(std::forward<Args>(args)...);

			map->insert(std::make_pair(entity, intrusive_cast<RC>(component)));

			return component;
		}

		return intrusive_cast<Component>(component_iterator->second);
	}

	/**
	 * Returns the component of the given type from the given entity, the default value if no such component exists, note that the component will be copied
	 */
	template <typename Component, typename... Args>
	[[nodiscard]] Component getOr(const entity_type& entity, Args&&... args) {
		IRef<Component> result = get<Component>(entity);
		if (result == nullptr)
			return Component(std::forward<Args>(args)...);
		else
			return *result;
	}

	/**
	 * Returns whether the given entity has a component of the given type
	 */
	template <typename Component>
	[[nodiscard]] bool has(const entity_type& entity) noexcept {
		auto entities_iterator = this->entities.find(static_cast<representation_type>(entity));
		if (entities_iterator == this->entities.end())
			return false;

		component_type index = getComponentIndex<Component>();
		entity_map* map = this->components[index];
		auto component_iterator = map->find(entity);
		if (component_iterator == map->end())
			return false;

		return true;
	}

	/**
	 * Returns whether the registry contains the given entity
	 */
	[[nodiscard]] bool contains(const entity_type& entity) noexcept {
		return this->entities.find(static_cast<representation_type>(entity)) != entities.end();
	}


	//-------------------------------------------------------------------------------------//
	// Parent & self                                                                       //
	//-------------------------------------------------------------------------------------//    

	/**
	 * Returns the parent of the given entity
	 */
	[[nodiscard]] constexpr entity_type getParent(const representation_type& entity) {
		return parent(entity);
	}

	/**
	 * Returns the parent of the given entity
	 */
	[[nodiscard]] constexpr entity_type getParent(const entity_type& entity) {
		entity_set_iterator iterator = this->entities.find(static_cast<representation_type>(entity));
		if (iterator != this->entities.end())
			return parent(*iterator);

		return null_entity;
	}
	
	/**
	 * Returns the self id of the given entity
	 */
	[[nodiscard]] constexpr entity_type getSelf(const representation_type& entity) {
		return self(entity);
	}

	/**
	 * Returns the self id of the given entity
	 */
	[[nodiscard]] constexpr entity_type getSelf(const entity_type& entity) {
		return entity;
	}

	/**
	 * Sets the parent of the given entity to the given parent, returns true if successful, returns false if the entity does not exist.
	 */
	bool setParent(const entity_type& entity, const entity_type& parent) noexcept {
		auto entity_iterator = this->entities.find(static_cast<representation_type>(entity));
		if (entity_iterator == this->entities.end())
			return false;

		if (parent != null_entity) {
			auto parent_iterator = this->entities.find(parent);
			if (parent_iterator == this->entities.end())
				return false;
		}

		representation_type newEntity = merge(parent, entity);
		auto hint_iterator = entity_iterator;
		++hint_iterator;
		this->entities.erase(entity_iterator);
		this->entities.insert(hint_iterator, newEntity);

		return true;
	}

	/**
	 * Returns the children of the given parent entity
	 */
	[[nodiscard]] auto getChildren(const entity_type& entity) noexcept {
		entity_set_iterator first = this->entities.begin();
		entity_set_iterator last = this->entities.end();

		auto filter = [this, entity] (const entity_set_iterator& iterator) {
			return parent(*iterator) == entity;
		};

		auto transform = [] (const entity_set_iterator& iterator) {
			return self(*iterator);
		};

		if (!contains(entity))
			return filter_transform_view<no_type>(last, last, filter, transform);

		return filter_transform_view<no_type>(first, last, filter, transform);
	}

	/**
	 * Returns the children of the given parent entity
	 */
	[[nodiscard]] auto getChildren(const representation_type& entity) noexcept {
		return getChildren(self(entity));
	}


	//-------------------------------------------------------------------------------------//
	// Views                                                                               //
	//-------------------------------------------------------------------------------------//
	
	/**
	 * Returns an iterator which iterates over all entities having all the given components
	 */
private:
	template <typename Component, typename... Components>
	[[nodiscard]] auto view(type<conj<Component, Components...>>) noexcept {
		static_assert(unique_types<Component, Components...>);

		std::vector<component_type> other_components;
		other_components.reserve(sizeof...(Components));

		component_type smallest_component = getComponentIndex<Component>();
		std::size_t smallest_size = this->components[smallest_component]->size();

		extract_smallest_component<Components...>(smallest_component, smallest_size, other_components);

		auto filter = [this, other_components] (const component_map_iterator& iterator) {
			for (component_type component : other_components) {
				auto component_iterator = this->components[component]->find(iterator->first);
				if (component_iterator == this->components[component]->end())
					return false;
			}

			return true;
		};

		auto transform = [] (const component_map_iterator& iterator) {
			return iterator->first;
		};

		entity_map* map = this->components[smallest_component];
		component_map_iterator first(map->begin());
		component_map_iterator last(map->end());
		return filter_transform_view<conj<Component, Components...>>(first, last, filter, transform);
	}

	/**
	 * Returns an iterator which iterates over all entities having any of the given components
	 */
	template <typename... Components>
	[[nodiscard]] auto view(type<disj<Components...>>) noexcept {
		static_assert(unique_types<Components...>);
		entity_set entities;
		insert_entities<Components...>(entities);

		auto filter = [entities] (const entity_set_iterator& iterator) {
			return entities.find(*iterator) != entities.end();
		};

		return filter_view<disj<Components...>>(this->entities.begin(), this->entities.end(), filter);
	}

public:
	/**
	 * Returns a view which iterates over the components of the given entity 
	 */
	[[nodiscard]] auto getComponents(const entity_type& entity) noexcept {
		component_vector_iterator first = components.begin();
		component_vector_iterator last = components.end();

		auto filter = [entity] (const component_vector_iterator& iterator) {
			entity_map* map = *iterator;

			return map->find(entity) != map->end();
		};

		auto transform = [first, entity] (const component_vector_iterator& iterator) {
			entity_map* map = *iterator;
			auto result = std::make_pair(std::distance(first, iterator), map->at(entity));
			return result;
		};

		return filter_transform_view<no_type>(first, last, filter, transform);
	}

	/**
	 * Returns a view that iterates over all entities which satisfy the given filter 
	 */
	template <typename Filter>
	[[nodiscard]] auto filter(const Filter& filter) {
		entity_set_iterator first = this->entities.begin();
		entity_set_iterator last = this->entities.end();

		return filter_view<no_type, entity_set_iterator>(first, last, filter);
	}

	/**
	 * Returns a view that iterates over all entities which satisfy the given filter
	 */
	template <typename Filter>
	[[nodiscard]] auto filter() {
		entity_set_iterator first = this->entities.begin();
		entity_set_iterator last = this->entities.end();

		auto filter = [] (const entity_set_iterator& iterator) {
			return !match<Filter>()(*iterator);
		};

		return filter_view<no_type, entity_set_iterator>(first, last, filter);
	}

	/**
	 * Returns a view that iterates over all entities and outputs the transformed entity
	 */
	template <typename Transform>
	[[nodiscard]] auto transform(const Transform& transform) {
		entity_set_iterator first = this->entities.begin();
		entity_set_iterator last = this->entities.end();

		return transform_view<no_type, entity_set_iterator>(first, last, transform);
	}

	/**
	 * Returns a view that iterates over all entities which satisfy the given filter and outputs the transformed entity
	 */
	template <typename Filter, typename Transform>
	[[nodiscard]] auto filter_transform(const Filter& filter, const Transform& transform) {
		entity_set_iterator first = this->entities.begin();
		entity_set_iterator last = this->entities.end();

		return filter_transform_view<no_type, entity_set_iterator>(first, last, filter, transform);
	}

	/**
	 * Returns a view that iterates over all entities which satisfy the given filter and outputs the transformed entity
	 */
	template <typename Filter, typename Transform>
	[[nodiscard]] auto filter_transform(const Transform& transform) {
		entity_set_iterator first = this->entities.begin();
		entity_set_iterator last = this->entities.end();

		auto filter = [] (const entity_set_iterator& iterator) {
			return !match<Filter>()(*iterator);
		};

		return filter_transform_view<no_type, entity_set_iterator>(first, last, filter, transform);
	}
	
	/**
	 * Returns an iterator which iterates over all entities which satisfy the view type
	 */
	template <typename... Type>
	[[nodiscard]] auto view() noexcept {
		if constexpr (sizeof...(Type) == 1)
			if constexpr (std::is_base_of_v<RC, Type...>)
				return view(type<conj<Type...>>{});
			else 
				return view(type<Type...>{});
		else 
			return view(type<conj<Type...>>{});
		
	}
};

typedef Registry<std::uint16_t> Registry16;
typedef Registry<std::uint32_t> Registry32;
typedef Registry<std::uint64_t> Registry64;
	
};