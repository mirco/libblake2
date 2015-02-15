/* 
 * File:   test-blake2b.c
 * Author: mirco
 *
 * Created on 15. Februar 2015, 16:18
 */

#include <gtest/gtest.h>

#include "blake2b.h"

namespace {

auto empty_hash = "786A02F742015903C6C6FD852552D272912F4740E15847618A86E217F71F5419D25E1031AFEE585313896444934EB04B903A685B1448B755D56F701AFE9BE2CE";
auto pangram_hash = "A8ADD4BDDDFD93E4877D2746E62817B116364A1FA7BC148D95090BC7333B3673F82401CF7AA2E4CB1ECD90296E3F14CB5413F8ED77BE73045B13914CDCD6A918";

TEST(TestBlake2b, AllocDealloc){
	blake2b *b = blake2b_new();
	ASSERT_NE(nullptr, b);
	blake2b_delete(b);
}

class Blake2bTest: public ::testing::Test {
    protected:
	virtual void SetUp(){
		b = blake2b_new();
		blake2b_set_digest_length(b, 64);
	}
	virtual void TearDown() {
		blake2b_delete(b);
	}

	blake2b *b;
};

TEST_F(Blake2bTest, emptyString){
	uint8_t hash[64];
	auto ret = blake2b_hash(b, "", 0, hash);
	ASSERT_EQ(0, ret);
	char hex[129];
	ret = blake2b_hash_to_hex(hash, 64, hex);
	ASSERT_EQ(0, ret);
	ASSERT_STRCASEEQ(empty_hash, hex);
}

TEST_F(Blake2bTest, pangram){
	uint8_t hash[64];
	auto s = "The quick brown fox jumps over the lazy dog";
	auto ret = blake2b_hash(b, s, 43, hash);
	ASSERT_EQ(0, ret);
	char hex[129];
	ret = blake2b_hash_to_hex(hash, 64, hex);
	ASSERT_EQ(0, ret);
	ASSERT_STRCASEEQ(pangram_hash, hex);
}

TEST(testBlake2b, hexBinConvert) {
	uint8_t hash[64];
	auto ret = blake2b_hex_to_hash(empty_hash, 129, hash, 64)
	ASSERT_EQ(0, ret);
	char hex[129];
	auto ret = blake2b_hash_to_hex(hash, 64, hex);
	ASSERT_EQ(0, ret);
	ASSERT_STRCASEEQ(empty_hash, hex);
}

} // nanespace

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	
	return RUN_ALL_TESTS();
}

