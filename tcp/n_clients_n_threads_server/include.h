//
// Created by anton.lamtev on 24.09.2018.
//

#ifndef NETS_MAIN_H
#define NETS_MAIN_H

#include <sys/socket.h>
#include <pthread.h>

#include <vector>

struct AcceptThreadArgs {
    int listeningSocket;
};

void *acceptThreadRoutine(void *args);

struct ClientThreadArgs {
    int socket;
};

void *clientThreadRoutine(void *args);

void killClientWithIndex(size_t index, bool safe);

struct Client {
    int socket;
    pthread_t thread;
    Client(int socket, pthread_t thread);
};

void initRwLock();

void deinitRwLock();

void printHelp();

void printLaunchHelp();

bool parseProgramArguments(int argc, char **argv, int &port, size_t &messageLength);

#endif //NETS_MAIN_H
