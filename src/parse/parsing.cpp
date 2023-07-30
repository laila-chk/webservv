#include "parsing.hpp"

Config::Config()
{
}

Config::~Config()
{
}

locations::locations() {}

locations::~locations() {}

void ft_perr(std::string msg) {
  std::cout << msg << std::endl;
  exit(1);
}

bool is_num(std::string str)
{
  for (int i = 0; str[i]; i++)
  {
    if (!isdigit(str[i]))
      return false;
  }
  return true;
}
bool is_valid_address(std::string address)
{
  //122.231.312.21:2000
  int octets = 0;
  std::string octet;
  int range;

  for (size_t i = 0; i < address.length(); i++)
  {
    if (!isdigit(address[i]) && address[i] != '.' && address[i] != ':')
      return false;
    if (address[i] == '.' && isdigit(address[i + 1]) && i != 0)
    {
      if (!is_num(octet))
        return false;
      range = stoi(octet);
      if (range > 255)
        return false;
      octet.clear();
      octets++;
    }
    if (address[i] == ':')
    {
      if (octets != 3 || !is_num(octet) || i == address.length() - 1)
        return false;
      range = stoi(octet);
      if (range > 255)
        return false;
      octet.clear();
    } 
    if (isdigit(address[i]))
      octet += address[i];
  }
  if (!is_num(octet) || octets != 3)
    return false;
  return true;
}

bool directive(std::string buff, std::vector<std::string> serv_dirs, Config &srv, bool inLoc, int ii) 
{
  if (!buff.compare("{"))
    return true;
  int j = 0;
  std::string port;
  std::vector<std::string> words;

  for (size_t i = 0; i < buff.length(); i++) {
    if (buff[i] == ' ' || buff[i] == '\t' || buff[i] == ';') {
      words.push_back(buff.substr(j, i - j));
      while (buff[i] == ' ' || buff[i] == '\t')
        i++;
      j = i;
    }
  }

  for (size_t i = 0; i < serv_dirs.size(); i++) {
    if (!inLoc && !words[0].compare(serv_dirs[i])) {
      if (!words[0].compare("server_name"))
      {
        if (words.size() != 2)
          ft_perr("Error: bad server_name format!");
        srv.server_name = words[1];
      }
      else if (!words[0].compare("listen"))
      {
        if (words.size() != 2)
          ft_perr("Error: bad address format!");
        if (!is_valid_address(words[1]))
          ft_perr("Error: Invalid address!");

        size_t k = 0;
        while (k < words[1].length() && words[1][k] != ':')
          srv.address += words[1][k++];
        k++;
        while (k < words[1].length())
          port += words[1][k++];
        if (port.length())
          srv.port = stoi(port);
        else
          srv.port = -1;
      }
      else if (!words[0].compare("client_max_body_size"))
      {
        if (words.size() != 2)
          ft_perr("Error: bad client_max_body_size format!");
        if (!is_num(words[1]))
          ft_perr("Error: client_max_body_size should be a number!");
        srv.client_max_body_size = stoi(words[1]);
      }
      else if (!words[0].compare("error_page"))
      {
        for (size_t x  = 1; x < words.size(); x++)
        {
          if (x != words.size() - 1 && !is_num(words[x]))
            ft_perr("Error: error code should be a number!");
          if (x == words.size() - 1 && (is_num(words[x]) || words[x].compare(words[x].length() - 5, 5, ".html")))
            ft_perr("Error: error_page file is invalid!");
          if (is_num(words[x]))
            srv.error_pages[stoi(words[x])] = words[words.size()-1];
        }
      }
      return true;
    } else if (inLoc && !words[0].compare(serv_dirs[i])) {
      if (!words[0].compare("pattern"))
      {
        if (words.size() != 2)
          ft_perr("Error: bad pattern format!");
        srv.loc[ii].pattern = words[1];
      }

      else if (!words[0].compare("limit_except"))
        srv.loc[ii].methods = words[1];
      else if (!words[0].compare("return"))
      {
        if (words.size() != 3 || !is_num(words[1]))
          ft_perr("Error: bad redirection!");
        srv.loc[ii].redir_path = words[2];
      }
      else if (!words[0].compare("root"))
      {
        if (words.size() != 2)
          ft_perr("Error! Bad root format!");
        srv.loc[ii].root = words[1];
      }
      else if (!words[0].compare("autoindex"))
      {
        if (words.size() != 2 || (words[1].compare("on") && words[1].compare("off")))
          ft_perr("Error! bad autoindex format!");
        srv.loc[ii].autoindex = false;
        if (!words[1].compare("on"))
          srv.loc[ii].autoindex = true;
      }
      else if (!words[0].compare("index"))
        srv.loc[ii].def_files.push_back(words[1]);

      else if (!words[0].compare("cgi")) {
        if (words.size() != 3 || words[1][0] != '.')
          ft_perr("Error: cgi bad format!");

        std::string key;
        //skippin the '.' in the begining of an extention.
        for (size_t x = 1; x < words[1].length(); x++)
          key += words[1][x];
        srv.loc[ii].cgi[key] = words[2];
        //srv.loc[ii].cgi.push_back(make_pair(words[1], words[2]));
      }
      return true;
    }
  }
  std::cout << buff << std::endl;
  return false;
}

