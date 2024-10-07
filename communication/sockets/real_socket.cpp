#include "real_socket.h"

logger RealSocket::log = logger("communication");

RealSocket::RealSocket(){}

int RealSocket::socket(int domain, int type, int protocol) 
{
    int sockFd = ::socket(domain, type, protocol);
    if (sockFd < 0)
    {
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "Socket creation error: " + std::string(strerror(errno)));
    }
    RealSocket::log.logMessage(logger::LogLevel::INFO, "Create a socket: " + std::to_string(sockFd));
    return sockFd;
}

int RealSocket::setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) 
{
    int sockopt = ::setsockopt(sockfd, level, optname, optval, optlen);
    if (sockopt)
    {
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "Setsockopt failed: " + std::string(strerror(errno)));
        close(sockfd);
    }

    RealSocket::log.logMessage(logger::LogLevel::INFO, "Create a server socket: " + std::to_string(sockfd));
    return sockopt;
}

int RealSocket::bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int bindAns = ::bind(sockfd, addr, addrlen);
    if (bindAns < 0)
    {
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "Bind failed: " + std::string(strerror(errno)));
        close(sockfd);
    }

    return bindAns;
}

int RealSocket::listen(int sockfd, int backlog) 
{
    int listenAns = ::listen(sockfd, backlog);
    if (listenAns < 0)
    {
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "Listen failed: " + std::string(strerror(errno)));
        close(sockfd);
    }
    return listenAns;
}

int RealSocket::accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int newSocket = ::accept(sockfd, addr, addrlen);
    if (newSocket < 0)
    {
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "Accept failed: " + std::string(strerror(errno)));
    }

    RealSocket::log.logMessage(logger::LogLevel::INFO, "Connection succeed to socket number: " + std::to_string(sockfd));
    return newSocket;
}

int RealSocket::connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int connectAns = ::connect(sockfd, addr, addrlen);
    if (connectAns < 0)
    {
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "process", "server", "Connection Failed: " + std::string(strerror(errno)));
    }

    RealSocket::log.logMessage(logger::LogLevel::INFO, "process", "server", "Connection succeed ");
    return connectAns;
}

ssize_t RealSocket::recv(int sockfd, void *buf, size_t len, int flags)
{
    int valread = ::recv(sockfd, buf, len, flags);
    const Packet *p = static_cast<const Packet *>(buf);

    if (valread < 0)
        RealSocket::log.logMessage(logger::LogLevel::ERROR, std::to_string(p->getSrcId()), std::to_string(p->getDestId()), std::string(" Error occurred: in socket ") + std::to_string(sockfd) + std::string(" ") + std::string(strerror(errno)));
    else if (valread == 0)
        RealSocket::log.logMessage(logger::LogLevel::INFO, std::to_string(p->getSrcId()), std::to_string(p->getDestId()), std::string(" connection closed: in socket ") + std::to_string(sockfd) + std::string(" ") + std::string(strerror(errno)));
    else
        RealSocket::log.logMessage(logger::LogLevel::INFO, std::to_string(p->getSrcId()), std::to_string(p->getDestId()), std::string("received packet type: ") + Message::getMessageTypeString(Message::getMessageTypeFromID(p->getId())) +std::string(" number: ") + std::to_string(p->getPSN()) + std::string(", of messageId: ") + std::to_string(p->getId()) + std::string(" ") + std::string(strerror(errno)) + ( p->getDLC() ? " Data: " + Packet::pointerToHex(p->getPayload(),p->getDLC()):""));
        
    return valread;
}

ssize_t RealSocket::send(int sockfd, const void *buf, size_t len, int flags)
{
    int sendAns = ::send(sockfd, buf, len, flags);
    const Packet *p = static_cast<const Packet *>(buf);
    if (sendAns <= 0)
        RealSocket::log.logMessage(logger::LogLevel::ERROR, std::to_string(p->getSrcId()), std::to_string(p->getDestId()), "Error occurred while sending packet type: " + Message::getMessageTypeString(Message::getMessageTypeFromID(p->getId())) + ", number: " + std::to_string(p->getPSN()) + " ,of messageId: " + std::to_string(p->getId()) + ", error: " + std::string(strerror(errno)));
    else
        RealSocket::log.logMessage(logger::LogLevel::INFO, std::to_string(p->getSrcId()), std::to_string(p->getDestId()), "sending packet type: " + Message::getMessageTypeString(Message::getMessageTypeFromID(p->getId())) + ", number: " + std::to_string(p->getPSN()) + " ,of messageId: " + std::to_string(p->getId()) + (p->getDLC() ? ", Data: " + Packet::pointerToHex(p->getPayload(), p->getDLC()) : ""));

    return sendAns;
}            
            
int RealSocket::close(int fd)
{
    RealSocket::log.logMessage(logger::LogLevel::INFO, "Close socket number: " + std::to_string(fd));
    shutdown(fd, SHUT_RDWR);
    return ::close(fd);
}