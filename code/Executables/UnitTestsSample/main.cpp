#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
	// Expect two strings not to be equal.
	EXPECT_STRNE("hello", "world");
	// Expect equality.
	EXPECT_EQ(7 * 6, 42);
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	int unitTestsRes = testing::UnitTest::GetInstance()->Run();
	if (unitTestsRes != 0)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}
