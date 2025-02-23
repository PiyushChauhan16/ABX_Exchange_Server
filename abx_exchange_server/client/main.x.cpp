#include "Types.h"
#include "ABXMarketConnection.h"
#include "Message.h"
#include "Log.h"
#include "ABXProcessor.h"

struct ABX_Exchange : public DefaultType<ABX_Exchange>
{
    using LinkType = link_type::IPv4TCPSocket;
    using Address = address::IPv4Address;
    using ReqAllMsg = message::RequestAll;
    using ReqSpecificMsg = message::RequestSpecific;
    using ResponseMsg = message::Response;
    using Processor = processor::ABXProcessor<ABX_Exchange>;
    using MarketConnection = ABXMarketConnection<ABX_Exchange>;
    using SingletonBuffer = buffer::SingletonVector<ResponseMsg>;
};

struct Config
{
    // Logger config
    bool enable_std_logger;
    bool enable_file_logger;
    std::string file_path;
    int log_level;

    // exchange config;
    std::string exchange_uri;
    int port;

    Config(bool std_logger, bool file_logger, const std::string &path, int level, const std::string &uri, int p)
        : enable_std_logger(std_logger), enable_file_logger(file_logger), file_path(path),
          log_level(level), exchange_uri(uri), port(p) {}
};

int main(int argc, char **argv)
{
    // Default config for the client
    // Tweak the values to play around
    // with client.
    Config config = Config(
        /*enable_std_logger =*/true,
        /*enable_file_logger =*/true,
        /*file_path =*/"./output.log",
        /*log_level =*/3,                // ALL
        /*exchange_uri = */ "127.0.0.1", // taking loopback uri for portability
        /*port = */ 3000                 // server port
    );

    auto &Logger = LoggerSingleton::instance(static_cast<LoggerSingleton::SEVERITY>(config.log_level));
    if (config.enable_std_logger)
        Logger.addStdLogger();
    if (config.enable_file_logger)
        Logger.addFileLogger(config.file_path);

    LOG(DEBUG, "Starting ABX Client");
    ABXMarketConnection<ABX_Exchange>().start(config.exchange_uri, config.port);
    return 0;
}