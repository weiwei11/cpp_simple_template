#include <gtest/gtest.h>

bool TestDemo() {

    return true;
}

/** Test */

class DemoTests : public ::testing::Test
{

};

TEST_F(DemoTests, TestDemo)
{
    EXPECT_TRUE(TestDemo());
}
