#include "logger.h"
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include  <boost/log/utility/setup/file.hpp>
#include <boost/parameter/keyword.hpp>

void intialize_logging(bool enable_file_logging){
    boost::log::add_common_attributes();
    if (enable_file_logging){
        boost::log::add_file_log
        (
            boost::log::keywords::file_name = "logs/server_%m-%d-%Y_%T.log",
            boost::log::keywords::rotation_size = 10 * 1024 * 1024,
            boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
            boost::log::keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%]: %Message%"
        );
    }
    boost::log::add_console_log(std::cout,  boost::log::keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%]: %Message%");
}