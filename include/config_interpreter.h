#ifndef CONFIG_INTERPRETER_H
#define CONFIG_INTERPRETER_H

#include "config_parser.h"
#include <vector>
#include <map>
#include <string>
#include "utils.h"

class ConfigInterpreter {
public:
    // Retrieves the port from the Nginx config file
    // Returns the port if found, otherwise returns 80 (default)
    // If port is set to a non-integer, will still return 80
    static int getPort(NginxConfig& config);

    static void setServerPaths(NginxConfig& config);

    static void setServerPaths(ServerPaths server_paths);

    static ServerPaths getServerPaths();

    static void set_api_key(const std::string& api_key) { api_key_ = api_key; }
    static std::string get_api_key() { return api_key_; }
    static void set_chatgpt_url(const std::string& chatgpt_url) { chatgpt_url_ = chatgpt_url; }
    static std::string get_chatgpt_url() { return chatgpt_url_; }

private:
    static ServerPaths server_paths_;
    static std::string api_key_;
    static std::string chatgpt_url_;
};

#endif
