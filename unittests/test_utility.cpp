// MIT License
//
// Copyright (c) 2022 Paul Bowen-Huggett
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#include <gtest/gtest.h>

#include "icubaby/icubaby.hpp"

TEST (IsSurrogate, High) {
  EXPECT_FALSE (icubaby::is_high_surrogate (icubaby::first_high_surrogate - 1));
  EXPECT_TRUE (icubaby::is_high_surrogate (icubaby::first_high_surrogate));
  EXPECT_TRUE (icubaby::is_high_surrogate (icubaby::last_high_surrogate));
  EXPECT_FALSE (icubaby::is_high_surrogate (icubaby::last_high_surrogate + 1));
}

TEST (IsSurrogate, Low) {
  EXPECT_FALSE (icubaby::is_low_surrogate (icubaby::first_low_surrogate - 1));
  EXPECT_TRUE (icubaby::is_low_surrogate (icubaby::first_low_surrogate));
  EXPECT_TRUE (icubaby::is_low_surrogate (icubaby::last_low_surrogate));
  EXPECT_FALSE (icubaby::is_low_surrogate (icubaby::last_low_surrogate + 1));
}

TEST (IsSurrogate, Any) {
  EXPECT_FALSE (icubaby::is_surrogate (icubaby::first_high_surrogate - 1));
  EXPECT_TRUE (icubaby::is_surrogate (icubaby::first_high_surrogate));
  EXPECT_TRUE (icubaby::is_surrogate (icubaby::last_high_surrogate));
  EXPECT_TRUE (icubaby::is_surrogate (icubaby::last_high_surrogate + 1));
  EXPECT_TRUE (icubaby::is_surrogate (icubaby::first_low_surrogate - 1));
  EXPECT_TRUE (icubaby::is_surrogate (icubaby::first_low_surrogate));
  EXPECT_TRUE (icubaby::is_surrogate (icubaby::last_low_surrogate));
  EXPECT_FALSE (icubaby::is_surrogate (icubaby::last_low_surrogate + 1));
}

TEST (IsCodePointStart, Utf8) {
  EXPECT_TRUE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b0'0000000)))
      << "Single byte code point";
  EXPECT_TRUE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b0'1111111)))
      << "Single byte code point";

  EXPECT_TRUE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b110'00000)))
      << "First byte of a two byte code point";
  EXPECT_TRUE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b110'11111)))
      << "First byte of a two byte code point";

  EXPECT_TRUE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b1110'0000)))
      << "First byte of a three byte code point";
  EXPECT_TRUE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b1110'1111)))
      << "First byte of a three byte code point";

  EXPECT_TRUE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b11110'000)))
      << "First byte of a four byte code point";

  EXPECT_TRUE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b11110'111)))
      << "First byte of a four byte code point";

  EXPECT_FALSE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b10'000000)));
  EXPECT_FALSE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b10'111111)));
}

TEST (IsCodePointStart, Utf16) {
  EXPECT_TRUE (icubaby::is_code_point_start (char16_t{0x0000}));
  EXPECT_TRUE (icubaby::is_code_point_start (char16_t{0x0000}));
  EXPECT_TRUE (icubaby::is_code_point_start (
      static_cast<char16_t> (icubaby::first_high_surrogate - 1)));
  EXPECT_TRUE (icubaby::is_code_point_start (
      static_cast<char16_t> (icubaby::first_high_surrogate)));
  EXPECT_TRUE (icubaby::is_code_point_start (
      static_cast<char16_t> (icubaby::last_high_surrogate)));
  EXPECT_FALSE (icubaby::is_code_point_start (
      static_cast<char16_t> (icubaby::first_low_surrogate)));
  EXPECT_FALSE (icubaby::is_code_point_start (
      static_cast<char16_t> (icubaby::last_low_surrogate)));
  EXPECT_TRUE (icubaby::is_code_point_start (
      static_cast<char16_t> (icubaby::last_low_surrogate + 1)));
  EXPECT_TRUE (icubaby::is_code_point_start (char16_t{0xFFFF}));
}

TEST (IsCodePointStart, Utf32) {
  EXPECT_TRUE (icubaby::is_code_point_start (char32_t{0}));
  EXPECT_TRUE (icubaby::is_code_point_start (
      static_cast<char32_t> (icubaby::first_high_surrogate - 1)));
  EXPECT_FALSE (icubaby::is_code_point_start (icubaby::first_high_surrogate));
  EXPECT_FALSE (icubaby::is_code_point_start (icubaby::last_low_surrogate));
  EXPECT_TRUE (icubaby::is_code_point_start (
      static_cast<char32_t> (icubaby::last_low_surrogate + 1)));
  EXPECT_TRUE (icubaby::is_code_point_start (icubaby::max_code_point));
  EXPECT_FALSE (icubaby::is_code_point_start (
      static_cast<char32_t> (icubaby::max_code_point + 1)));
  EXPECT_FALSE (icubaby::is_code_point_start (char32_t{0xFFFFFFFF}));
}

namespace {

struct AsciiUtf8 : testing::Test {
  AsciiUtf8 () : CUs{'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'} {}
  std::vector<icubaby::char8> const CUs;
};

}  // end anonymous namespace

TEST_F (AsciiUtf8, Length) {
  EXPECT_EQ (11U, icubaby::length (std::begin (CUs), std::end (CUs)));
}
TEST_F (AsciiUtf8, Index) {
  auto begin = std::begin (CUs);
  auto end = std::end (CUs);
  auto it = begin;
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{0}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{1}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{2}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{3}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{4}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{5}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{6}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{7}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{8}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{9}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{10}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{11}));

  EXPECT_EQ (end, icubaby::index (begin, end, size_t{12}));
  EXPECT_EQ (end,
             icubaby::index (begin, end, std::numeric_limits<size_t>::max ()));
}

