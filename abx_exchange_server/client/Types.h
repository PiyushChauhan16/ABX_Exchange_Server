#pragma once

#include "Address.h"
#include "TCPSocket.h"
#include "SpinDispatcher.h"
#include "Buffer.h"

template <typename DERIVED>
struct DefaultType
{
    using Socket = link_type::TCPSocket<DERIVED>;
    using Dispatcher = dispatcher::SpinDispatcher<DERIVED>;
};
