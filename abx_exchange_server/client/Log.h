#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>

[[maybe_unused]] bool createFileWithDirectories(const std::string &filePath)
{
    std::filesystem::path path(filePath);

    // Extract parent directory
    std::filesystem::path parentDir = path.parent_path();

    // Create directories if they don't exist
    if (!parentDir.empty() && !std::filesystem::exists(parentDir))
    {
        std::filesystem::create_directories(parentDir);
    }

    // Create the file
    std::ofstream file(filePath);
    if (file)
    {
        std::cout << "File created: " << filePath << std::endl;
        return true;
    }
    else
    {
        std::cerr << "Failed to create file: " << filePath << std::endl;
        return false;
    }
}

#define LOG(...) LoggerSingleton::instance().addPrefixAndWrite(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

class LoggerSingleton
{
public:
    enum SEVERITY
    {
        ERROR,
        INFO,
        DEBUG,
        ALL
    };

private:
    LoggerSingleton(SEVERITY severity) : m_severity(severity), m_enable_std_logger(false), m_enable_file_logger(false), m_file() {}

public:
    ~LoggerSingleton();

    static LoggerSingleton &instance(SEVERITY severity = SEVERITY::ALL)
    {
        static auto singleton = std::shared_ptr<LoggerSingleton>(new LoggerSingleton(severity));
        return (*singleton);
    }
    void addStdLogger() { m_enable_std_logger = true; }
    void addFileLogger(const std::string &file);
    template <typename FILE, typename LINE, typename FUNCTION, typename... MSG>
    void addPrefixAndWrite(FILE _file, LINE _line, FUNCTION _function, SEVERITY severity, MSG... rest);

private:
    void write();
    template <typename TYPE, typename... MSG>
    void write(TYPE first, MSG... rest);

private:
    SEVERITY m_severity;
    bool m_enable_std_logger;
    bool m_enable_file_logger;
    std::string m_file;
    std::ofstream m_file_of;
};

LoggerSingleton::~LoggerSingleton()
{
    if (m_enable_file_logger)
    {
        m_file_of.close();
    }
}

void LoggerSingleton::addFileLogger(const std::string &file)
{
    m_enable_file_logger = true;
    m_file = file;
    if (not::createFileWithDirectories(file))
    {
        throw std::runtime_error(__PRETTY_FUNCTION__);
    }
    m_file_of = std::ofstream(file.c_str());
}

template <typename FILE, typename LINE, typename FUNCTION, typename... MSG>
void LoggerSingleton::addPrefixAndWrite(FILE _file, LINE _line, FUNCTION _function, SEVERITY severity, MSG... rest)
{
    if (severity > m_severity)
        return;
    std::string sev_str;

    switch (severity)
    {
    case SEVERITY::ERROR:
    {
        sev_str = "ERR";
        break;
    }
    case SEVERITY::DEBUG:
    {
        sev_str = "DEB";
        break;
    }
    case SEVERITY::INFO:
    {
        sev_str = "INF";
        break;
    }
    }

    if (m_enable_std_logger)
    {
        std::cout << "[ " << _file << " : " << _line << " ] " << sev_str << " " << _function << "(): ";
    }
    if (m_enable_file_logger)
    {
        (m_file_of) << "[ " << _file << " : " << _line << " ] " << sev_str << " " << _function << "(): ";
    }

    write(rest...);
}

template <typename TYPE, typename... MSG>
void LoggerSingleton::write(TYPE first, MSG... rest)
{
    if (m_enable_std_logger)
    {
        std::cout << first;
    }
    if (m_enable_file_logger)
    {
        (m_file_of) << first;
    }

    write(rest...);
}

void LoggerSingleton::write()
{
    if (m_enable_std_logger)
    {
        std::cout << std::endl;
    }
    if (m_enable_file_logger)
    {
        (m_file_of) << std::endl;
    }
}

#define ERROR LoggerSingleton::SEVERITY::ERROR
#define INFO LoggerSingleton::SEVERITY::INFO
#define DEBUG LoggerSingleton::SEVERITY::DEBUG