#include "pch.h"


#include "../RM/rule_machine.h"


TEST(TestCaseName, TestName) 
{
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(false);

  ::testing::StaticAssertTypeEq<int, int>();
}



