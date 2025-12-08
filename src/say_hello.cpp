#include <fmt/base.h>

#include <hpc-utils/say_hello.hpp>

void say_hello(const std::string& name) {
    fmt::println("hello {}", name);
}
