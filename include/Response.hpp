/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maamer <maamer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 21:11:00 by mtellami          #+#    #+#             */
/*   Updated: 2023/08/23 18:07:41 by mtellami         ###   ########.fr       */
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
    TEMPORARY_REDIRECT    	  = 307,
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
				std::string _body;
        int         _status_code;
        std::string _http_version;
        std::string _content_type;
        std::string _content_len;
        off_t       _body_size;


        Response(Cluster *cluster);
        std::string getStatusMsg(int status);
        void toString(std::string const  &type, Client *cl);
        // std::string getContentType();
        ~Response(void);
        std::string get_error_page(std::string page, int code);
        void  bad_request(Client *cl);
        void  payload_too_large(Client *cl);
        void  not_found(Client *cl);
        void  method_not_allowed(Client *cl);
        void GET(Client *cl, locations *var);
        void  POST(Client *cl);
        void  DELETE(Client *cl);

        
  
        bool file_exists(const char *path);
        //std::string full_path(locations *var);
        bool isDirectory(const char *path);
        void removeDirectory(const char* path);
        void to_String_Delete( void );
				void get_body_content(Client *cl, std::string url);
        void to_string_get(Client *cl, std::string path);
        std::string final_url(Client *cl) ;


        void setBodySize(std::string &path);
        void handleFileRequest(Client *cl);
        void handleDirectoryRequest(Client *cl, locations *var) ;
        //std::string	get_exetention(std::map<std::string, std::string> mime, std::string exe);
        void	auto_index(Client *client, std::string uri);
        //bool redirection();
        std::vector<std::string> split(std::string &str, char delim, bool keepDelim);
        bool  fileExistsInDirectory(std::string &path, const std::string &fileName);
        bool checkDirFile(std::string &path, std::string const &index);
};



#endif
