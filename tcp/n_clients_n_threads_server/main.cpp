#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <iostream>
#include <vector>

#include <nets_lib/receivenbytes.h>

#include "main.h"

Clients clients{};

constexpr int DEFAULT_PORT = 7500;
size_t messageLength;

int main(int argc, char **argv) {
    int port;

    if (!parseProgramArguments(argc, argv, port, messageLength)) {
        std::cout << "Invalid program arguments" << std::endl;
        printLaunchHelp();
        return -1;
    }

    int listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket < 0) {
        std::cerr << "Socket error: " << strerror(errno) << std::endl;
        return 1;
    }

    sockaddr_in localAddress{};
    localAddress.sin_family = AF_INET;
    localAddress.sin_port = htons(port);
    localAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    int error = bind(listeningSocket, (struct sockaddr *) &localAddress, sizeof(localAddress));
    if (error != 0) {
        std::cerr << "Unable to bind: " << strerror(errno) << std::endl;
        return 1;
    }

    error = listen(listeningSocket, 5);
    if (error != 0) {
        std::cerr << "Unable to listen: " << strerror(errno) << std::endl;
        return 1;
    }

    pthread_t acceptThread;

    AcceptThreadArgs acceptThreadArgs{};
    acceptThreadArgs.listeningSocket = listeningSocket;
    pthread_create(&acceptThread, nullptr, acceptThreadRoutine, &acceptThreadArgs);

    while (true) {
        std::string instruction;
        std::getline(std::cin, instruction);

        if (instruction == "exit") {
            shutdown(listeningSocket, SHUT_RDWR);
            close(listeningSocket);
            pthread_join(acceptThread, nullptr);
            break;
        } else if (instruction == "list") {
            for (size_t i = 0; i < clients.size(); ++i) {
                int socket = clients.get(i).socket;
                sockaddr_in addr{};
                getpeername(socket, (sockaddr *) &addr, nullptr);
                char ipStr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(addr.sin_addr), ipStr, INET_ADDRSTRLEN);
                std::cout << i + 1 << ".\t" << ipStr << ":" << addr.sin_port << std::endl;
            }
        } else {
            auto idx = instruction.find_last_of("kill");
            if (idx != std::string::npos) {
                size_t clientIndex;
                try {
                    clientIndex = static_cast<size_t>(std::stoul(instruction.substr(idx + 1)));
                } catch (const std::invalid_argument &ignored) {
                    printHelp();
                    continue;
                }
                killClientWithIndex(clientIndex - 1, true, true);
            }
        }
    }

    return 0;
}

#pragma mark - Thread routines

void *acceptThreadRoutine(void *args) {
    auto acceptThreadArgs = reinterpret_cast<AcceptThreadArgs *>(args);

    while (true) {
        int *acceptedSocket = new int{};
        *acceptedSocket = accept(acceptThreadArgs->listeningSocket, nullptr, nullptr);

        if (*acceptedSocket == -1) {
            std::cerr << "Unable to accept: " << strerror(errno) << std::endl;
            break;
        } else {
            std::cout << "Accepted" << std::endl;
        }

        pthread_t clientThread = nullptr;
        pthread_attr_t clientThreadAttr;
        pthread_attr_init(&clientThreadAttr);

        clients.add(Client{*acceptedSocket, clientThread});

        auto *clientThreadArgs = new ClientThreadArgs{};
        clientThreadArgs->socket = acceptedSocket;
        pthread_create(&clientThread, &clientThreadAttr, clientThreadRoutine, reinterpret_cast<void *>(clientThreadArgs));
    }

    for (size_t i = 0; i < clients.size(); ++i) {
        killClientWithIndex(i);
    }
    clients.clear();

    return nullptr;
}