void dirs(std::vector<std::string> &serv_dirs) {
  // we cann later add location directives too, since they will be know by their
  // indexs.
  serv_dirs.push_back("server_name");
  serv_dirs.push_back("listen");
  serv_dirs.push_back("client_max_body_size");
  serv_dirs.push_back("error_page");

  // location directives:
  serv_dirs.push_back("pattern");
  serv_dirs.push_back("limit_except");
  serv_dirs.push_back("return");
  serv_dirs.push_back("root");
  serv_dirs.push_back("autoindex");
  serv_dirs.push_back("index");
  serv_dirs.push_back("cgi");
}

void Serv_block_init(std::vector<Config> &srvs, std::string path) {
  std::string buff;
  std::ifstream file(path);
  std::string word;
  std::vector<std::string> serv_dirs;
  int i = 0;
  int ii = 0;
  bool inSer = false;
  bool inLoc = false;
  int j = 0;

  dirs(serv_dirs);
  if (file.fail())
    ft_perr("Error: Bad file permissions, or file doesn't exist!");
  while (getline(file, buff)) {
    if (buff.length()) {
      buff.erase(0, buff.find_first_not_of(" \t"));
      if (buff[0] == '#')
        continue;
      j = buff.length() - 1;
      while (j > 0 && (buff[j] == ' ' || buff[j] == '\t'))
        j--;
      buff.erase(j + 1, buff.length());

      if (!inSer && (!buff.compare(0, 6, "server") && buff.length() > 6))
        ft_perr("Error: bad syntax!");

      if (!buff.compare("server") && !inSer) {
        srvs.push_back(Config());
        getline(file, buff);
        buff.erase(std::remove_if(buff.begin(), buff.end(), isspace),
            buff.end());
        if (buff.compare("{") != 0)
          ft_perr("Error: Opening bracket is missing !");
        inSer = true;
        i++;
      } else if (!buff.compare("server") && inSer)
        ft_perr("Error: Server can't exist within a context");

      if (inSer &&
          (buff.compare(0, 8, "location") == 0 && buff.length() == 8)) {
        srvs[i - 1].loc.push_back(locations());
        ii++;

        getline(file, buff);
        buff.erase(std::remove_if(buff.begin(), buff.end(), isspace),
            buff.end());
        if (buff.compare("{") != 0)
          ft_perr("Error: Opening bracket is missing for location!");
        inLoc = true;
      } else if (inSer &&
          (!buff.compare(0, 8, "location") && buff.length() != 8))
        ft_perr("Error: bad syntax!");
      if (inLoc && !buff.compare("}"))
        inLoc = false;

      else if (inSer && !buff.compare("}"))
        inSer = false;

      else if (!directive(buff, serv_dirs, srvs[i - 1], inLoc, ii - 1))
        ft_perr("Error: Uknown directive!");
    }
  }
  if (inSer || inLoc)
    ft_perr("Error: missing Bracket!");
}
