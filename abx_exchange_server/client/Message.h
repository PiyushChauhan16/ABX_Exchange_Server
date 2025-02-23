#pragma once
#include <cstdint>

namespace message
{

    struct RequestAll
    {
        int8_t request_type;
    };

    struct RequestSpecific
    {
        int8_t request_type;
        int8_t seq_num;
    };

    struct Response
    {
        char symbol[4];
        char side;
        int32_t quantity;
        int32_t price;
        int32_t seq_num;
    };

}
