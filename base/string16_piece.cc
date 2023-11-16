// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Copied from strings/stringpiece.cc with modifications

#include <algorithm>
#include <iostream>  // NOLINT

#include "base/utf_string_conversions.h"
#include "base/string16_piece.h"

#define CHAR16_MAX  65536

namespace base {

typedef String16Piece::size_type size_type;

std::ostream& operator<<(std::ostream& o, const String16Piece& piece) {
  return o << UTF16ToUTF8(piece.as_string16());
}

bool operator==(const String16Piece& x, const String16Piece& y) {
  if (x.size() != y.size())
    return false;

  return String16Piece::wordmemcmp(x.data(), y.data(), x.size()) == 0;
}

void String16Piece::CopyToString(string16* target) const {
  if (empty()) {
    target->clear();
  } else {
    target->assign(data(), size());
  }
}

void String16Piece::AppendToString(string16* target) const {
  if (!empty())
    target->append(data(), size());
}

size_type String16Piece::copy(char16* buf, size_type n, size_type pos) const {
  size_type ret = std::min(length_ - pos, n);
  c16memcpy(buf, ptr_ + pos, ret);
  return ret;
}

size_type String16Piece::find(const String16Piece& s, size_type pos) const {
  if (pos > length_)
    return npos;

  const char16* result = std::search(ptr_ + pos, ptr_ + length_,
                                   s.ptr_, s.ptr_ + s.length_);
  const size_type xpos = result - ptr_;
  return xpos + s.length_ <= length_ ? xpos : npos;
}

size_type String16Piece::find(char16 c, size_type pos) const {
  if (pos >= length_)
    return npos;

  const char16* result = std::find(ptr_ + pos, ptr_ + length_, c);
  return result != ptr_ + length_ ? result - ptr_ : npos;
}

size_type String16Piece::rfind(const String16Piece& s, size_type pos) const {
  if (length_ < s.length_)
    return npos;

  if (s.empty())
    return std::min(length_, pos);

  const char16* last = ptr_ + std::min(length_ - s.length_, pos) + s.length_;
  const char16* result = std::find_end(ptr_, last, s.ptr_, s.ptr_ + s.length_);
  return result != last ? result - ptr_ : npos;
}

size_type String16Piece::rfind(char16 c, size_type pos) const {
  if (length_ == 0)
    return npos;

  for (size_type i = std::min(pos, length_ - 1); ; --i) {
    if (ptr_[i] == c)
      return i;
    if (i == 0)
      break;
  }
  return npos;
}

// For each character in characters_wanted, sets the index corresponding
// to the ASCII code of that character to 1 in table.  This is used by
// the find_.*_of methods below to tell whether or not a character is in
// the lookup table in constant time.
// The argument `table' must be an array that is large enough to hold all
// the possible values of an unsigned char16.  Thus it should be be declared
// as follows:
//   bool table[CHAR16_MAX + 1]
static inline void BuildLookupTable(const String16Piece& characters_wanted,
                                    bool* table) {
  const size_type length = characters_wanted.length();
  const char16* const data = characters_wanted.data();
  for (size_type i = 0; i < length; ++i) {
    table[static_cast<unsigned char16>(data[i])] = true;
  }
}

size_type String16Piece::find_first_of(const String16Piece& s,
                                     size_type pos) const {
  if (length_ == 0 || s.length_ == 0)
    return npos;

  // Avoid the cost of BuildLookupTable() for a single-character search.
  if (s.length_ == 1)
    return find_first_of(s.ptr_[0], pos);

  bool lookup[CHAR16_MAX + 1] = { false };
  BuildLookupTable(s, lookup);
  for (size_type i = pos; i < length_; ++i) {
    if (lookup[static_cast<unsigned char16>(ptr_[i])]) {
      return i;
    }
  }
  return npos;
}

size_type String16Piece::find_first_not_of(const String16Piece& s,
                                         size_type pos) const {
  if (length_ == 0)
    return npos;

  if (s.length_ == 0)
    return 0;

  // Avoid the cost of BuildLookupTable() for a single-character search.
  if (s.length_ == 1)
    return find_first_not_of(s.ptr_[0], pos);

  bool lookup[CHAR16_MAX + 1] = { false };
  BuildLookupTable(s, lookup);
  for (size_type i = pos; i < length_; ++i) {
    if (!lookup[static_cast<unsigned char16>(ptr_[i])]) {
      return i;
    }
  }
  return npos;
}

size_type String16Piece::find_first_not_of(char16 c, size_type pos) const {
  if (length_ == 0)
    return npos;

  for (; pos < length_; ++pos) {
    if (ptr_[pos] != c) {
      return pos;
    }
  }
  return npos;
}

size_type String16Piece::find_last_of(
    const String16Piece& s, size_type pos) const {
  if (length_ == 0 || s.length_ == 0)
    return npos;

  // Avoid the cost of BuildLookupTable() for a single-character search.
  if (s.length_ == 1)
    return find_last_of(s.ptr_[0], pos);

  bool lookup[CHAR16_MAX + 1] = { false };
  BuildLookupTable(s, lookup);
  for (size_type i = std::min(pos, length_ - 1); ; --i) {
    if (lookup[static_cast<unsigned char16>(ptr_[i])])
      return i;
    if (i == 0)
      break;
  }
  return npos;
}

size_type String16Piece::find_last_not_of(const String16Piece& s,
                                        size_type pos) const {
  if (length_ == 0)
    return npos;

  size_type i = std::min(pos, length_ - 1);
  if (s.length_ == 0)
    return i;

  // Avoid the cost of BuildLookupTable() for a single-character search.
  if (s.length_ == 1)
    return find_last_not_of(s.ptr_[0], pos);

  bool lookup[CHAR16_MAX + 1] = { false };
  BuildLookupTable(s, lookup);
  for (; ; --i) {
    if (!lookup[static_cast<unsigned char16>(ptr_[i])])
      return i;
    if (i == 0)
      break;
  }
  return npos;
}

size_type String16Piece::find_last_not_of(char16 c, size_type pos) const {
  if (length_ == 0)
    return npos;

  for (size_type i = std::min(pos, length_ - 1); ; --i) {
    if (ptr_[i] != c)
      return i;
    if (i == 0)
      break;
  }
  return npos;
}

String16Piece String16Piece::substr(size_type pos, size_type n) const {
  if (pos > length_) pos = length_;
  if (n > length_ - pos) n = length_ - pos;
  return String16Piece(ptr_ + pos, n);
}

const String16Piece::size_type String16Piece::npos = size_type(-1);

}  // namespace base
