#ifndef UTILS_H
#define UTILS_H

#include <map>
#include <string>
#include <vector>
    

//Struct for paths
struct ServerPaths{
    std::vector<std::string> echo_;
    std::map<std::string, std::string> static_;
    std::map<std::string, std::string> crud_;
    std::vector<std::string> health_;
    std::vector<std::string> sleep_;
};

#endif