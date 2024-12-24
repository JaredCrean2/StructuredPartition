#include "gtest/gtest.h"
#include "init.h"

TEST(Init, Return42)
{
  EXPECT_EQ(return42(), 42);
}
