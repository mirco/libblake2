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

#include "Blake2b.hpp"
#include "blake2b.h"

#include <array>
#include <exception>
#include <iostream>
#include <iomanip>
#include <string>

using std::array;
using std::exception;
using std::string;

extern "C" {

struct Blake2b {
	Blake2::Blake2b b;
};

blake2b *blake2b_new() {
	return new blake2b;
}

void blake2b_delete(blake2b* b) {
	delete b;
}

int blake2b_set_digest_length(blake2b *b, const size_t digest_len) {
	try {
		b->b.set_digest_length(digest_len);
	} catch (exception &e) {
		return -1;
	}
	return 0;
}

//int blake2b_set_key(blake2b *b, const char *const key, const size_t key_len);

int blake2b_set_salt(blake2b *b, const char *const salt, const size_t salt_len) {
	try {
		array<uint64_t, 2> s;
		s.fill(0u);
		memcpy(s.data(), salt, salt_len);
		b->b.set_salt(s);
	} catch (exception & e) {
		return -1;
	}
	return 0;
}

int blake2b_set_personalization(blake2b *b, const char * const personalization, const size_t personalization_len) {
	try {
		array<uint64_t, 2> p;
		p.fill(0u);
		memcpy(p.data(), personalization, personalization_len);
		b->b.set_personalization(p);
	} catch (exception &e) {
		return -1;
	}
	return 0;
}

int blake2b_hash(blake2b *b, const char *const message, const size_t len, uint8_t *const hash) {
	assert(b);
	assert(message);
	assert(hash);
	try {
		Blake2::Blake2b::hash_t h = b->b(message, len);
		memcpy(hash, h.data(), 64 * sizeof(uint8_t));
		return 0;
	} catch (exception &e) {
		return -1;
	}
}

int blake2b_hash_to_hex(const uint8_t * const hash, const size_t hlen, char *const output) {
	assert(hash);
	assert(output);
	try {
		size_t i;
		std::stringstream result;
		result << std::setfill('0') << std::hex;
		for(i = 0; i < hlen; ++i)
			result << std::setw(2) << (unsigned int)hash[i];
		memcpy(output, result.str().data(), hlen*2 + 1);
		return 0;
	} catch (exception &e) {
		return -1;
	}
}

int blake2b_hex_to_hash(const char *const hex, const size_t hexlen, uint8_t * const hash, const size_t hashlen) {
	assert(hex);
	assert(hash);
	assert(hexlen == hashlen * 2 + 1);
	try {
		auto j = hash;
		for (auto i = hex; (size_t)(i - hex) < hexlen; i += 2) {
			std::stringstream tmp;
			tmp << std::hex << *i << *(i + 1);
			unsigned int result;
			tmp >> result;
			*j = result;
			++j;
		}
		return 0;
	} catch (exception &e) {
		return -1;
	}
}

} // extern "C"