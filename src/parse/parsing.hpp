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

class locations {
  private:
  public:
    std::string pattern;
    std::string methods;
    std::string redir_path;
    std::string root;
    bool autoindex;
    std::vector<std::string> def;
    std::vector<std::pair<std::string, std::string> > cgi; // vec.push_back(make_pair(string, string2);
    locations();
    ~locations();
};

// Change it from 'servers' to 'Config'
class Config {
  private:
  public:
    std::vector<locations> loc;
    std::string address;
    std::string host;
    std::string server_name;
    // int client_max_body_size;
    std::string client_max_body_size;
    std::map<int, std::string> error_pages;

    // error pages are missing here;
    Config();
    ~Config();
};
//should be done:
//    store err pages 
//    check for invalid args in other attributes/amount

#endif
