#ifndef PUBLIC_BASE_HASH_H_
#define PUBLIC_BASE_HASH_H_

#include <string>
#include "public/base/basictypes.h"
#include "public/base/string_piece.h"

namespace base {

uint64 Fingerprint(const StringPiece& str);

uint64 Fingerprint(const void* key, int len);

uint32 Fingerprint32(const StringPiece& str);

uint32 Fingerprint32(const void* key, int len);

void FingerprintToString(uint64 fp, std::string* str);

std::string FingerprintToString(uint64 fp);

uint64 StringToFingerprint(const std::string& str);

// MurmurHash2, 64-bit versions, by Austin Appleby.
// http://sites.google.com/site/murmurhash/
uint64 MurmurHash64A(const void* key, int len, uint32 seed);

uint32 MurmurHash32A(const void* key, int len, uint32 seed);

uint32 MurmurHash3_32(const void* key, int len, uint32 seed);

static const uint64 kEmptyContentHashLow = 7113472399480571277UL;
static const uint64 kEmptyContentHashHigh = 7809847782465536322UL;

// 128 bits FNV checksum.
// Sample useage:
//   uint64 digest[2];
//   fnv128(data, len, &digest)
//
void FNV128(const char* data, int len, void* digest);

}  // namespace base

#endif  // PUBLIC_BASE_HASH_H_
