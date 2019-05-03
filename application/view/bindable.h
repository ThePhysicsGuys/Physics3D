#pragma once

class Bindable {
public:
	unsigned int id;

	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual void close() = 0;
};