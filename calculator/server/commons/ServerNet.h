//
// Created by anton.lamtev on 11/11/2018.
//

#ifndef NETS_SERVERNET_H
#define NETS_SERVERNET_H

class ServerNetDelegate;


class ServerNet {
public:
    virtual void setDelegate(ServerNetDelegate *delegate) = 0;

    virtual void start() = 0;

    virtual void stop() = 0;
};

#endif //NETS_SERVERNET_H
