#pragma once

namespace dispatcher
{

    template <typename SOCKET, typename PROCESSOR>
    class DispatcherBase
    {
    public:
        void init(SOCKET sock, PROCESSOR proc)
        {
            m_socket = sock;
            m_processor = proc;
        }
        virtual void run() = 0;

    public:
        SOCKET m_socket;
        PROCESSOR m_processor;
    };

}