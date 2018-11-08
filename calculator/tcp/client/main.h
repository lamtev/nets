//
// Created by anton.lamtev on 05.10.2018.
//

#ifndef NETS_MAIN_H
#define NETS_MAIN_H

#include <string>

class Message;

Message *requestWithInstruction(const std::string &instruction);

void printHelp();

constexpr static const char *help = R"(
Arguments:

<ip> <port>
)";

#endif //NETS_MAIN_H
