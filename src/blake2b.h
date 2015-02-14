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

#ifndef BLAKE2B_CAPI_H
#define	BLAKE2B_CAPI_H

#if __GNUC__ >= 4
#define BLAKE2_EXPORT_SYMBOL __attribute__ ((visibility("default")))
#else
#define BLAKE2_EXPORT_SYMBOL
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <inttypes.h>

struct BLAKE2_EXPORT_SYMBOL Blake2b;

typedef struct Blake2b blake2b;

BLAKE2_EXPORT_SYMBOL blake2b *blake2b_new();

BLAKE2_EXPORT_SYMBOL void blake2b_delete(blake2b* b);

BLAKE2_EXPORT_SYMBOL int blake2b_set_digest_length(blake2b *b, const size_t digest_len);

int blake2b_set_key(blake2b *b, const char *const key, const size_t key_len);

BLAKE2_EXPORT_SYMBOL int blake2b_set_salt(blake2b *b, const char *const salt, const size_t salt_len);

BLAKE2_EXPORT_SYMBOL int blake2b_set_personalization(blake2b *b, const char * const personalization, const size_t personalization_len);

BLAKE2_EXPORT_SYMBOL int blake2b_hash(blake2b *b, const char *const message, const size_t len, uint8_t *const hash);

BLAKE2_EXPORT_SYMBOL int blake2b_hash_to_hex(const uint8_t * const hash, const size_t hlen, char *const output);

BLAKE2_EXPORT_SYMBOL int blake2b_hex_to_hash(const char *const hex, const size_t hexlen, uint8_t *const hash, const size_t hashlen);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif	// BLAKE2B_CAPI_H

