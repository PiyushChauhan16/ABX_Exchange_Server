#pragma once

#include "DispatcherBase.h"
#include "Log.h"

namespace dispatcher
{

    template <typename EXCHANGE>
    class SpinDispatcher : public DispatcherBase<typename EXCHANGE::Socket, typename EXCHANGE::Processor>
    {
    public:
        using PARENT = DispatcherBase<typename EXCHANGE::Socket, typename EXCHANGE::Processor>;
        using SOCKET = typename EXCHANGE::Socket;
        using PROCESSOR = typename EXCHANGE::Processor;

    public:
        void init(SOCKET sock, PROCESSOR proc);
        void run() override;
        void runOnce();
        void stop() { m_stop = true; }
        void start() { m_stop = false; }

    private:
        void process();

    private:
        bool m_stop;
        bool m_stop_successful_read;
    };

    template <typename EXCHANGE>
    void SpinDispatcher<EXCHANGE>::init(SOCKET sock, PROCESSOR proc)
    {
        m_stop = false;
        m_stop_successful_read = false;
        PARENT::init(sock, proc);
    }

    template <typename EXCHANGE>
    void SpinDispatcher<EXCHANGE>::run()
    {
        // stop is for stopping the dispatcher
        // based on some conditions happened
        // while looping. At the start of run
        // setting m_stop so that consecutive
        // call to run() never fails.
        m_stop = false;
        while (not m_stop)
        {
            process();
        }
    }

    template <typename EXCHANGE>
    void SpinDispatcher<EXCHANGE>::runOnce()
    {
        m_stop_successful_read = true;
        run();
    }

    template <typename EXCHANGE>
    void SpinDispatcher<EXCHANGE>::process()
    {

        char buffer[PARENT::m_processor.intended_response_size()] = {0};

        ssize_t response = ::recv(PARENT::m_socket.getFd(), buffer, sizeof(buffer), 0);
        if (response > 0)
        {
            LOG(DEBUG, "Received data of size ", buffer, " at socket ", PARENT::m_socket.getFd());
            PARENT::m_processor.onRead(buffer, response);
            m_stop = m_stop_successful_read;
        }
        else if (response < 0)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                LOG(INFO, "timed out! no data received, stopping dispatcher");
                m_stop = true;
            }
            else
            {
                LOG(ERROR, "something went wrong,", strerror(errno));
            }
        }
        else
        {
            LOG(INFO, "MarketConnection disconnected");
            m_stop = true;
        }
    }

}