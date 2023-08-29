#include "parsing.hpp"

Config::Config() {
  server_name = "Webserv";
  client_max_body_size = 10000;

	std::string dir = "default.d/response_pages/";
  error_pages[200] = dir + "200.html";
  error_pages[201] = dir + "201.html";
  error_pages[204] = dir + "204.html";
  error_pages[301] = dir + "301.html";
  error_pages[307] = dir + "307.html";
  error_pages[400] = dir + "400.html";
  error_pages[401] = dir + "401.html";
  error_pages[403] = dir + "403.html";
  error_pages[404] = dir + "404.html";
  error_pages[405] = dir + "405.html";
  error_pages[413] = dir + "413.html";
  error_pages[414] = dir + "414.html";
  error_pages[500] = dir + "500.html";
  error_pages[501] = dir + "501.html";
}

Config::~Config() {}

locations::locations() {
  pattern = "";
  autoindex = false;
  root = "default.d/";
  cgi["py"] = "default.d/cgi_bins/python";
  cgi["php"] = "default.d/cgi_bins/php";
}

locations::~locations() {
}

void ft_perr(std::string msg) {
  std::cerr << msg << std::endl;
  exit(EXIT_FAILURE);
}

bool is_num(std::string str) {
  for (int i = 0; str[i]; i++) {
    if (!isdigit(str[i]))
      return false;
  }
  return true;
}

// _stoi
static int _stoi(std::string str) {
  std::istringstream iss(str);
  int nbr;
  iss >> nbr;
  return nbr;
}

bool is_valid_address(std::string address) {
  int octets = 0;
  std::string octet;
  int range;

  for (size_t i = 0; i < address.length(); i++) {
    if (!isdigit(address[i]) && address[i] != '.' && address[i] != ':')
      return false;
    if (address[i] == '.' && isdigit(address[i + 1]) && i != 0) {
      if (!is_num(octet))
        return false;
      range = _stoi(octet);
      if (range > 255)
        return false;
      octet.clear();
      octets++;
    }
    if (address[i] == ':') {
      if (octets != 3 || !is_num(octet) || i == address.length() - 1)
        return false;
      range = _stoi(octet);
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

bool directive(std::string buff, std::vector<std::string> serv_dirs,
               Config &srv, bool inLoc, int ii) {
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
      if (!words[0].compare("server_name")) {
        if (words.size() != 2)
          ft_perr("Error: bad server_name format!");
        srv.server_name = words[1];
      } else if (!words[0].compare("listen")) {
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
          srv.port = _stoi(port);
        else
          ft_perr("Error: port is required in listen directive.");
      } else if (!words[0].compare("client_max_body_size")) {
        if (words.size() != 2)
          ft_perr("Error: bad client_max_body_size format!");
        if (!is_num(words[1]))
          ft_perr("Error: client_max_body_size should be a number!");
        srv.client_max_body_size = _stoi(words[1]);
      } else if (!words[0].compare("error_page")) {
        for (size_t x = 1; x < words.size(); x++) {
          if (x != words.size() - 1 && !is_num(words[x]))
            ft_perr("Error: error code should be a number!");
          if (x == words.size() - 1 &&
              (is_num(words[x]) ||
               words[x].compare(words[x].length() - 5, 5, ".html")))
            ft_perr("Error: error_page file is invalid!");
          if (is_num(words[x]))
            srv.error_pages[_stoi(words[x])] = words[words.size() - 1];
        }
      }
      return true;
    } else if (inLoc && !words[0].compare(serv_dirs[i])) {
      if (!words[0].compare("pattern")) {
        if (words.size() != 2)
          ft_perr("Error: bad pattern format!");
        srv.loc[ii].pattern = words[1];
      }
      else if (!words[0].compare("limit_except")) {
        for (size_t x = 1; x < words.size(); x++)
          srv.loc[ii].methods.push_back(words[x]);
      } else if (!words[0].compare("return")) {
        if (words.size() != 3 || !is_num(words[1]))
          ft_perr("Error: bad redirection!");
        srv.loc[ii].redir_path = words[2];
      } else if (!words[0].compare("root")) {
        if (words.size() != 2)
          ft_perr("Error! Bad root format!");
        srv.loc[ii].root = "./" + words[1];
      } else if (!words[0].compare("autoindex")) {
        if (words.size() != 2 ||
            (words[1].compare("on") && words[1].compare("off")))
          ft_perr("Error! bad autoindex format!");
        srv.loc[ii].autoindex = false;
        if (!words[1].compare("on"))
          srv.loc[ii].autoindex = true;
      } else if (!words[0].compare("index")) 
        srv.loc[ii].def_files.push_back(words[1]);

      else if (!words[0].compare("cgi")) {
        if (words.size() != 3 || words[1][0] != '.')
          ft_perr("Error: cgi bad format!");

        std::string key;
        for (size_t x = 1; x < words[1].length(); x++)
          key += words[1][x];
        srv.loc[ii].cgi[key] = words[2];
      }
      return true;
    }
  }
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

void check_Configs(std::vector<Config> &srvs) {
  for (size_t i = 0; i < srvs.size(); i++) {
		for (size_t k = i + 1; k < srvs.size(); k++) {
			if (srvs[i].address == srvs[k].address && srvs[i].port == srvs[k].port)
				ft_perr("Error: duplicated address and port!");
		}
    if (!srvs[i].loc.size())
      ft_perr("Error: missing location context in server context!");
		for (std::map<int, std::string>::iterator it = srvs[i].error_pages.begin(); it != srvs[i].error_pages.end(); it++) {
			if (it->first < 100 || it->first > 600)
				ft_perr("Error: invalid error page code!");
			if (access(it->second.c_str(), R_OK))
				ft_perr("Error: can't access error page!");
		}
    for (size_t j = 0; j < srvs[i].loc.size(); j++) {
      if (!srvs[i].loc[j].pattern.compare(""))
        ft_perr("Error: pattern in location is required!");
      if (!srvs[i].loc[j].def_files.size())
        srvs[i].loc[j].def_files.push_back("");
			for (std::vector<std::string>::iterator it = srvs[i].loc[j].methods.begin();
					it != srvs[i].loc[j].methods.end(); it++) {
				if (*it != "GET" && *it != "POST" && *it != "DELETE")
					ft_perr("Error: unknown method!");
			}
			for (std::map<std::string, std::string>::iterator it = srvs[i].loc[j].cgi.begin(); it!= srvs[i].loc[j].cgi.end(); it++) {
				if (access(it->second.c_str(), X_OK))
					ft_perr("Error: can't execute cgi bin");
			}
    }
  }
}

void Serv_block_init(std::vector<Config> &srvs, std::string path) {
  std::string buff;
  std::ifstream file;

  file.open(path.c_str());
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
      if (!buff.size() || buff[0] == '#')
        continue;
      j = buff.length() - 1;
      while (j > 0 && (buff[j] == ' ' || buff[j] == '\t'))
        j--;
      buff.erase(j + 1, buff.length());

      if (!inSer && (!buff.compare(0, 6, "server") && buff.length() > 6))
        ft_perr("Error: bad syntax!");

      if (!buff.compare("server") && !inSer) {
        srvs.push_back(Config());

				ii=0;
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
  //  check_for_valid_server
  check_Configs(srvs);
}