namespace {

struct JapaneseUtf8 : testing::Test {
  JapaneseUtf8 ()
      : CUs{
            // U+304A HIRAGANA LETTER O (UTF-8 E3 81 8A)
            static_cast<icubaby::char8> (0xE3),
            static_cast<icubaby::char8> (0x81),
            static_cast<icubaby::char8> (0x8A),
            // U+306F HIRAGANA LETTER HA (UTF-8 E3 81 AF)
            static_cast<icubaby::char8> (0xE3),
            static_cast<icubaby::char8> (0x81),
            static_cast<icubaby::char8> (0xAF),
            // U+3088 HIRAGANA LETTER YO (UTF-8 E3 82 88)
            static_cast<icubaby::char8> (0xE3),
            static_cast<icubaby::char8> (0x82),
            static_cast<icubaby::char8> (0x88),
            // U+3046 HIRAGANA LETTER U (UTF-8 E3 81 86)
            static_cast<icubaby::char8> (0xE3),
            static_cast<icubaby::char8> (0x81),
            static_cast<icubaby::char8> (0x86),
            // U+3054 HIRAGANA LETTER GO (UTF-8 E3 81 94)
            static_cast<icubaby::char8> (0xE3),
            static_cast<icubaby::char8> (0x81),
            static_cast<icubaby::char8> (0x94),
            // U+3056 HIRAGANA LETTER ZA (UTF-8 E3 81 96)
            static_cast<icubaby::char8> (0xE3),
            static_cast<icubaby::char8> (0x81),
            static_cast<icubaby::char8> (0x96),
            // U+3044 HIRAGANA LETTER I (UTF-8)
            static_cast<icubaby::char8> (0xE3),
            static_cast<icubaby::char8> (0x81),
            static_cast<icubaby::char8> (0x84),
            // U+307E HIRAGANA LETTER MA (UTF-8 E3 81 BE)
            static_cast<icubaby::char8> (0xE3),
            static_cast<icubaby::char8> (0x81),
            static_cast<icubaby::char8> (0xBE),
            // U+3059 HIRAGANA LETTER SU (UTF-8 E3 81 99)
            static_cast<icubaby::char8> (0xE3),
            static_cast<icubaby::char8> (0x81),
            static_cast<icubaby::char8> (0x99),
        } {}
  std::vector<icubaby::char8> const CUs;
};

}  // end anonymous namespace

TEST_F (JapaneseUtf8, LengthWithExplicitSize) {
  EXPECT_EQ (9U, icubaby::length (std::begin (CUs), std::end (CUs)));
}
TEST_F (JapaneseUtf8, IndexStdString) {
  auto begin = std::begin (CUs);
  auto end = std::end (CUs);
  auto it = begin;
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{0}));
  std::advance (it, 3);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{1}));
  std::advance (it, 3);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{2}));
  std::advance (it, 3);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{3}));
  std::advance (it, 3);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{4}));
  std::advance (it, 3);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{5}));
  std::advance (it, 3);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{6}));
  std::advance (it, 3);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{7}));
  std::advance (it, 3);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{8}));

  EXPECT_EQ (end, icubaby::index (begin, end, size_t{9}));
}

namespace {

struct ChineseCharactersUtf8 : testing::Test {
  ChineseCharactersUtf8 ()
      : CUs{
            // U+2070E CJK UNIFIED IDEOGRAPH-2070E (UTF-8 F0 A0 9C 8E)
            static_cast<icubaby::char8> (0xF0),
            static_cast<icubaby::char8> (0xA0),
            static_cast<icubaby::char8> (0x9C),
            static_cast<icubaby::char8> (0x8E),
            // U+20731 CJK UNIFIED IDEOGRAPH-20731 (UTF-8 F0 A0 9C B1)
            static_cast<icubaby::char8> (0xF0),
            static_cast<icubaby::char8> (0xA0),
            static_cast<icubaby::char8> (0x9C),
            static_cast<icubaby::char8> (0xB1),
            // U+20779 CJK UNIFIED IDEOGRAPH-20779 (UTF-8 F0 A0 9D B9)
            static_cast<icubaby::char8> (0xF0),
            static_cast<icubaby::char8> (0xA0),
            static_cast<icubaby::char8> (0x9D),
            static_cast<icubaby::char8> (0xB9),
            // U+20C53 CJK UNIFIED IDEOGRAPH-20C53 (UTF-8 F0 A0 B1 93)
            static_cast<icubaby::char8> (0xF0),
            static_cast<icubaby::char8> (0xA0),
            static_cast<icubaby::char8> (0xB1),
            static_cast<icubaby::char8> (0x93),
        } {}

  std::vector<icubaby::char8> const CUs;
};

}  // end anonymous namespace

TEST_F (ChineseCharactersUtf8, LengthWithExplicitSize) {
  EXPECT_EQ (icubaby::length (std::begin (CUs), std::end (CUs)), 4U);
}
TEST_F (ChineseCharactersUtf8, IndexStdString) {
  auto begin = std::begin (CUs);
  auto end = std::end (CUs);
  auto it = begin;
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{0}));
  std::advance (it, 4);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{1}));
  std::advance (it, 4);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{2}));
  std::advance (it, 4);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{3}));

  EXPECT_EQ (end, icubaby::index (begin, end, size_t{4}));
}
