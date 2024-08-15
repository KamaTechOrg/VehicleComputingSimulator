#ifndef REALSOCKET_H
#define REALSOCKET_H

#include "Isocket.h"
#include <unistd.h>
#include <arpa/inet.h> // For inet_pton

class RealSocket : public ISocket {
public:
    ~RealSocket() override = default;
    int socket(int domain, int type, int protocol) override {
        return ::socket(domain, type, protocol);
    }
    int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) override {
        return ::setsockopt(sockfd, level, optname, optval, optlen);
    }
    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) override {
        return ::bind(sockfd, addr, addrlen);
    }
    int listen(int sockfd, int backlog) override {
        return ::listen(sockfd, backlog);
    }
    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) override {
        return ::accept(sockfd, addr, addrlen);
    }
    int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) override {
        return ::connect(sockfd, addr, addrlen);
    }
    ssize_t recv(int sockfd, void *buf, size_t len, int flags) override {
        return ::recv(sockfd, buf, len, flags);
    }
    ssize_t send(int sockfd, const void *buf, size_t len, int flags) override {
        return ::send(sockfd, buf, len, flags);
    }
    int close(int fd) override {
        return ::close(fd);
    }
    // int inet_pton(int af, const char *src, void *dst) override {
    //     return ::inet_pton(af, src, dst);
    // }
};

#endif // REALSOCKET_H