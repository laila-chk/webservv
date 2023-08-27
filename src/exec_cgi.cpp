#include "Client.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

char **init_env(Request* req, std::string path) {
  std::vector<std::string> env_vars;
  if (req->getContentLength().compare("")) 
    env_vars.push_back("CONTENT_LENGTH=" + req->getContentLength());
  if (req->get_query().compare(""))
    env_vars.push_back("QUERY_STRING=" + req->get_query());
  env_vars.push_back("REQUEST_METHOD=" + req->get_method());
  env_vars.push_back("SERVER_PROTOCOL=" + req->get_protocol());
  env_vars.push_back("PATH_INFO=" + path);

  char **env = new char *[env_vars.size() + 1];
  for (size_t i = 0; i < env_vars.size(); i++) {
    env[i] = new char[env_vars[i].length() + 1];
    strcpy(env[i], env_vars[i].c_str());
  }
  env[env_vars.size()] = NULL;
  return env;
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
    pos = full_path.find_last_of(".");
    std::string key(full_path, pos + 1, full_path.length() - pos);

    if (cgi[key].length()) {
      char **args = new char *[3];
      args[0] = new char[cgi[key].length() + 1];
      args[1] = new char[full_path.length() + 1];
      strcpy(args[0], cgi[key].c_str());
      strcpy(args[1], full_path.c_str());
      args[2] = NULL;
      int file = open(client->filename.c_str(), O_CREAT | O_RDWR, 0777);

      client->pid = fork();
      if (client->pid < 0)
        std::cerr << "fork() failed!" << std::endl;
      else if (client->pid == 0) {
        dup2(file, 1);
        execve(args[0], args, env);
				exit(1);
      }
    }
  }
	client->stats = waitpid(client->pid, NULL, WNOHANG);
}

