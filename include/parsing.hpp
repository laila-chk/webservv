#ifndef PARSING_HPP
#define PARSING_HPP

#include "include.hpp"

class locations {
  private:
  public:
    std::string pattern;
    std::vector<std::string> methods;
    std::string redir_path;
    std::string root;
    bool autoindex;
    std::vector<std::string> def_files;
    std::map<std::string, std::string> cgi;

    locations();
    ~locations();
};

class Config {
  private:
  public:
    std::vector<locations> loc;
    std::string address;
    int         port;
    std::string server_name;
    int client_max_body_size;
    std::map<int, std::string> error_pages;

    Config();
    ~Config();
};

#endif
