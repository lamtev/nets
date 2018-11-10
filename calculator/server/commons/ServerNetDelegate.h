//
// Created by anton.lamtev on 01.10.2018.
//

#ifndef NETS_SERVERNETDELEGATE_H
#define NETS_SERVERNETDELEGATE_H


#include "ServerNetError.h"

class TCPServerNet;


class ServerNetDelegate {
public:
    virtual void netDidFailWithError(TCPServerNet *net, ServerNetError error) = 0;
};


#endif //NETS_SERVERNETDELEGATE_H
