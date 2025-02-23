#pragma once

#include <string>
#include <vector>
#include <unistd.h>

template <typename EXCHANGE>
class ABXMarketConnection
{
public:
    enum MKT_ACTION
    {
        STREAM_ALL = 1,
        RESEND_PACKET = 2,
    };

    using SOCKET = typename EXCHANGE::Socket;
    using REQ_ALL_MSG = typename EXCHANGE::ReqAllMsg;
    using REQ_SPEC_MSG = typename EXCHANGE::ReqSpecificMsg;
    using RESPONSE_MSG = typename EXCHANGE::ResponseMsg;
    using PROCESSOR = typename EXCHANGE::Processor;
    using DISPATCHER = typename EXCHANGE::Dispatcher;
    using COMMON_BUFFER = typename EXCHANGE::SingletonBuffer;

public:
    void start(const std::string &address, int port);
    static void dumpPacket(RESPONSE_MSG msg);
    std::string jsonify();
    static void newSymbol(RESPONSE_MSG msg);

private:
    SOCKET m_link;
    PROCESSOR m_processor;
    DISPATCHER m_dispatcher;
    std::vector<RESPONSE_MSG> m_data;
};

template <typename EXCHANGE>
void ABXMarketConnection<EXCHANGE>::dumpPacket(RESPONSE_MSG msg)
{
    LOG(INFO, "[EXCHANGE PACKET]");
    LOG(INFO, "symbol:", msg.symbol[0], msg.symbol[1], msg.symbol[2], msg.symbol[3]);
    LOG(INFO, "side:", msg.side);
    LOG(INFO, "quantity:", msg.quantity);
    LOG(INFO, "price:", msg.price);
    LOG(INFO, "seq_num:", msg.seq_num);
    LOG(INFO, "");
}

template <typename EXCHANGE>
void ABXMarketConnection<EXCHANGE>::newSymbol(RESPONSE_MSG msg)
{
    LOG(INFO, "received new symbol packet");
    dumpPacket(msg);
}

template <typename EXCHANGE>
std::string ABXMarketConnection<EXCHANGE>::jsonify()
{
    std::string json, space4(4, ' '), space8(8, ' ');

    // starting the list
    json += "[\n";

    for (auto packet : COMMON_BUFFER::instance().get())
    {
        json += space4 + "{\n";
        json += space8 + "\"symbol\" : \"" + packet.symbol[0] + packet.symbol[1] + packet.symbol[2] + packet.symbol[3] + "\",\n";
        json += space8 + "\"side\" : \"" + packet.side + "\",\n";
        json += space8 + "\"quantity\" : " + std::to_string(packet.quantity) + ",\n";
        json += space8 + "\"price\" : " + std::to_string(packet.price) + ",\n";
        json += space8 + "\"seq_num\" : " + std::to_string(packet.seq_num) + ",\n";
        json += space4 + "},\n";
    }

    // ending the list
    json += "]\n";

    return json;
}

template <typename EXCHANGE>
void ABXMarketConnection<EXCHANGE>::start(const std::string &address, int port)
{
    auto requestAll = [&]()
    {
        // init operations
        m_link.createSocket();
        // disabling Nagle's algo and setting
        // default timeout to <3,0>
        m_link.setTCPNoDelay();
        m_link.setTimeout(3/*sec*/, 0 /*microsec*/);
        m_link.connect(address, port);

        m_processor.init(m_link);
        m_dispatcher.init(m_link, m_processor);

        // preparing message
        REQ_ALL_MSG msg;
        msg.request_type = MKT_ACTION::STREAM_ALL;

        // worker logic
        while (COMMON_BUFFER::instance().empty())
        {
            m_link.write(msg);
            m_dispatcher.run();
        }

        // closing connection from our side
        // as well since server disconnects
        // the client after fulfilling the
        // request.
        m_link.close();
    };

    auto requestMissingPackets = [&]()
    {
        // init operatins
        m_link.createSocket();
        // disabling Nagle's algo and setting
        // default timeout to <3,0>
        m_link.setTCPNoDelay();
        m_link.setTimeout(3/*sec*/, 0 /*microsec*/);
        m_link.connect(address, port);

        // identify and storing missing seqnums
        // that needs to be requested again
        int expected_seqnum = 1;
        std::vector<int> missing_seqnum;

        for (auto packet : COMMON_BUFFER::instance().get())
        {
            while (expected_seqnum < packet.seq_num)
            {
                LOG(INFO, "packet with seqnum '", expected_seqnum, "' is missing");
                missing_seqnum.push_back(expected_seqnum++);
            }
            expected_seqnum++;
        }

        // sending request and populating buffer with
        // missing seqnum packets
        for (auto mis_seqnum : missing_seqnum)
        {
            LOG(INFO, "requesting packet with seqnum:", expected_seqnum);
            REQ_SPEC_MSG msg;
            msg.request_type = MKT_ACTION::RESEND_PACKET;
            msg.seq_num = mis_seqnum;
            m_link.write(msg);

            m_dispatcher.start();
            m_dispatcher.runOnce();
        }

        // closing connection.
        m_link.close();
    };

    LOG(INFO, "starting ABX_EXCHANGE client");
    // requesting information for all
    // ticker symbols from ABX Exchange
    requestAll();

    // requesting the information for
    // a seqnum which are missed in
    // requestAll() call.
    //
    // Here, we assume that we never miss
    // the last seqnum received. Therefore,
    // an attempt is made to have all the packets
    // with seqnum in range [1, last_seqnum_recv];
    requestMissingPackets();

    // sorting the packets in increasing order
    // based on seqnum
    COMMON_BUFFER::instance().sort([](RESPONSE_MSG a, RESPONSE_MSG b)
                                   { return a.seq_num < b.seq_num; });

    // converting std::vector of
    // packets in a JSON string.
    LOG(DEBUG, jsonify());
}