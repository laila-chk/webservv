/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   include.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtellami <mtellami@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 09:25:47 by mtellami          #+#    #+#             */
/*   Updated: 2023/08/29 15:29:24 by mtellami         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDE_HPP
#define INCLUDE_HPP

// Headers
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h> 
#include <dirent.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

#include <algorithm>
#include <cctype>
#include <cstdlib>

#include <cstring>
#include <string>
#include <list>
#include <vector>
#include <map>

// Macros
#define FAIL -1
#define SIZE 4096

// Typedefs
typedef int SOCK_FD;

// Exceptions
class System : public std::exception {
  public:
    virtual const char* what() const throw() {
      return "System call function failed.";
    }
};

#endif
