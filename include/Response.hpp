/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maamer <maamer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 21:11:00 by mtellami          #+#    #+#             */
/*   Updated: 2023/08/08 11:30:10 by maamer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "include.hpp"
#include "Cluster.hpp"
#include "Request.hpp"
#include "Client.hpp"

class Client;

enum ERRORS {
		OK                    	  = 200,
		CREATED               	  = 201,
		NO_CONTENT            	  = 204,
    MOVED_PERMANENTLY     	  = 301,
		BAD_REQUEST           	  = 400,
		UNAUTHORIZED          	  = 401,
		FORBIDDEN             	  = 403,
		NOT_FOUND             	  = 404,
		METHOD_NOT_ALLOWED    	  = 405,
		REQUEST_ENTITY_TOO_LARGE  = 413,
		REQUEST_URI_TOO_LARGE     = 414,
    INTERNAL_SERVER_ERROR     = 500,
		NOT_IMPLEMENTED           = 501,
};

class Response {
    public:
        Cluster     *_cluster;
        std::string _header;
        int         _status_code;
        std::string _http_version;
        std::string _content_type;
        std::string _content_len;
        std::string _location;
        off_t       _body_size;


        Response(Cluster *cluster);
        std::string getStatusMsg(int status);
        void toString( std::string const  &type);
        void toStringGet();
        void setBodySize(off_t size);
        std::string getContentType();
        ~Response(void);
        std::string get_error_page(std::string page, int code);
        void  bad_request(Client *cl);
        void  payload_too_large(Client *cl);
        void  not_found(Client *cl);
        void  method_not_allowed(Client *cl);
        void  GET(Client *cl);
        void  POST(Client *cl);
        void  DELETE(Client *cl);

        
        void getListOfFiles(const char *path, std::vector<std::string> &list);
        bool file_exists(const char *path);
        std::string full_path(locations *var);
        char *joinRootAndPattern(const char *root, const char *pattern);
        bool isDirectory(const char *path);
        void removeDirectory(const char* path);
        void to_String_Delete( void );
};



#endif
