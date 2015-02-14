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
#include <cassert>
#include <cstring>
#include <string>
#include <vector>

namespace Blake2 {

using std::array;
using std::begin;
using std::memcpy;
using std::string;
using std::vector;

class Blake2b {
    public:
	using hash_t = array<uint64_t, 8>;
	using salt_t = array<uint64_t, 2>;
	using personalization_t = array<uint64_t, 2>;

	Blake2b(const unsigned int &digestLength,
		const unsigned int &keyLength,
		const salt_t &salt,
		const personalization_t &personalization);

	Blake2b() {
		setup_parameter_block();
	}


	hash_t operator()(const string &data);
	hash_t operator()(const vector<uint64_t> &data);
	hash_t operator()(const char *data, const size_t &len);

	void set_digest_length(const size_t &digest_length);
	void set_salt(const salt_t &salt);
	void set_personalization(const personalization_t &personalization);

	static uint64_t initialization_vector(const size_t &i) {
		assert(i < 8);
		return i_v[i];
	}

	static uint64_t permutation_matrix(const size_t &i, const size_t &j) {
		assert(j < 16);
		return p_m[i][j];
	}
	static string to_string(const hash_t &hash);

    private:
	using s_t = array<unsigned int, 16>;
	using sigma_t = array<s_t, 12>;

	void setup_parameter_block();
	hash_t initialize_h() const;
	template<class Container>
	hash_t hash_internal(const Container &v_m, const size_t &orig_size);

	static constexpr auto i_v = hash_t{
		{
			0x6a09e667f3bcc908ULL,
			0xbb67ae8584caa73bULL,
			0x3c6ef372fe94f82bULL,
			0xa54ff53a5f1d36f1ULL,
			0x510e527fade682d1ULL,
			0x9b05688c2b3e6c1fULL,
			0x1f83d9abfb41bd6bULL,
			0x5be0cd19137e2179ULL
		}
	};

	static constexpr auto p_m = sigma_t{
		{
			s_t
			{
				{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}
			},
			s_t
			{
				{14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3}
			},
			s_t
			{
				{ 11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4}
			},
			s_t
			{
				{ 7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8}
			},
			s_t
			{
				{ 9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13}
			},
			s_t
			{
				{ 2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9}
			},
			s_t
			{
				{ 12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11}
			},
			s_t
			{
				{ 13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10}
			},
			s_t
			{
				{ 6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5}
			},
			s_t
			{
				{ 10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0}
			},
			s_t
			{
				{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}
			},
			s_t
			{
				{ 14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3}
			}
		}
	};

	struct ParameterBlock {
		uint8_t digest_length;
		uint8_t key_length;
		uint8_t fanout;
		uint8_t depth;
		uint32_t leaf_length;
		uint64_t node_offset;
		uint8_t node_depth;
		uint8_t inner_length;
		uint8_t RFU[14];
		salt_t salt;
		personalization_t personalization;
	};

	// make this a union so we don't have to do even more ugly casts

	union ParameterBlockUnion {
		struct ParameterBlock pbs;
		array<uint64_t, 8> pba;
	};
	ParameterBlockUnion parameter_block;

};

} // namespace Blake2
