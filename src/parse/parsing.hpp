#ifndef PARSING_HPP
#define PARSING_HPP

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iostream>

class locations {
  private:
  public:
    std::string pattern;
    //if a method is not allowed a 405 err page
    std::vector<std::string> methods;
    std::string redir_path;
    std::string root;
    bool autoindex;
    std::vector<std::string> def_files;
    //Change from vec of pairs into map for easier handling + bonus
//    std::vector<std::pair<std::string, std::string> > cgi;
    std::map<std::string, std::string> cgi;
    locations();
    ~locations();
};

// Change it from 'servers' to 'Config'
class Config {
  private:
  public:
    std::vector<locations> loc;
    std::string address;
    //if the port isn't determined in the config file, port will take the value -1
    int         port;
    std::string host;
    std::string server_name;
     int client_max_body_size;
    std::map<int, std::string> error_pages;
    Config();
    ~Config();
};

#endif
