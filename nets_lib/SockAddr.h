//
// Created by anton.lamtev on 08/11/2018.
//

#ifndef NETS_SOCKADDR_H
#define NETS_SOCKADDR_H

#include <netinet/in.h>

#include <memory>
#include <string>
#include <utility>

struct SockAddr {

    sockaddr addr;
    socklen_t len;

    SockAddr(const sockaddr &addr, socklen_t len) noexcept;

    SockAddr(const sockaddr_in &addr, socklen_t len) noexcept;

    SockAddr &operator=(const SockAddr &source) noexcept;

    bool operator==(const SockAddr &source) const noexcept;

    bool operator!=(const SockAddr &source) const noexcept;

    friend struct std::hash<SockAddr>;

};

namespace std {
    template<>
    struct hash<SockAddr> {
        size_t operator()(const SockAddr &addr) const {
            size_t res = 17;
            auto p = (char *) &addr.addr;
            for (int i = 0; i < addr.len; i++) {
                res = res * 31 + hash<char>()(p[i]);
            }
            res = res * 31 + hash<socklen_t>()(addr.len);
            return res;
        }
    };
}

#endif //NETS_SOCKADDR_H
