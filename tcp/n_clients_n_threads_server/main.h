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
    int *socket;
};

void * clientThreadRoutine(void *args);

void killClientWithIndex(size_t index, bool safe = false, bool remove = false);

struct Client {
    int socket;
    pthread_t thread;
    Client(int socket, pthread_t thread);
};

class Clients {
 private:
    std::vector<Client> clients{};
    pthread_mutex_t readLock;
    pthread_mutex_t writeLock;

 public:
    Clients() noexcept;
    ~Clients() noexcept;
    const Client &get(size_t index);
    size_t size() noexcept;
    void add(const Client &client) noexcept;
    void remove(size_t index);
    void clear() noexcept;

 private:
    void rwLock();
    void rwUnlock();
};

void printHelp();

void printLaunchHelp();

bool parseProgramArguments(int argc, char **argv, int &port, size_t &messageLength);

#endif //NETS_MAIN_H
