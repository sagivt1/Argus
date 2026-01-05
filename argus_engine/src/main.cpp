#include <vector>
#include <span>
#include <print>


int main() {
    std::vector data = {1, 2, 3, 4, 5};

    std::span view{data};

    std::println("Argus Engine Online.");
    std::println("System Check: C++23 Span Size is {}", view.size());

    return 0;
}

