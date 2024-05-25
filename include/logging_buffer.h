#ifndef LOGGING_BUFFER_H
#define LOGGING_BUFFER_H

#include "logger.h"
#include <string>
#include <queue>
#include <mutex>
#include <utility>

using namespace std::chrono_literals;
#define WAIT_TIME 10ms

enum LogSeverity {
    INFO = 0,
    ERROR = 1,
    FATAL = 2
};

using BufferEntry = std::pair<LogSeverity, std::string>;

class LoggingBuffer {
    public:
        LoggingBuffer(std::queue<BufferEntry>* buffer1, std::queue<BufferEntry>* buffer2);
        bool addToBuffer(LogSeverity severity, std::string message);
        bool writeToLog();
    private:
        std::queue<BufferEntry>* available_buffer_;

        std::queue<BufferEntry>* buffer1_;
        std::queue<BufferEntry>* buffer2_;

        std::mutex writing_lock_;
        int available_buffer_index_;
};

#endif
