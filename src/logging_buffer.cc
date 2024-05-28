#include "logging_buffer.h"

#include <boost/log/trivial.hpp>
#include <string>
#include <queue>
#include <mutex>
#include <utility>
#include <thread>
#include <chrono>

LoggingBuffer::LoggingBuffer(std::queue<BufferEntry>* buffer1, std::queue<BufferEntry>* buffer2) 
    : available_buffer_index_(1), buffer1_(buffer1), buffer2_(buffer2), available_buffer_(buffer1), writing_lock_() {}

bool LoggingBuffer::addToBuffer(LogSeverity severity, std::string message) {
    std::unique_lock lock(writing_lock_);
    available_buffer_->emplace(severity, message);
    lock.unlock();

    return true;
}

bool LoggingBuffer::writeToLog() {
    if (!available_buffer_->empty()) {
        std::queue<BufferEntry>* queue = available_buffer_;

        // Switch the queue available to be written to allow other threads to buffer log 
        // messages while this thread logs all previously buffered messages.
        std::unique_lock lock(writing_lock_);

        switch(available_buffer_index_) {
            case 1:
                available_buffer_ = buffer2_;
                available_buffer_index_ = 2;
                break;
            case 2:
                available_buffer_ = buffer1_;
                available_buffer_index_ = 1;
                break;
        }

        lock.unlock();

        while(!queue->empty()) {
            BufferEntry top = queue->front();
            queue->pop();

            switch(top.first) {
                case INFO:
                    BOOST_LOG_TRIVIAL(info) << top.second;
                    break;
                case ERROR:
                    BOOST_LOG_TRIVIAL(error) << top.second;
                    break;
                case FATAL:
                    BOOST_LOG_TRIVIAL(fatal) << top.second;
                    break;
            }
        }
    }

    else {
        // If nothing is in the buffer, wait for the buffer to be populated.
        std::this_thread::sleep_for(WAIT_TIME);
    }

    return true;
}

bool LoggingBuffer::empty() {
    return buffer1_->empty() && buffer2_->empty();
}