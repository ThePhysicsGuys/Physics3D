#pragma once

typedef unsigned int GLID;

class Bindable {
protected:
	GLID id;

	inline Bindable() : id(0) {}
	inline Bindable(GLID id) : id(id) {}

public:

	inline GLID getID() const {
		return id;
	}

	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual void close() = 0;
};