#include <op.hpp>

int main(int argc, char* argv[])
{
    auto s = op::argument("-s", "--string")
        .help("string value");
    auto i = op::argument<int>("-i", "--integer")
        .help("integer value");

    op::parse(argc, argv);
}
