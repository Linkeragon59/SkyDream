#include <gtest/gtest.h>
#include <fmt/color.h>
#include <spdlog/spdlog.h>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
	// Expect two strings not to be equal.
	EXPECT_STRNE("hello", "world");
	// Expect equality.
	EXPECT_EQ(7 * 6, 42);
}

int main(int argc, char* argv[])
{
	fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold,
				"Hello, {}!\n", "world");
	fmt::print(fg(fmt::color::floral_white) | bg(fmt::color::slate_gray) |
				fmt::emphasis::underline, "Olá, {}!\n", "Mundo");
	fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
				"你好{}！\n", "世界");

	spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);
    
    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Positional args are {1} {0}..", "too", "supported");
    spdlog::info("{:<30}", "left aligned");
    
    spdlog::set_level(spdlog::level::debug); // Set *global* log level to debug
    spdlog::debug("This message should be displayed..");    
    
    // change log pattern
    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
    
    // Compile time log levels
    // Note that this does not change the current log level, it will only
    // remove (depending on SPDLOG_ACTIVE_LEVEL) the call on the release code.
    SPDLOG_TRACE("Some trace message with param {}", 42);
    SPDLOG_DEBUG("Some debug message");

	testing::InitGoogleTest(&argc, argv);
	int unitTestsRes = testing::UnitTest::GetInstance()->Run();
	if (unitTestsRes != 0)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}
