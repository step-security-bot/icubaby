#include <gmock/gmock.h>

#include <array>
#include <iterator>
#include <vector>

#include "icubaby/icubaby.hpp"

static_assert (std::is_same_v<icubaby::t8_8 ::input_type, char8_t> &&
               std::is_same_v<icubaby::t8_8 ::output_type, char8_t>);
static_assert (std::is_same_v<icubaby::t8_16::input_type, char8_t> &&
               std::is_same_v<icubaby::t8_16::output_type, char16_t>);
static_assert (std::is_same_v<icubaby::t8_32::input_type, char8_t> &&
               std::is_same_v<icubaby::t8_32::output_type, char32_t>);

using testing::ElementsAre;

// | UTF-8 Sequence         | Code point                   |
// | ---------------------- + ---------------------------- +
// | 0x24,                  | U+0024  DOLLAR SIGN          |
// | 0xC2, 0xA2,            | U+00A2  CENT SIGN            |
// | 0xE0, 0xA4, 0xB9,      | U+0939  DEVANAGARI LETTER HA |
// | 0xE2, 0x82, 0xAC,      | U+20AC  EURO SIGN            |
// | 0xED, 0x95, 0x9C,      | U+D55C  HANGUL SYLLABLE HAN  |
// | 0xF0, 0x90, 0x8D, 0x88 | U+10348 GOTHIC LETTER HWAIR  |
TEST (Utf8, Good) {
  icubaby::t8_32 d;
  EXPECT_TRUE (d.good ());

  auto const matcher1 = ElementsAre (char32_t{0x0024});
  auto const matcher2 = ElementsAre (char32_t{0x0024}, char32_t{0x00A2});
  auto const matcher3 =
      ElementsAre (char32_t{0x0024}, char32_t{0x00A2}, char32_t{0x0939});
  auto const matcher4 = ElementsAre (char32_t{0x0024}, char32_t{0x00A2},
                                     char32_t{0x0939}, char32_t{0x20AC});
  auto const matcher5 =
      ElementsAre (char32_t{0x0024}, char32_t{0x00A2}, char32_t{0x0939},
                   char32_t{0x20AC}, char32_t{0xD55C});
  auto const matcher6 =
      ElementsAre (char32_t{0x0024}, char32_t{0x00A2}, char32_t{0x0939},
                   char32_t{0x20AC}, char32_t{0xD55C}, char32_t{0x10348});

  std::vector<char32_t> cu;
  auto out = std::back_inserter (cu);
  out = d (0x24, out);
  EXPECT_TRUE (d.good ());
  EXPECT_THAT (cu, matcher1);

  {
    // 0xC2, 0xA2 => U+00A2  CENT SIGN
    std::array<char8_t, 2> const cent_sign = {{0xC2, 0xA2}};
    out = d (cent_sign[0], out);
    EXPECT_TRUE (d.good ());
    EXPECT_THAT (cu, matcher1);
    out = d (cent_sign[1], out);
    EXPECT_TRUE (d.good ());
    EXPECT_THAT (cu, matcher2);
  }
  {
    // 0xE0, 0xA4, 0xB9 => U+0939 DEVANAGARI LETTER HA
    std::array<char8_t, 3> const devanagri_letter_ha{{0xE0, 0xA4, 0xB9}};
    out = d (devanagri_letter_ha[0], out);
    EXPECT_TRUE (d.good ());
    EXPECT_THAT (cu, matcher2);
    out = d (devanagri_letter_ha[1], out);
    EXPECT_TRUE (d.good ());
    EXPECT_THAT (cu, matcher2);
    out = d (devanagri_letter_ha[2], out);
    EXPECT_TRUE (d.good ());
    EXPECT_THAT (cu, matcher3);
  }
  {
    // 0xE2, 0x82, 0xAC => U+20AC EURO SIGN
    std::array<char8_t, 3> const euro_sign{{0xE2, 0x82, 0xAC}};
    out = d (euro_sign[0], out);
    EXPECT_TRUE (d.good ());
    EXPECT_THAT (cu, matcher3);
    out = d (euro_sign[1], out);
    EXPECT_TRUE (d.good ());
    EXPECT_THAT (cu, matcher3);
    out = d (euro_sign[2], out);
    EXPECT_TRUE (d.good ());
    EXPECT_THAT (cu, matcher4);
  }
  {
    // 0xED, 0x95, 0x9C,      | U+D55C  HANGUL SYLLABLE HAN
    std::array<char8_t, 3> const hangul_syllable_han{{0xED, 0x95, 0x9C}};
    out = d (hangul_syllable_han[0], out);
    EXPECT_TRUE (d.good ());
    EXPECT_THAT (cu, matcher4);
    out = d (hangul_syllable_han[1], out);
    EXPECT_TRUE (d.good ());
    EXPECT_THAT (cu, matcher4);
    out = d (hangul_syllable_han[2], out);
    EXPECT_TRUE (d.good ());
    EXPECT_THAT (cu, matcher5);
  }
  {
    // 0xF0, 0x90, 0x8D, 0x88 | U+10348 GOTHIC LETTER HWAIR
    std::array<char8_t, 4> const gothic_letter_hwair{{0xF0, 0x90, 0x8D, 0x88}};
    out = d (gothic_letter_hwair[0], out);
    EXPECT_TRUE (d.good ());
    EXPECT_THAT (cu, matcher5);
    out = d (gothic_letter_hwair[1], out);
    EXPECT_TRUE (d.good ());
    EXPECT_THAT (cu, matcher5);
    out = d (gothic_letter_hwair[2], out);
    EXPECT_TRUE (d.good ());
    EXPECT_THAT (cu, matcher5);
    out = d (gothic_letter_hwair[3], out);
    EXPECT_TRUE (d.good ());
    EXPECT_THAT (cu, matcher6);
  }

  EXPECT_TRUE (d.finalize ());
  EXPECT_TRUE (d.good ());
}

TEST (Utf8, Bad1) {
  icubaby::t8_32 d2;
  std::vector<char32_t> out;
  auto it = std::back_inserter (out);
  it = d2 (char8_t{0x80}, it);
  EXPECT_THAT (out, ElementsAre (icubaby::replacement_char));
  EXPECT_FALSE (d2.good ());
  it = d2 (char8_t{0x24}, it);
  EXPECT_THAT (out, ElementsAre (icubaby::replacement_char, char32_t{0x24}));
  EXPECT_FALSE (d2.finalize ());
  EXPECT_FALSE (d2.good ());
}

TEST (Utf8, Bad2) {
  icubaby::t8_32 d2;
  std::vector<char32_t> out;
  auto it = std::back_inserter (out);
  it = d2 (char8_t{0x80}, it);
  EXPECT_FALSE (d2.good ());
  EXPECT_THAT (out, ElementsAre (icubaby::replacement_char));
  EXPECT_FALSE (d2.finalize ());
  EXPECT_FALSE (d2.good ());
}