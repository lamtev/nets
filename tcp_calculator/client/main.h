//
// Created by anton.lamtev on 05.10.2018.
//

#ifndef NETS_MAIN_H
#define NETS_MAIN_H

#include <string>

#include <protocol/Message.h>
#include <protocol/Operation.h>


Message *messageWithInstruction(const std::string &instruction);

#endif //NETS_MAIN_H
