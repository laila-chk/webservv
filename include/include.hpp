/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   include.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtellami <mtellami@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 09:25:47 by mtellami          #+#    #+#             */
/*   Updated: 2023/08/12 19:53:55 by mtellami         ###   ########.fr       */
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

#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

#include <string>
#include <list>
#include <vector>
#include <map>
#include <dirent.h>
// Macros
#define FAIL -1
#define SIZE 3072

// Typedefs
typedef int SOCK_FD; // socket file descriptor

// Exceptions
class System : public std::exception {
    public:
        virtual const char* what() const throw() {
            return "System call function failed.";
        }
};

#endif
