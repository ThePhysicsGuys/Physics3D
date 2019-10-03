#pragma once

class Bindable {
protected:
	unsigned int id;

	Bindable() : id(0) {

	}

	Bindable(unsigned int id) : id(id) {

	}

public:
	unsigned int getID() {
		return id;
	}

	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual void close() = 0;
};