#include "Logger.hpp"
#include <chrono>

using namespace std;
namespace Tool
{
    string FormatMessage(string Message)
    {
        return Message;
    }

    Logger::Logger()
    {
        mode = OutputMode::Console;
    }

    Logger::Logger(OutputMode mode)
    {
        this->mode = mode;
        this->outFile = make_unique<fstream>();
        const chrono::time_point now{std::chrono::system_clock::now()};
        auto fileTo = now;
        //outFile->open()
    }

    Logger::~Logger()
    {
        outFile->close();
    }

    void Logger::Debug(string info)
    {
#ifdef _DEBUG
        Log(info);
#endif
    }
    void Logger::Log(string info)
    {
        if (mode == OutputMode::Console || outFile == nullptr)
        {
            cout << FormatMessage(info) << endl;
        }
        else
        {
            *outFile << FormatMessage(info) << endl;
        }
    }
}