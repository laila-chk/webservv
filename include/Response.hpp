/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maamer <maamer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 21:11:00 by mtellami          #+#    #+#             */
/*   Updated: 2023/07/31 12:27:40 by maamer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "include.hpp"
#include "../src/parse/parsing.hpp"
#include "Request.hpp"


#define BSIZE 1024 //temporary buffer to store header fields in

class Response {
    public:
        // some member function
        std::map<std::string, std::string>  _header_response;
        std::string protocol;
        std::string status_code;
        std::string status_message;

        //member functions
        void check_head(Config _config, Request request);
};

#endif