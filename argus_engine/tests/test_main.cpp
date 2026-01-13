#include <gtest/gtest.h>
#include <vector>
#include <span>

TEST(EnvironmentTest, Cpp23SpanWorks) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    std::span<int> view{data};

    EXPECT_EQ(view.size(), 5);
    EXPECT_EQ(view[0], 1);
    EXPECT_EQ(view.back(), 5);
}

TEST(MathTest, BasicAddition) {
    int a = 5;
    int b = 10;
    EXPECT_EQ(a + b, 15);
}

TEST(VisionTest, BoundingBoxPlaceholder) {
    struct Box {
        int x, y, width, height;
    };

    Box b = {0, 0, 1920, 1080};
    
    EXPECT_GT(b.width, 0);
    EXPECT_GT(b.height, 0);
}