void *clientThreadRoutine(void *args) {
    auto clientThreadArgs = reinterpret_cast<ClientThreadArgs *>(args);

    while (*clientThreadArgs->socket != -1) {
        char buf[messageLength];
        ssize_t bytesReceived = receiveNBytes(*clientThreadArgs->socket, messageLength, buf);
        if (bytesReceived == -1 || bytesReceived == 0) {
            std::cerr << "Unable to receive: " << strerror(errno) << std::endl;
            break;
        }
        std::cout << "Received bytes: " << buf << std::endl;

        ssize_t bytesSent = send(*clientThreadArgs->socket, buf, strlen(buf), 0);
        if (bytesSent == -1) {
            std::cerr << "Unable to send: " << strerror(errno) << std::endl;
        }
    }

    delete clientThreadArgs->socket;
    delete clientThreadArgs;

    return nullptr;
}

void killClientWithIndex(size_t index, bool safe, bool remove) {
    if (safe && clients.size() <= index) {
        return;
    }
    const auto &client = clients.get(index);
    shutdown(client.socket, SHUT_RDWR);
    close(client.socket);

    pthread_join(client.thread, nullptr);

    if (remove) {
        clients.remove(index);
    }
}

#pragma mark - Client inplementation

Client::Client(int socket, pthread_t thread) : socket{socket}, thread{thread} {}

#pragma mark - Clients implementation

Clients::Clients() noexcept {
    pthread_mutex_init(&readLock, nullptr);
    pthread_mutex_init(&writeLock, nullptr);
}

Clients::~Clients() noexcept {
    pthread_mutex_destroy(&readLock);
    pthread_mutex_destroy(&writeLock);
}

const Client &Clients::get(size_t index) {
    pthread_mutex_lock(&writeLock);
    const auto &cl = clients[index];
    pthread_mutex_unlock(&writeLock);

    return cl;
}

size_t Clients::size() noexcept {
    pthread_mutex_lock(&writeLock);
    auto sz = clients.size();
    pthread_mutex_unlock(&writeLock);

    return sz;
}

void Clients::add(const Client &client) noexcept {
    rwLock();
    clients.push_back(client);
    rwUnlock();
}

void Clients::remove(size_t index) {
    rwLock();
    clients.erase(clients.cbegin() + index);
    rwUnlock();
}

void Clients::clear() noexcept {
    rwLock();
    clients.clear();
    rwUnlock();
}

void Clients::rwLock() {
    pthread_mutex_lock(&readLock);
    pthread_mutex_lock(&writeLock);
}

void Clients::rwUnlock() {
    pthread_mutex_unlock(&readLock);
    pthread_mutex_unlock(&writeLock);
}


#pragma mark - Help

static const char *helpMessage = R"(
Usage:

exit            close all sockets, terminate threads and exit
list            print client list
kill <idx>      kill idx-th client
)";

void printHelp() {
    std::cout << helpMessage << std::endl;
}

static const char *launchHelpMessage = R"(
Arguments:

No arguments:                       launch with default args
-p <PORT> -ml <MESSAGE_LENGTH>      launch with specified port and message length
)";

void printLaunchHelp() {
    std::cout << launchHelpMessage << std::endl;
}

#pragma mark - Program arguments parsing

bool parseProgramArguments(int argc, char **argv, int &port, size_t &messageLength) {
    if (argc == 1) {
        port = DEFAULT_PORT;
        messageLength = 64;
        return true;
    } else if (argc > 5) {
        return false;
    } else {
        std::string argv1 = argv[1];
        std::string argv3 = argv[3];
        std::string p;
        std::string ml;
        if (argv1 == "-p" && argv3 == "-ml") {
            p = argv[2];
            ml = argv[4];
        } else if (argv3 == "-p" && argv1 == "-ml") {
            p = argv[4];
            ml = argv[2];
        } else {
            return false;
        }
        try {
            port = std::stoi(p);
            messageLength = std::stoul(ml);
            return true;
        } catch (const std::invalid_argument &ignored) {
            return false;
        }
    }
}
