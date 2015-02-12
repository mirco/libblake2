/* 
 * File:   VectorRange.hpp
 * Author: mirco
 *
 * Created on 12. Februar 2015, 03:21
 */

#pragma once

#include <array>
#include <vector>

namespace Blake2 {

template<class Container, size_t size>
class VectorRange {
    public:

	VectorRange(const Container &c) : start(0), cont(&c) { }

	VectorRange operator++() {
		auto t_size = sizeof(uint64_t) / sizeof(typename Container::value_type);
		assert(start + size < cont->size() * t_size);
		start += size;

		return *this;
	}

	uint64_t operator[](const size_t &index) const {
		assert(index < size);
		auto t_size = sizeof(uint64_t) / sizeof(typename Container::value_type);

		auto i = (start + index) * t_size;
		// simulate the padding so we don't have to copy the input
		if (i >= cont->size())
			return 0;
		else if (i + t_size >= cont->size()) {
			auto result = 0u;
			memcpy(&result, cont->data() + i, cont->size() - i);

			return result;
		}

		return *reinterpret_cast<const uint64_t*> (cont->data() + i);
	}

    private:
	size_t start;
	const Container *cont;
};

template<>
class VectorRange<vector<uint64_t>, 16> {
    public:

	VectorRange(const vector<uint64_t> &v) : start(0), vec(&v) { }

	VectorRange operator++() {
		assert(start + 16 < vec->size());
		start += 16;

		return *this;
	}

	uint64_t operator[](const size_t & index) const {
		assert(index < 16);

		auto i = start + index;
		if (i >= vec->size())

			return 0u;

		return vec->at(i);
	}
    private:
	size_t start;
	const vector<uint64_t> *vec;
};

template<>
class VectorRange<const char *, 16> {
    public:

	VectorRange(const char *d, const size_t &len) : start(0), data(d), size(len) { }

	VectorRange operator++() {
		assert((start + 16) * sizeof(uint64_t) < size);
		start += 16;

		return *this;
	}

	uint64_t operator[](const size_t &index) const {
		assert(index < 16);

		auto i = (start + index)*8;
		if (i >= size)
			return 0u;
		if(i+8>=size){
			auto result = 0u;
			memcpy(&result, data + i, size - i);
			return result;
		}
		return *reinterpret_cast<const uint64_t*> (data + i);
	}
    private:
	size_t start;
	const char *data;
	size_t size;
};

} // namespace Blake2
