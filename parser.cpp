#include <op/parser.hpp>

#include <cassert>
#include <memory>

namespace op {

Parser globalParser;

void Parser::parse(int argc, char* argv[])
{
    // TODO: set program name?
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    }
    parseImpl(args, false);
}

void Parser::parseImpl(
    const std::vector<std::string>& args, bool stopAtNonOption)
{
    std::vector<std::string> leftovers;
    std::vector<std::unique_ptr<Error>> errors;

    for (auto arg = args.begin(); arg != args.end(); ++arg) {
        assert(!arg->empty());

        if (arg->length() > 1 && arg->at(0) == '-' && arg->at(1) != '-') {
            for (size_t i = 1; i < arg->length(); i++) {




                if (auto keyOp = _ops.find(std::string{arg->at(i)});
                        keyOp != _ops.end()) {
                    auto& op = keyOp->second;

                    if (op->isFlag) {
                        op->raise();
                    } else {
                        if (i + 1 == arg->length()) {
                            ++arg;
                            if (arg == args.end()) {
                                errors.push_back(
                                    std::make_unique<MissingArgument>(
                                        std::string{"-"} + arg->at(i)));
                            } else {
                                op->set(*arg);
                            }
                        } else {
                            op->set(arg->substr(i + 1));
                        }
                        break;
                    }
                } else {
                    errors.push_back(
                        std::make_unique<UnknownOption>(
                            std::string{"-"} + arg->at(i)));
                }
            }
        } else if (
                arg->length() > 2 && arg->at(0) == '-' && arg->at(1) == '-') {
            if (auto keyOp = _ops.find(arg->substr(2)); keyOp != _ops.end()) {
                auto& op = keyOp->second;

                ++arg;
                if (arg == args.end()) {
                    errors.push_back(std::make_unique<MissingArgument>(*arg));
                } else {
                    op->set(*arg);
                }
            } else {
                errors.push_back(std::make_unique<UnknownOption>(*arg));
            }
        } else if (*arg == "--") {
            std::copy(++arg, args.end(), std::back_inserter(leftovers));
            return;
        } else {
            if (stopAtNonOption) {
                std::copy(arg, args.end(), std::back_inserter(leftovers));
            } else {
                leftovers.push_back(*arg);
            }
        }
    }
}

void Parser::printHelp(std::ostream& output) const
{
    for (const auto& info : _optionInfo) {
        output << "  ";
        if (auto it = info.keys.begin(); it != info.keys.end()) {
            output << *it++;
            for (; it != info.keys.end(); ++it) {
                output << ", " << *it;
            }
        }
        if (info.requiresArgument) {
            output << "=";
        }
        output << info.metavar;
        output << " " << info.help;
        output << "\n";
    }
}

} // namespace op
