//
// Copyright(c) 2015 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//
//
// spdlog usage example
//
//

#define SPDLOG_TRACE_ON
#define SPDLOG_DEBUG_ON

#include "spdlog/sinks/daily_file_sink.h"

#include "spdlog/sinks/stdout_color_sinks.h"

#include <iostream>
#include <memory>


void basic_example();
void rotating_example();
void daily_example();
void async_example();
void user_defined_example();
void err_handler_example();
void syslog_example();

namespace spd = spdlog;
int main(int, char *[])
{

    try
    {
        auto console = spdlog::stdout_color_mt("console");
        console->info("Welcome to spdlog!");
        console->error("Some error message with arg: {}", 1);

        // Formatting examples
        console->warn("Easy padding in numbers like {:08d}", 12);
        console->critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
        console->info("Support for floats {:03.2f}", 1.23456);
        console->info("Positional args are {1} {0}..", "too", "supported");
        console->info("{:<30}", "left aligned");

        spd::get("console")->info("loggers can be retrieved from a global registry using the spdlog::get(logger_name)");

        // Runtime log levels
        spd::set_level(spd::level::info); // Set global log level to info
        console->debug("This message should not be displayed!");
        console->set_level(spd::level::trace); // Set specific logger's log level
        console->debug("This message should be displayed..");


        // Customize msg format for all loggers
        spd::set_pattern("[%H:%M:%S %z] [%^---%L---%$] [thread %t] %v");
        console->info("This an info message with custom format");



        // Compile time log levels
        // define SPDLOG_DEBUG_ON or SPDLOG_TRACE_ON
        SPDLOG_TRACE(console, "Enabled only #ifdef SPDLOG_TRACE_ON..{} ,{}", 1, 3.23);
        SPDLOG_DEBUG(console, "Enabled only #ifdef SPDLOG_DEBUG_ON.. {} ,{}", 1, 3.23);

        // various file loggers
        basic_example();
        rotating_example();
        daily_example();

        // async logging using a backing thread pool
        async_example();

        // user defined types logging by implementing operator<<
        user_defined_example();

        // custom  error handler
        err_handler_example();



        // Apply a function on all registered loggers
        spd::apply_all([&](std::shared_ptr<spdlog::logger> l) { l->info("End of example."); });

        // Release and close all loggers
        spdlog::drop_all();
    }
    // Exceptions will only be thrown upon failed logger or sink construction (not during logging)
    catch (const spd::spdlog_ex &ex)
    {
        std::cout << "Log init failed: " << ex.what() << std::endl;
        return 1;
    }
}


#include "spdlog/sinks/basic_file_sink.h"
void basic_example()
{
    // Create basic file logger (not rotated)
    auto my_logger = spd::basic_logger_mt("basic_logger", "logs/basic-log.txt");
}

#include "spdlog/sinks/rotating_file_sink.h"
void rotating_example()
{
    // Create a file rotating logger with 5mb size max and 3 rotated files
    auto rotating_logger = spd::rotating_logger_mt("some_logger_name", "logs/rotating.txt", 1048576 * 5, 3);
}

#include "spdlog/sinks/daily_file_sink.h"
void daily_example()
{
    // Create a daily logger - a new file is created every day on 2:30am
    auto daily_logger = spd::daily_logger_mt("daily_logger", "logs/daily.txt", 2, 30);

}






#include "spdlog/async.h"
void async_example()
{
    // default thread pool settings can be modified *before* creating the async logger:
    // spdlog::init_thread_pool(32768, 1); // queue with max 32k items 1 backing thread.
    // spdlog::init_thread_pool(32768, 4); // queue with max 32k items 4 backing threads.

    auto async_file = spd::basic_logger_mt<spdlog::async_factory>("async_file_logger", "logs/async_log.txt");

    // alternatively:
    // auto async_file = spd::create_async<spd::sinks::basic_file_sink_mt>("async_file_logger", "logs/async_log.txt");

    for (int i = 0; i < 100; ++i)
    {
        async_file->info("Async message #{}", i + 1);
    }
}



// user defined types logging by implementing operator<<
#include "spdlog/fmt/ostr.h" // must be included
struct my_type
{
    int i;
    template<typename OStream>
    friend OStream &operator<<(OStream &os, const my_type &c)
    {
        return os << "[my_type i=" << c.i << "]";
    }
};


void user_defined_example()
{
    spd::get("console")->info("user defined type: {}", my_type{14});
}

//
// custom error handler
//
void err_handler_example()
{
    // can be set globaly or per logger(logger->set_error_handler(..))
    spdlog::set_error_handler([](const std::string &msg) { spd::get("console")->error("*******my err handler: {}", msg); });
    spd::get("console")->info("some invalid message to trigger an error {}{}{}{}", 3);
    // spd::get("console")->info("some invalid message to trigger an error {}{}{}{}", 3);
}


// syslog example (linux/osx/freebsd)
#ifndef _WIN32
#include "spdlog/sinks/syslog_sink.h"
void syslog_example()
{
    std::string ident = "spdlog-example";
    auto syslog_logger = spd::syslog_logger("syslog", ident, LOG_PID);
    syslog_logger->warn("This is warning that will end up in syslog.");
}
#endif

// Android example
#if defined(__ANDROID__)
#incude "spdlog/sinks/android_sink.h"
void android_example()
{
    std::string tag = "spdlog-android";
    auto android_logger = spd::android_logger("android", tag);
    android_logger->critical("Use \"adb shell logcat\" to view this message.");
}

#endif
