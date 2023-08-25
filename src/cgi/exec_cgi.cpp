#include "Client.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static int _stoi(std::string str) {
  std::istringstream iss(str);
  int nbr;
  iss >> nbr;
  return nbr;
}

char **init_env(Request* req, std::string path) {
  std::vector<std::string> env_vars;
  if (req->getContentLength().compare(""))
    env_vars.push_back("CONTENT_LENGTH=" + req->getContentLength());
  //    env_vars.push_back("CONTENT_TYPE = " + req->getContentType());
  if (req->get_query().compare(""))
    env_vars.push_back("QUERY_STRING=" + req->get_query());
  env_vars.push_back("REQUEST_METHOD=" + req->get_method());
  env_vars.push_back("SERVER_PROTOCOL=" + req->get_protocol());
  env_vars.push_back("PATH_INFO=" + path);

  // allocate extra space for var that should be added later and set them to
  // null;
  char **env = new char *[env_vars.size() + 1];
  for (size_t i = 0; i < env_vars.size(); i++) {
    env[i] = new char[env_vars[i].length() + 1];
    strcpy(env[i], env_vars[i].c_str());
  }
  env[env_vars.size()] = NULL;
  return env;
}

void parse_cgi_file(std::string filename, Response* res)
{
  bool head = 1;
  std::string buff;
  std::ifstream file;

  file.open(filename.c_str());
  if (file.fail())
    std::cerr << "Error occured while running cgi" << std::endl;
  while (getline(file, buff))
  {
    if (buff.compare("\r\n"))
      head = 0;
    if (head)
    {
      if (!buff.compare(0,12 ,"Content-type"))
      {
        std::string tmp(buff, 12, buff.length());
        res->_content_type = tmp;
      }
      else if (!buff.compare(0, 6,"Status"))
      {
        //setting stat num to -1 to check for it if it changed correctly
        res->_status_code = -1;
        // Status: 200 ok
        std::string status_code;
        std::stringstream line(buff);
        std::string word;
        getline(line, word, ' ');
        getline(line, status_code, ' ');
        getline(line, res->_status_message, ' ');
        res->_status_code = _stoi(status_code);
        if (res->_status_code == -1 || !res->_status_message.compare(""))
        {
          //some error should be returned here
          std::cerr << "this err msg should be changed!! " << std::endl;
          //  Status code 400 'Bad Request' may be used for an unknown request format, such as a missing CONTENT_TYPE.
          //do we have to change stt code in this case?
        }

      }

    }
    else
      res->body += buff;
    //here, we will be sending the body
  }
}

void cgi_exec(std::string path, Client *client) {
  client->filename = rand_name() + ".html";
  if (!client->done_cgi())
  {
    client->set_done_cgi(true);
    std::map<std::string, std::string> cgi(client->get_location()->cgi);
    size_t pos = path.find_last_of(".");
    while (pos < path.length() && path[pos] != '/')
      pos++;
    std::string full_path(path, 0, pos);
    char **env = init_env(client->_req, full_path);
    // get cgi binary to run script based on script's extension
    pos = full_path.find_last_of(".");
    std::string key(full_path, pos + 1, full_path.length() - pos);

    //  if (client.get_cluster()->get_conf().location[loc].cgi[key].length())
    if (cgi[key].length()) {
      char **args = new char *[3];
      args[0] = new char[cgi[key].length() + 1];
      args[1] = new char[full_path.length() + 1];
      strcpy(args[0], cgi[key].c_str());
      strcpy(args[1], full_path.c_str());
      args[2] = NULL;
      //creating an output file
      //    manually naming the file is pointless, there may be more than one script running at the same time
      int file = open(client->filename.c_str(), O_CREAT | O_RDWR, 0777);

      client->pid = fork();
      if (client->pid < 0)
        std::cerr << "fork() failed!" << std::endl;
      else if (client->pid == 0) {
        // duping the stdin is needed in order to pass stdin params to script,
        dup2(file, 1);
        // body is needed first.
        execve(args[0], args, env);
      } else if (client->pid > 0) {
        // checks on timeouts should be added!!
      }
    }
  }
	client->stats = waitpid(client->pid, NULL, WNOHANG);

  //assuming that the child finished successefully:
  //Parsing the header of cgi:

  // parse_cgi_file(filename, client->_res);

  /*
     if (!waitpid(client->pid, &client->stats, WNOHANG))
     std::cerr << "still running" << std::endl;
     if (WIFEXITED(client->stats))
     std::cout << "succeful exit" << std::endl;
  //setting cgi as done cuz we left the child
  std::cout << "==++--=-  " << env[0] << std::endl;
  std::cout << "==++--=-  " << env[1] << std::endl;
  // else   -->  that means no bin is available to run the script;
  */
}

// NEED TO ADD:
//   [x]  getters in Client class
//   [x]  a var to check weither the script is running
//   [ ]  check if we already runned the script
//   [x] read more on rfc
//   [ ] parse new header
//   [ ]
//
//    CGI RESPONSE FORM:
//    message-header (one or more header fields )
//    \r\n\r\n (blank line)
//    Body (may be NULL )
//
//
//    script must return one of three options, we will go for *returning a document response*
//    
