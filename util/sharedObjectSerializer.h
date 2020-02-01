#pragma once

#include <map>
#include <vector>
#include <assert.h>
#include <limits>

#include "serializeBasicTypes.h"

template<typename T, typename SerializeID = uint32_t>
class SharedObjectSerializer {
	SerializeID curPredefinedID = 0;
	SerializeID curDynamicID = std::numeric_limits<SerializeID>::max();

	std::vector<T> itemsYetToSerialize;
public:
	std::map<T, SerializeID> objectToIDMap;

	SharedObjectSerializer() = default;
	template<typename ListType>
	SharedObjectSerializer(const ListType& knownObjects) {
		for(const T& obj : knownObjects) {
			addPredefined(obj);
		}
	}

	void addPredefined(const T& obj) {
		assert(objectToIDMap.find(obj) == objectToIDMap.end());
		objectToIDMap.emplace(obj, curPredefinedID);
		curPredefinedID++;
	}

	void include(const T& obj) {
		if(objectToIDMap.find(obj) == objectToIDMap.end()) {
			itemsYetToSerialize.push_back(obj);
			objectToIDMap.emplace(obj, curDynamicID);
			curDynamicID--;
		}
	}

	// The given deserializer must be of the form 'serialize(T, std::ostream&)'. The object may be passed by ref or const ref
	template<typename Serializer>
	void serializeRegistry(Serializer serialize, std::ostream& ostream) {
		::serialize<SerializeID>(static_cast<SerializeID>(itemsYetToSerialize.size()), ostream);
		for(const T& item : itemsYetToSerialize) {
			serialize(item, ostream);
		}
		itemsYetToSerialize.clear();
	}

	void serializeIDFor(const T& obj, std::ostream& ostream) const {
		auto found = objectToIDMap.find(obj);
		if(found == objectToIDMap.end()) throw SerializationException("The given object was not registered!");

		::serialize<SerializeID>((*found).second, ostream);
	}
};

template<typename T, typename SerializeID = uint32_t>
class SharedObjectDeserializer {
	SerializeID curPredefinedID = 0;
	SerializeID curDynamicID = std::numeric_limits<SerializeID>::max();

public:
	std::map<SerializeID, T> IDToObjectMap;

	SharedObjectDeserializer() = default;
	template<typename ListType>
	SharedObjectDeserializer(const ListType& knownObjects) {
		for(const T& obj : knownObjects) {
			addPredefined(obj);
		}
	}

	void addPredefined(const T& obj) {
		IDToObjectMap.emplace(curPredefinedID, obj);
		curPredefinedID++;
	}

	// The given deserializer must be of the form 'T deserialize(std::istream&)', it may return references or const refs. 
	template<typename Deserializer>
	void deserializeRegistry(Deserializer deserialize, std::istream& istream) {
		size_t numberOfObjectsToDeserialize = ::deserialize<SerializeID>(istream);
		for(size_t i = 0; i < numberOfObjectsToDeserialize; i++) {
			T object = deserialize(istream);
			IDToObjectMap.emplace(curDynamicID, object);
			curDynamicID--;
		}
	}

	T deserializeObject(std::istream& istream) const {
		SerializeID id = ::deserialize<SerializeID>(istream);

		auto found = IDToObjectMap.find(id);
		if(found == IDToObjectMap.end()) throw SerializationException("There is no associated object for the id " + std::to_string(id));

		return (*found).second;
	}
};
