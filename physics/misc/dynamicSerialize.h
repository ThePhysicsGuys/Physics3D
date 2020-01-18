#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <type_traits>
#include <stdexcept>
#include <initializer_list>

#include "serializeBasicTypes.h"

class SerializationException : public std::exception {
public:
	SerializationException() = default;
	SerializationException(const char* text) : std::exception(text) {}
};

template<typename BaseType>
class DynamicSerializerRegistry {
public:
	struct DynamicSerializer {
		int serializerID;

		DynamicSerializer(int serializerID) : serializerID(serializerID) {}

		virtual void serialize(const BaseType& object, std::ostream& ostream) const = 0;
		virtual BaseType* deserialize(std::istream& istream) const = 0;
	};

	template<typename ConcreteType>
	class ConcreteDynamicSerializer : public DynamicSerializer {
		void (*serializeFunc)(const ConcreteType&, std::ostream&);
		ConcreteType* (*deserializeFunc)(std::istream&);
	public:
		ConcreteDynamicSerializer(void (*sf)(const ConcreteType&, std::ostream&), ConcreteType* (*dsf)(std::istream&), int serializerID) :
			DynamicSerializer(serializerID), serializeFunc(sf), deserializeFunc(dsf) {}

		virtual void serialize(const BaseType& object, std::ostream& ostream) const override {
			const ConcreteType& concreteObject = static_cast<const ConcreteType&>(object);
			this->serializeFunc(concreteObject, ostream);
		}
		virtual BaseType* deserialize(std::istream& istream) const override {
			return this->deserializeFunc(istream);
		}
	};

private:
	std::unordered_map<std::type_index, const DynamicSerializer*> serializeRegistry;
	std::map<int, const DynamicSerializer*> deserializeRegistry;

public:
	DynamicSerializerRegistry() = default;
	DynamicSerializerRegistry(std::initializer_list<std::pair<std::type_index, const DynamicSerializer*>> initList) : serializeRegistry(), deserializeRegistry() {
		for(const std::pair<std::type_index, const DynamicSerializer*>& item : initList) {
			const DynamicSerializer* ds = item.second;
			serializeRegistry.emplace(item.first, ds);
			if(deserializeRegistry.find(ds->serializerID) != deserializeRegistry.end()) throw std::logic_error("Duplicate serializerID?");
			deserializeRegistry.emplace(ds->serializerID, ds);
		}
	}
	
	template<typename ConcreteType, typename = typename std::enable_if<std::is_base_of<BaseType, ConcreteType>::value, int>::type>
	void registerSerializerDeserializer(void (*serializeFunc)(const ConcreteType&, std::ostream&), ConcreteType* (*deserializeFunc)(std::istream&), int serializerID) {
		if(deserializeRegistry.find(serializerID) != deserializeRegistry.end()) {
			throw std::logic_error("A serializer with this id has already been defined!");
		}

		std::type_index ti = typeid(ConcreteType);
		if(serializeRegistry.find(ti) != serializeRegistry.end()) {
			throw std::logic_error("A serializer for this type has already been defined!");
		}

		ConcreteDynamicSerializer<ConcreteType>* newSerializer = new ConcreteDynamicSerializer<ConcreteType>(serializeFunc, deserializeFunc, serializerID);

		serializeRegistry.emplace(ti, newSerializer);
		deserializeRegistry.emplace(serializerID, newSerializer);
	}

	template<typename ConcreteType, typename = typename std::enable_if<std::is_base_of<BaseType, ConcreteType>::value, int>::type>
	void registerSerializerDeserializer(void (*serialize)(const ConcreteType&, std::ostream&), ConcreteType* (*deserialize)(std::istream&)) {
		int i = 0;
		while(deserializeRegistry.find(i++) != deserializeRegistry.end());

		registerSerializerDeserializer(serialize, deserialize, i);
	}

	void serialize(const BaseType& object, std::ostream& ostream) {
		auto location = serializeRegistry.find(typeid(object));
		if(location == serializeRegistry.end()) {
			throw SerializationException("This class is not in the serialization registry!");
		}
		const DynamicSerializer* serializer = (*location).second;
		::serialize<int>(serializer->serializerID, ostream);
		serializer->serialize(object, ostream);
	}

	BaseType* deserialize(std::istream& istream) {
		int serialID = ::deserialize<int>(istream);
		auto location = deserializeRegistry.find(serialID);
		if(location == deserializeRegistry.end()) {
			throw SerializationException("Invalid dynamic class ID!");
		}
		const DynamicSerializer* deserializer = (*location).second;
		return deserializer->deserialize(istream);
	}
};

