#include <unistd.h>
#include "../../include/Request.hpp"
#include "../parse/parsing.hpp"

void cgi_exec(std::string path, std::map<std::string, std::string> cgi, Request req)
{
  char *env[] = {NULL};
  //get method and parse path
  if (!req.get_method().compare("POST")) {
    //url exp of post meth: https://exple.com/cgi-bins/exp.py
    std::string PATH_INFO(path);
    //making an env to pass to execve based on req vars:
  //  char *env[] = {req.get_method().c_str(), NULL}; //other getters should be included accordingly to post method
    //the last word in a post url is the script's name:
    size_t pos = path.find_last_of('.');
    if (pos == std::string::npos) {
      std::cout << "404 err" << std::endl;
      return;
    }
    std::string key(path, pos+1, path.length() - pos);
    if (!cgi[key].compare("")){
      // unsupported script extension == no matching script name.
      std::cout << "404 err" << std::endl;
      return ;
    }
    else {
      char **args = new char*[3];
      args[0] = new char[cgi[key].length() + 1];
      args[1] = new char[path.length() + 1];
      std::strcpy(args[0], cgi[key].c_str());
      std::strcpy(args[1], path.c_str());
      args[2] = NULL;
      int pid = fork();
      if (pid < 0)
        std::cerr << "fork() failed!" << std::endl;
      else if (!pid)
      {
        execve(args[0], args, env);
        std::cerr << "execve failed!" << std::endl;
      }
      else if (pid > 0) {
        //checks on timeouts should be added!!
        waitpid(pid, NULL, 0);
      }
    }
  }
  else if (!req.get_method().compare("GET")) {
    //url exmpl of get meth: https://example.com/cgi-bins/scriptname/pathinfo?val1=one&val2=two
    size_t pos = path.find("?");
    if (pos != std::string::npos)
      std::string PATH_INFO(path, 0, pos);
    //char *env[] = {req.get_methodd(), NULL};

  }
}
