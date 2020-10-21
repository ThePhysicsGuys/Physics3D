#pragma once

struct iterator_end {};

template<typename Iterator, typename IteratorEnd>
class default_iterator {
private:
	Iterator current;
	Iterator end;

public:
	default_iterator() = default;
	default_iterator(const Iterator& start, const Iterator& end) : current(start), end(end) {

	}

	default_iterator& operator++() {
		++current;

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

template<typename Iterator, typename IteratorEnd, typename Filter>
class filter_iterator {
private:
	Iterator current;
	Iterator end;
	Filter filter;

public:
	filter_iterator() = default;
	filter_iterator(const Iterator& start, const Iterator& end, const Filter& filter) : current(start), end(end), filter(filter) {
		while (current != end && !filter(current))
			++current;
	}

	filter_iterator& operator++() {
		do {
			++current;
		} while (current != end && !filter(current));

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

template<typename Iterator, typename IteratorEnd, typename Transform>
class transform_iterator {
private:
	Iterator current;
	Iterator end;
	Transform transform;

public:
	transform_iterator() = default;
	transform_iterator(const Iterator& start, const Iterator& end, const Transform& transform) : current(start), end(end), transform(transform) {

	}

	transform_iterator& operator++() {
		++current;

		return *this;
	}

	bool operator!=(IteratorEnd) const {
		return current != end;
	}

	decltype(transform(current)) operator*() const {
		return transform(current);
	}

	bool operator==(Iterator iterator) const {
		return current == iterator;
	}
};

template<typename Iterator, typename IteratorEnd, typename Filter, typename Transform>
class filter_transform_iterator {
private:
	Iterator current;
	Iterator end;
	Filter filter;
	Transform transform;

public:
	filter_transform_iterator() = default;
	filter_transform_iterator(const Iterator& start, const Iterator& end, const Filter& filter, const Transform& transform) : current(start), end(end), filter(filter), transform(transform) {
		while (current != end && !filter(current))
			++current;
	}

	filter_transform_iterator& operator++() {
		do {
			++current;
		} while (current != end && !filter(current));

		return *this;
	}

	bool operator!=(IteratorEnd) const {
		return current != end;
	}

	decltype(transform(current)) operator*() const {
		return transform(current);
	}

	bool operator==(Iterator iterator) const {
		return current == iterator;
	}
};