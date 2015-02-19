/***
  This file is part of libblake2

  Copyright 2015 Mirco Tischler

  libblake2 is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  libblake2 is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with libblake2; If not, see <http://www.gnu.org/licenses/>.
 ***/

#pragma once

#include <array>
#include <cstring>
#include <vector>

namespace Blake2 {

using std::vector;

template<class Container, size_t size>
class SubrangeAdaptor {
    public:

	SubrangeAdaptor(const Container &c) : start(0), cont(c) { }
	SubrangeAdaptor() = delete;
	SubrangeAdaptor(const SubrangeAdaptor&) = default;
	SubrangeAdaptor(SubrangeAdaptor &&) = default;
	SubrangeAdaptor<Container, size>& operator=(const SubrangeAdaptor&) = default;
	SubrangeAdaptor<Container, size>& operator=(SubrangeAdaptor&&) = default;
	~SubrangeAdaptor() = default;

	SubrangeAdaptor operator++() {
		auto t_size = sizeof(uint64_t) / sizeof(typename Container::value_type);
		assert(start + size < cont.size() * t_size);
		start += size;

		return *this;
	}

	uint64_t operator[](const size_t &index) const {
		assert(index < size);
		auto t_size = sizeof(uint64_t) / sizeof(typename Container::value_type);

		auto i = (start + index) * t_size;
		// simulate the padding so we don't have to copy the input
		if (i >= cont.size())
			return 0;
		else if (i + t_size >= cont.size()) {
			auto result = 0u;
			memcpy(&result, cont.data() + i, cont.size() - i);

			return result;
		}

		return *reinterpret_cast<const uint64_t*> (cont.data() + i);
	}

    private:
	size_t start;
	const Container &cont;
};

template<>
class SubrangeAdaptor<vector<uint64_t>, 16> {
    public:

	SubrangeAdaptor(const vector<uint64_t> &v) : start(0), vec(v) { }

	SubrangeAdaptor operator++() {
		assert(start + 16 < vec.size());
		start += 16;

		return *this;
	}

	uint64_t operator[](const size_t & index) const {
		assert(index < 16);

		auto i = start + index;
		if (i >= vec.size())

			return 0u;

		return vec.at(i);
	}
    private:
	size_t start;
	const vector<uint64_t> &vec;
};

template<>
class SubrangeAdaptor<const char *, 16> {
    public:

	SubrangeAdaptor(const char *d, const size_t &len) : start(0), data(d), size(len) {
		if (len < 16 * 8) {
			p.resize(16 * 8);
			memcpy(p.data(), d, len);
		}
	}

	SubrangeAdaptor operator++() {
		auto offset = (start + 16) * sizeof(uint64_t);
		assert(offset < size);

		if (offset + 16 * 8 > size) {
			p.resize(16 * 8);
			memcpy(p.data(), &data[offset], size - offset);
		}

		start += 16;

		return *this;
	}

	uint64_t operator[](const size_t &index) const {
		assert(index < 16);

		if (!p.empty()) {
			return p[index];
		}

		auto offset = (start + index)*8;

		return *reinterpret_cast<const uint64_t*> (data + offset);
	}

    private:
	size_t start;
	const char *data;
	size_t size;
	vector<uint64_t> p;
};

} // namespace Blake2
