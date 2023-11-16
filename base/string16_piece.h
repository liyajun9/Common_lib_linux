// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Copied from strings/stringpiece.h with modifications
// Copied from base/string_piece.h with modifications
//
// A string-like object that points to a sized piece of memory.
//
// Functions or methods may use const String16Piece& parameters to accept either
// a "const char16*" or a "string" value that will be implicitly converted to
// a String16Piece.  The implicit conversion means that it is often appropriate
// to include this .h file in other files rather than forward-declaring
// String16Piece as would be appropriate for most other Google classes.
//
// Systematic usage of String16Piece is encouraged as it will reduce unnecessary
// conversions from "const char16*" to "string" and back again.
//

#ifndef PUBLIC_BASE_STRING16_PIECE_H_
#define PUBLIC_BASE_STRING16_PIECE_H_

#include <algorithm>
#include <iosfwd>
#include <string>

#include "base/basictypes.h"
#include "base/string16.h"

namespace base {

class String16Piece {
 public:
  typedef size_t size_type;

 private:
  const char16* ptr_;
  size_type     length_;

 public:
  // We provide non-explicit singleton constructors so users can pass
  // in a "const char16*" or a "string" wherever a "String16Piece" is
  // expected.
  String16Piece() : ptr_(NULL), length_(0) { }
  String16Piece(const char16* str)  // NOLINT
    : ptr_(str), length_((str == NULL) ? 0 : c16len(str)) { }
  String16Piece(const string16& str)  // NOLINT
    : ptr_(str.data()), length_(str.size()) { }
  String16Piece(const char16* offset, size_type len)
    : ptr_(offset), length_(len) { }

  // data() may return a pointer to a buffer with embedded NULs, and the
  // returned buffer may or may not be null terminated.  Therefore it is
  // typically a mistake to pass data() to a routine that expects a NUL
  // terminated string.
  const char16* data() const { return ptr_; }
  size_type size() const { return length_; }
  size_type length() const { return length_; }
  bool empty() const { return length_ == 0; }

  void clear() {
    ptr_ = NULL;
    length_ = 0;
  }
  void set(const char16* data, size_type len) {
    ptr_ = data;
    length_ = len;
  }
  void set(const char16* str) {
    ptr_ = str;
    length_ = str ? c16len(str) : 0;
  }

  char16 operator[](size_type i) const { return ptr_[i]; }

  void remove_prefix(size_type n) {
    ptr_ += n;
    length_ -= n;
  }

  void remove_suffix(size_type n) {
    length_ -= n;
  }

  int compare(const String16Piece& x) const {
    int r = wordmemcmp(ptr_, x.ptr_, std::min(length_, x.length_));
    if (r == 0) {
      if (length_ < x.length_) r = -1;
      else if (length_ > x.length_) r = +1;
    }
    return r;
  }

  string16 as_string16() const {
    // string16 doesn't like to take a NULL pointer even with a 0 size.
    return !empty() ? string16(data(), size()) : string16();
  }

  void CopyToString(string16* target) const;
  void AppendToString(string16* target) const;

  // Does "this" start with "x"
  bool starts_with(const String16Piece& x) const {
    return ((length_ >= x.length_) &&
            (wordmemcmp(ptr_, x.ptr_, x.length_) == 0));
  }

  // Does "this" end with "x"
  bool ends_with(const String16Piece& x) const {
    return ((length_ >= x.length_) &&
            (wordmemcmp(ptr_ + (length_-x.length_), x.ptr_, x.length_) == 0));
  }

  // standard STL container boilerplate
  typedef char16 value_type;
  typedef const char16* pointer;
  typedef const char16& reference;
  typedef const char16& const_reference;
  typedef ptrdiff_t difference_type;
  static const size_type npos;
  typedef const char16* const_iterator;
  typedef const char16* iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  iterator begin() const { return ptr_; }
  iterator end() const { return ptr_ + length_; }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(ptr_ + length_);
  }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(ptr_);
  }

  size_type max_size() const { return length_; }
  size_type capacity() const { return length_; }

  size_type copy(char16* buf, size_type n, size_type pos = 0) const;

  size_type find(const String16Piece& s, size_type pos = 0) const;
  size_type find(char16 c, size_type pos = 0) const;
  size_type rfind(const String16Piece& s, size_type pos = npos) const;
  size_type rfind(char16 c, size_type pos = npos) const;

  size_type find_first_of(const String16Piece& s, size_type pos = 0) const;
  size_type find_first_of(char16 c, size_type pos = 0) const {
    return find(c, pos);
  }
  size_type find_first_not_of(const String16Piece& s, size_type pos = 0) const;
  size_type find_first_not_of(char16 c, size_type pos = 0) const;
  size_type find_last_of(const String16Piece& s, size_type pos = npos) const;
  size_type find_last_of(char16 c, size_type pos = npos) const {
    return rfind(c, pos);
  }
  size_type find_last_not_of(
      const String16Piece& s, size_type pos = npos) const;
  size_type find_last_not_of(char16 c, size_type pos = npos) const;

  String16Piece substr(size_type pos, size_type n = npos) const;

  static int wordmemcmp(const char16* p, const char16* p2, size_type N) {
    return c16memcmp(p, p2, N);
  }
};

bool operator==(const String16Piece& x, const String16Piece& y);

inline bool operator!=(const String16Piece& x, const String16Piece& y) {
  return !(x == y);
}

inline bool operator<(const String16Piece& x, const String16Piece& y) {
  const int r = String16Piece::wordmemcmp(x.data(), y.data(),
                                        std::min(x.size(), y.size()));
  return ((r < 0) || ((r == 0) && (x.size() < y.size())));
}

inline bool operator>(const String16Piece& x, const String16Piece& y) {
  return y < x;
}

inline bool operator<=(const String16Piece& x, const String16Piece& y) {
  return !(x > y);
}

inline bool operator>=(const String16Piece& x, const String16Piece& y) {
  return !(x < y);
}

// allow String16Piece to be logged (needed for unit testing).
extern std::ostream& operator<<(std::ostream& o, const String16Piece& piece);

}  // namespace base

#endif  // PUBLIC_BASE_STRING16_PIECE_H_
