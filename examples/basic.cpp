#include <op.hpp>

int main(int argc, char* argv[])
{
    auto s = op::option("-s", "--string")
        .help("string value");
    auto i = op::option<int>("-i", "--integer")
        .help("integer value");

    op::parse(argc, argv);
}
