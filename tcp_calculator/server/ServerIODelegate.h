//
// Created by anton.lamtev on 01.10.2018.
//

#ifndef NETS_SERVERIODELEGATE_H
#define NETS_SERVERIODELEGATE_H

#include <cstdint>
#include <vector>

class ServerIO;
class ClientSession;


class ServerIODelegate {
 public:
    virtual void ioWantsToKillClientWithId(ServerIO *io, uint64_t id) = 0;
    virtual std::vector<ClientSession> ioWantsToListClients(ServerIO *io) = 0;
    virtual void ioWantsToExit(ServerIO *io) = 0;
};


#endif //NETS_SERVERIODELEGATE_H
