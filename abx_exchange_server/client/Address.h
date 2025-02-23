#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>

namespace address
{

    struct IPv4Address
    {
        using SockAddr = sockaddr_in;
        static SockAddr kernelAddressStruct(const std::string &address, int port)
        {
            SockAddr sock_addr{};
            sock_addr.sin_family = AF_INET;
            sock_addr.sin_port = htons(port);

            if (inet_pton(AF_INET, address.c_str(), &sock_addr.sin_addr) <= 0)
            {
                perror("Invalid IP address");
                exit(EXIT_FAILURE);
            }

            return sock_addr;
        }
    };

}