#pragma once

#include "Log.h"
#include <cstring>
#include <algorithm>

namespace processor
{

    template <typename ABX_EXCHANGE>
    class ABXProcessor
    {
    public:
        using SOCKET = typename ABX_EXCHANGE::Socket;
        using RESPONSE_MSG = typename ABX_EXCHANGE::ResponseMsg;
        using DISPATCHER = typename ABX_EXCHANGE::Dispatcher;
        using COMMON_BUFFER = typename ABX_EXCHANGE::SingletonBuffer;
        using MKT_CONN = typename ABX_EXCHANGE::MarketConnection;

    public:
        ABXProcessor() : m_link() {}
        void init(SOCKET sock)
        {
            m_link = sock;
        }

        size_t intended_response_size() { return 1024; }

        void onRead(char *buffer, int buf_size)
        {
            RESPONSE_MSG response;

            // copy the first 4 bytes as-is (ASCII Symbol)
            std::memcpy(response.symbol, buffer, 4);

            // Copy Buy/Sell Indicator (ASCII char)
            response.side = buffer[4];

            // Extract int32_t fields (Big Endian -> Host Byte Order)
            std::memcpy(&response.quantity, buffer + 5, 4);
            response.quantity = ntohl(response.quantity);

            std::memcpy(&response.price, buffer + 9, 4);
            response.price = ntohl(response.price);

            std::memcpy(&response.seq_num, buffer + 13, 4);
            response.seq_num = ntohl(response.seq_num);

            MKT_CONN::newSymbol(response);
            COMMON_BUFFER::instance().push_back(response);
        }

    private:
        SOCKET m_link;
    };

}