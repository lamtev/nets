//
// Created by anton.lamtev on 01.10.2018.
//

#ifndef NETS_SERVERNETDELEGATE_H
#define NETS_SERVERNETDELEGATE_H


#include "ServerNetError.h"

class ServerNet;


class ServerNetDelegate {
 public:
    virtual void netDidFailWithError(ServerNet *net, ServerNetError error) = 0;
};


#endif //NETS_SERVERNETDELEGATE_H
