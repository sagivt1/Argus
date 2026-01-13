#include <gtest/gtest.h>
#include <vector>
#include <span>

// Validates compiler environment supports necessary C++20/23 features (std::span).
TEST(EnvironmentTest, Cpp23SpanWorks) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    // Verify non-owning view over contiguous memory.
    std::span<int> view{data};

    EXPECT_EQ(view.size(), 5);
    EXPECT_EQ(view[0], 1);
    EXPECT_EQ(view.back(), 5);
}

// Sanity check: ensures GTest framework is correctly linked and executable.
TEST(MathTest, BasicAddition) {
    int a = 5;
    int b = 10;
    EXPECT_EQ(a + b, 15);
}

// Placeholder to verify test harness logic before integrating real Vision module types.
TEST(VisionTest, BoundingBoxPlaceholder) {
    // Local mock to decouple test harness from Vision library dependencies.
    struct Box {
        int x, y, width, height;
    };

    // Simulate standard 1080p frame dimensions.
    Box b = {0, 0, 1920, 1080};
    
    EXPECT_GT(b.width, 0);
    EXPECT_GT(b.height, 0);
}