//
// Created by vad1mchk on 17/11/24.
//
#include <gtest/gtest.h>

#include "monolith/shell/util/string_functions.hpp"

class StringFunctionsTest : public ::testing::Test {};

TEST_F(StringFunctionsTest, StringShouldBeSplitAndJoinedCorrectly) {
  ASSERT_EQ(
    ShortenEachPart(
      "java.util.concurrent.atomic.AtomicInteger", '.', 1, false
      ),
    "j.u.c.a.AtomicInteger"
  );
  ASSERT_EQ(
    ShortenEachPart(
      "java.util.concurrent.atomic.AtomicInteger", '.', 1, true
      ),
    "j.u.c.a.A"
  );
  ASSERT_EQ(
    ShortenEachPart(
      "java.util.concurrent.atomic.AtomicInteger", '.', 4, false
      ),
    "java.util.conc.atom.AtomicInteger"
  );
}

TEST_F(StringFunctionsTest, StringShouldBeSplitAndJoinedCorrectlyWithLeadingOrTrailingDelimiter) {
  ASSERT_EQ(
    ShortenEachPart(
      "./janedoe/Documents/testfiles/", '/', 1, false
      ),
    "./j/D/testfiles/"
  );

  ASSERT_EQ(
    ShortenEachPart(
      "/usr/bin/bash", '/', 1, false
      ),
    "/u/b/bash"
  );

  ASSERT_EQ(
    ShortenEachPart(
      "/home/johndoe/Documents/testfiles/", '/', 1, false
      ),
    "/h/j/D/testfiles/"
  );
}