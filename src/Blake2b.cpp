/* 
 * File:   Blake2b.cpp
 * Author: mirco
 * 
 * Created on 9. Februar 2015, 18:19
 */

#include "Blake2b.hpp"
#include "VectorRange.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <climits>
#include <iomanip>
#include <string>
#include <vector>

namespace Blake2 {

// 
using std::array;
using std::advance;
using std::begin;
using std::end;
using std::hex;
using std::memcpy;
using std::setfill;
using std::setw;
using std::string;
using std::stringstream;
using std::vector;

// typedefs
using hash_t = Blake2b::hash_t;
using salt_t = Blake2b::salt_t;
using personalization_t = Blake2b::personalization_t;

// declaration of static data members
constexpr hash_t Blake2b::i_v;
constexpr Blake2b::sigma_t Blake2b::p_m;

// local typedefs
using counter_t = array<uint64_t, 2>;
using final_flag_t = array<uint64_t, 2>;
template<class Container>
using data_t = VectorRange<Container, 16>;
using state_vector_t = array<uint64_t, 16>;

#define HASH_INIT {0,0,0,0,0,0,0,0}
#define STATE_VECTOR_INIT {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

// forward declarations of static methods
template<class Container>
static data_t<Container> initialize_m(const Container &data, const size_t &len);
template<>
data_t<const char*> initialize_m<const char *>(const char * const &data, const size_t &len);
template<class Container>
static hash_t compress(
		       hash_t &h,
		       const data_t<Container> &m,
		       const array<uint64_t, 2> &t,
		       const array<uint64_t, 2 > &f);
static hash_t finalize(
		       const hash_t &h,
		       const array<uint64_t, 16> &v);
template<class Container>
static void round(
		  const unsigned int &index,
		  array<uint64_t, 16> &v,
		  const data_t<Container> &m);
template<class Container>
static void G(
	      const unsigned int &r,
	      const unsigned int &i,
	      uint64_t &a, uint64_t &b, uint64_t &c, uint64_t &d,
	      const data_t<Container> &m);

static constexpr uint64_t ror(const uint64_t &val, const size_t &n);

//
// impleentations
//

Blake2b::Blake2b(const unsigned int &digestLength,
		 const unsigned int &keyLength,
		 const salt_t &salt,
		 const personalization_t &personalization) {
	assert(digestLength >= 1 && digestLength <= 64);
	assert(keyLength <= 64);

	setup_parameter_block();
	
	parameter_block.pbs.digest_length = static_cast<uint8_t> (digestLength);
	parameter_block.pbs.key_length = static_cast<uint8_t> (keyLength);

	// we don't support tree hashing thus don't set leaf_length
	// node_offset, node_depth or inner_length

	auto it = begin(parameter_block.pba);
	advance(it, 8);
	for (auto &el : salt) {
		*it = el;
		++it;
	}

	for (auto &el : personalization) {

		*it = el;
		++it;
	}
}

string Blake2b::to_string(const hash_t& hash) {
	stringstream result;
	result << hex << setfill('0');
	auto d = reinterpret_cast<const uint8_t*> (hash.data());
	for (auto i = 0u; i < 64; i++)
		result << setw(2) << (uint64_t) d[i];

	return result.str();
}

void Blake2b::set_digest_length(const size_t &digest_length) {
	parameter_block.pbs.digest_length = static_cast<uint8_t> (digest_length);
}

void Blake2b::set_salt(const salt_t &salt) {
	parameter_block.pbs.salt = salt;
}

void Blake2b::set_personalization(const personalization_t &personalization) {
	parameter_block.pbs.personalization = personalization;
}

hash_t Blake2b::operator()(const string &data) {
	return hash_internal(data, data.size());
}

hash_t Blake2b::operator()(const vector<uint64_t> &data) {
	return hash_internal(data, data.size()*8);
}

hash_t Blake2b::operator()(const char *data, const size_t &len) {
	return hash_internal(data, len);
}

void Blake2b::setup_parameter_block() {
	parameter_block.pba.fill(0);

	parameter_block.pbs.fanout = static_cast<uint8_t> (1u);
	parameter_block.pbs.depth = static_cast<uint8_t> (1u);
}

template<class Container>
static data_t<Container> initialize_m(const Container& data, const size_t &len) {
	(void)len; // to silence unused parameter warning
	return data_t<Container>(data);
}

template<>
data_t<const char *> initialize_m<const char *>(const char *const &data, const size_t &len) {
	return data_t<const char *>(data, len);
}

template<class Container>
hash_t Blake2b::hash_internal(const Container &v_m, const size_t &orig_size) {
	auto h = initialize_h();
	auto m = initialize_m(v_m, orig_size);
	auto t = counter_t{{0, 0}};
	auto f = final_flag_t{{0, 0}};

	// Iterate through all complete 16 word chunks of the message.
	// If the message size is a multiple of 16*word size make sure to leave
	// the last chunk for finalization.
	while ((t[0] += 128) < orig_size) {
		h = compress(h, m, t, f);
		++m;
	}

	t[0] = orig_size;
	f[0] = ~0ULL;
	h = compress(h, m, t, f);

	return h;
}

hash_t Blake2b::initialize_h() const {
	auto h = hash_t{HASH_INIT};
	auto iI = begin(i_v);
	auto iP = begin(parameter_block.pba);

	for (auto &el : h) {
		el = *iI ^ *iP;
		++iI;
		++iP;
	}
	return h;
}

template<class Container>
static hash_t compress(
		       hash_t &h,
		       const data_t<Container> &m,
		       const counter_t &t,
		       const final_flag_t &f) {
	// initialize the state vector
	auto state_vector = state_vector_t{STATE_VECTOR_INIT};
	auto it = begin(state_vector);
	for (const auto &el : h) {
		*it = el;
		++it;
	}
	for (auto i = 0; i < 8; ++i) {
		*it = Blake2b::initialization_vector(i);
		++it;
	}
	state_vector[12] ^= t[0];
	state_vector[13] ^= t[1];
	state_vector[14] ^= f[0];
	state_vector[15] ^= f[1];

	for (auto i = 0u; i < 12; ++i)
		round(i, state_vector, m);

	return finalize(h, state_vector);
}

template<class Container>
static void round(const unsigned int &index, state_vector_t &v, const data_t<Container> &m) {
	// rows
	for (auto i = 0; i < 4; ++i)
		G(index, i, v[0 + i], v[4 + i], v[8 + i], v[12 + i], m);

	// diagonals
	G(index, 4, v[0], v[5], v[10], v[15], m);
	G(index, 5, v[1], v[6], v[11], v[12], m);
	G(index, 6, v[2], v[7], v[8], v[13], m);
	G(index, 7, v[3], v[4], v[9], v[14], m);
}

static hash_t finalize(const hash_t &h, const state_vector_t &v) {
	auto ret = hash_t{HASH_INIT};
	auto ih = begin(h);
	auto iv = begin(v);
	auto ir = begin(ret);

	while (ir != end(ret)) {
		*ir = *ih ^ *iv ^ *(iv + 8);
		++ih;
		++iv;
		++ir;
	}

	return ret;
}

template<class Container>
static void G(const unsigned int &r, const unsigned int &i, uint64_t &a, uint64_t &b, uint64_t &c,
	      uint64_t &d, const data_t<Container> &m) {
	a = a + b + m[Blake2b::permutation_matrix(r, 2 * i)];
	d = ror((d ^ a), 32u);
	c = c + d;
	b = ror((b ^ c), 24u);
	a = a + b + m[Blake2b::permutation_matrix(r, 2 * i + 1)];
	d = ror((d ^ a), 16u);
	c = c + d;
	b = ror((b ^ c), 63u);
}

static constexpr uint64_t ror(const uint64_t &val, const size_t &n) {
	return(val >> n) | (val << (sizeof(uint64_t) * CHAR_BIT - n));
}

} // namespace Blake2
