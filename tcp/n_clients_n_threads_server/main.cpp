#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <iostream>
#include <vector>

#include <nets_lib/receivenbytes.h>

#include "TCPClient.h"

constexpr int DEFAULT_PORT = 7500;
size_t messageLength;

std::vector<Client> clients{};

//TODO: looks like we don't need mutex because no problems can occur
pthread_rwlock_t rwLock;

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

    int enable = 1;
    setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, (void *) &enable, sizeof(int));

#ifdef __APPLE__
    setsockopt(listeningSocket, SOL_SOCKET, SO_NOSIGPIPE, (void *) &enable, sizeof(int));
#endif

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

    initRwLock();

    pthread_t acceptThread;

    AcceptThreadArgs acceptThreadArgs{};
    acceptThreadArgs.listeningSocket = listeningSocket;
    pthread_create(&acceptThread, nullptr, acceptThreadRoutine, &acceptThreadArgs);

    while (true) {
        std::string instruction;
        std::getline(std::cin, instruction);

        if (instruction == "exit") {
            break;
        } else if (instruction == "list") {
            pthread_rwlock_rdlock(&rwLock);
            for (size_t i = 0; i < clients.size(); ++i) {
                int socket = clients[i].socket;
                sockaddr_in addr{};
                getpeername(socket, (sockaddr *) &addr, nullptr);
                char ipStr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(addr.sin_addr), ipStr, INET_ADDRSTRLEN);
                std::cout << i + 1 << ".\t" << ipStr << ":" << addr.sin_port << std::endl;
            }
            pthread_rwlock_unlock(&rwLock);
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
                pthread_rwlock_wrlock(&rwLock);
                killClientWithIndex(clientIndex - 1, true);
                clients.erase(clients.cbegin() + clientIndex - 1);
                pthread_rwlock_unlock(&rwLock);
            }
        }
    }

    shutdown(listeningSocket, SHUT_RDWR);
    close(listeningSocket);
    pthread_join(acceptThread, nullptr);

    deinitRwLock();

    return 0;
}


void *acceptThreadRoutine(void *args) {
    auto acceptThreadArgs = reinterpret_cast<AcceptThreadArgs *>(args);

    while (true) {
        int acceptedSocket = accept(acceptThreadArgs->listeningSocket, nullptr, nullptr);

        if (acceptedSocket == -1) {
            std::cerr << "Unable to accept: " << strerror(errno) << std::endl;
            break;
        }
        std::cout << "Accepted" << std::endl;

        pthread_t clientThread = nullptr;
        pthread_attr_t clientThreadAttr;
        pthread_attr_init(&clientThreadAttr);

        pthread_rwlock_wrlock(&rwLock);
        clients.emplace_back(acceptedSocket, clientThread);
        pthread_rwlock_unlock(&rwLock);

        auto *clientThreadArgs = new ClientThreadArgs{};
        clientThreadArgs->socket = acceptedSocket;
        pthread_create(&clientThread, &clientThreadAttr, clientThreadRoutine, reinterpret_cast<void *>(clientThreadArgs));
    }

    pthread_rwlock_wrlock(&rwLock);
    for (size_t i = 0; i < clients.size(); ++i) {
        killClientWithIndex(i, false);
    }
    clients.clear();
    pthread_rwlock_unlock(&rwLock);

    return nullptr;
}

void *clientThreadRoutine(void *args) {
    auto clientThreadArgs = reinterpret_cast<ClientThreadArgs *>(args);

    while (true) {
        char buf[messageLength];
        ssize_t bytesReceived = receiveNBytes(clientThreadArgs->socket, messageLength, buf);
        if (bytesReceived == -1 || bytesReceived == 0) {
            std::cerr << "Unable to receive: " << strerror(errno) << std::endl;
            break;
        }
        std::cout << "Received bytes: " << buf << std::endl;

#ifdef __APPLE__
        ssize_t bytesSent = send(clientThreadArgs->socket, buf, strlen(buf), 0);
#else
        ssize_t bytesSent = send(clientThreadArgs->_socket, buf, strlen(buf), MSG_NOSIGNAL);
#endif
        if (bytesSent == -1) {
            std::cerr << "Unable to send: " << strerror(errno) << std::endl;
        }
    }

    delete clientThreadArgs;

    return nullptr;
}

void killClientWithIndex(size_t index, bool safe) {
    if (safe && clients.size() <= index) {
        return;
    }
    const auto &client = clients[index];
    shutdown(client.socket, SHUT_RDWR);
    close(client.socket);

    pthread_join(client.thread, nullptr);
}


Client::Client(int socket, pthread_t thread) : socket{socket}, thread{thread} {}

void initRwLock() {
    pthread_rwlock_init(&rwLock, nullptr);
}

void deinitRwLock() {
    pthread_rwlock_destroy(&rwLock);
}


static const char *helpMessage = R"(
Usage:

exit            close all sockets, terminate threads and exit
list            print udp list
kill <idx>      kill idx-th udp
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


bool parseProgramArguments(int argc, char **argv, int &port, size_t &messageLength) {
    if (argc == 1) {
        port = DEFAULT_PORT;
        messageLength = 64;
        return true;
    } else if (argc > 5 || argc == 0) {
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
