#pragma once

class Bindable {
protected:
	unsigned int id;
public:

	unsigned int getID() {
		return id;
	}

	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual void close() = 0;
};