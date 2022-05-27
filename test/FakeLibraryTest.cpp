//
// Created by TSprech on 5/27/2022.
//

#include "gtest/gtest.h"
#include "FakeLibrary.hpp"

TEST(Fake, Multiply) {
  EXPECT_EQ(mult(3, 9), 27);
  EXPECT_EQ(mult(2, 4), 8);
}

TEST(Fake, MultiplyBy2) {
  EXPECT_EQ(mult2(16), 32);
}
