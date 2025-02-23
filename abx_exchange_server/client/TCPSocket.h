#pragma once
#include <sys/socket.h>
#include <netinet/tcp.h>

#include <string>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>

#include "Log.h"

namespace link_type
{
    struct IPv4TCPSocket
    {
        static int createSocket() { return socket(AF_INET, SOCK_STREAM, 0); }
    };

    struct IPv6TCPSocket
    {
        static int createSocket() { return socket(AF_INET6, SOCK_STREAM, 0); }
    };

}

namespace link_type
{

    template <typename EXCHANGE>
    class TCPSocket
    {
    public:
        using LINK_TYPE = typename EXCHANGE::LinkType;
        using ADDRESS = typename EXCHANGE::Address;
        using SOCKADDR = typename ADDRESS::SockAddr;

    public:
        TCPSocket() : m_socket_fd(-1) {}
        void createSocket();
        int getFd() { return m_socket_fd; }
        void connect(const std::string &address, const int port);
        void setTCPNoDelay();
        void setTimeout(int sec, int microsec);
        template <typename MSG_TYPE>
        void write(MSG_TYPE msg);
        void close();

    private:
        void errHandler();

    private:
        int m_socket_fd;
    };

    template <typename EXCHANGE>
    void TCPSocket<EXCHANGE>::createSocket()
    {
        m_socket_fd = LINK_TYPE::createSocket();
        if (m_socket_fd < 0)
        {
            errHandler();
        }
    }

    template <typename EXCHANGE>
    void TCPSocket<EXCHANGE>::setTCPNoDelay()
    {
        int flag = 1;
        setsockopt(m_socket_fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
    }

    template <typename EXCHANGE>
    void TCPSocket<EXCHANGE>::setTimeout(int sec, int microsec)
    {
        struct timeval timeout;
        timeout.tv_sec = sec;
        timeout.tv_usec = microsec;

        if (setsockopt(m_socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
        {
            close();
            errHandler();
        }
    }

    template <typename EXCHANGE>
    void TCPSocket<EXCHANGE>::connect(const std::string &address, int port)
    {
        SOCKADDR kernel_socket_address = ADDRESS::kernelAddressStruct(address, port);
        LOG(DEBUG, m_socket_fd);

        int response = ::connect(m_socket_fd, reinterpret_cast<sockaddr *>(&kernel_socket_address), static_cast<socklen_t>(sizeof(kernel_socket_address)));

        if (response < 0)
        {
            errHandler();
        }
        else if (response == 0)
        {
            LOG(INFO, "connected to server");
        }
    }

    template <typename EXCHANGE>
    void TCPSocket<EXCHANGE>::errHandler()
    {
        LOG(ERROR, "something went wrong, ", strerror(errno));
        exit(1);
    }

    template <typename EXCHANGE>
    template <typename MSG_TYPE>
    void TCPSocket<EXCHANGE>::write(MSG_TYPE msg)
    {
        int total_bytes_written = 0;
        while (total_bytes_written < sizeof(MSG_TYPE))
        {
            auto bytes_written = ::write(m_socket_fd, &msg, sizeof(MSG_TYPE));
            fsync(m_socket_fd);
            if (bytes_written < 0)
            {
                errHandler();
            }

            LOG(DEBUG, bytes_written, " bytes written on wire ");

            total_bytes_written += bytes_written;
        }
    }

    template <typename EXCHANGE>
    void TCPSocket<EXCHANGE>::close()
    {
        if (::close(m_socket_fd) < 0)
        {
            errHandler();
        }
    }

}