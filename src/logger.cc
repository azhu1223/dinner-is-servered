#include "logger.h"
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/parameter/keyword.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

void initialize_logging(bool enable_file_logging) {
    boost::log::add_common_attributes();

    if (enable_file_logging) {
        boost::log::add_file_log(
            boost::log::keywords::file_name = "logs/server_%Y-%m-%d_%H-%M-%S.log",
            boost::log::keywords::rotation_size = 10 * 1024 * 1024, // 10 MB
            boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0), // Midnight rotation
            boost::log::keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%] [%ProcessID%]: %Message%"
        );
    }

    boost::log::add_console_log(
        std::cout,
        boost::log::keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%] [%ProcessID%]: %Message%"
    );
}
