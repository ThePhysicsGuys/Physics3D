#pragma once

template<typename T>
struct Tracker {
public:
	static int created;
	static int alive;

	Tracker() {
		created++;
		alive++;
	}

	Tracker(const Tracker&) {
		created++;
		alive++;
	}

protected:
	~Tracker() {
		alive--;
	}
};

template<typename T> 
int Tracker<T>::created(0);

template<typename T>
int Tracker<T>::alive(0);