//
// Created by anton.lamtev on 08/11/2018.
//

#include "SockAddr.h"


SockAddr::SockAddr(const sockaddr &addr, socklen_t len) noexcept :
        addr(addr),
        len(len) {}

SockAddr::SockAddr(const sockaddr_in &addr, socklen_t len) noexcept :
        addr(*(sockaddr *) &addr),
        len(len) {}

SockAddr &SockAddr::operator=(const SockAddr &source) noexcept {
    std::memcpy(&addr, &source.addr, source.len);
    len = source.len;
    return *this;
}

bool SockAddr::operator==(const SockAddr &source) const noexcept {
    return (len == source.len && memcmp(&addr, &source.addr, len) == 0);
}

bool SockAddr::operator!=(const SockAddr &source) const noexcept {
    return !this->operator==(source);
}
