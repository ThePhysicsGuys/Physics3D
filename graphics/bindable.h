#pragma once

typedef unsigned int GLID;

namespace Graphics {

class Bindable {
protected:
	GLID id;

	Bindable();
	Bindable(GLID id);

public:
	GLID getID() const;

	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual void close() = 0;
};

};