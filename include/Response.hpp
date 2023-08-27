/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maamer <maamer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 21:11:00 by mtellami          #+#    #+#             */
/*   Updated: 2023/08/27 15:35:28 by mtellami         ###   ########.fr       */
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
				Client			*_client;
        std::string _header;
				std::string _body;
        int         _status_code;
        std::string _http_version;
        std::string _content_type;
        std::string _content_len;
        off_t       _body_size;

				// for cgi
				std::string _status_message;
				std::string body;

        Response(Cluster *cluster, Client *client);
				~Response(void);

        std::string getStatusMsg(int status);
        std::string get_error_page(std::string page, int code);

        void  bad_request(Client *cl);
        void  payload_too_large(Client *cl);
        void  not_found(Client *cl);
        void  method_not_allowed(Client *cl);

        void	GET(Client *cl, locations *var);
        void  POST(Client *cl);
        void  DELETE(Client *cl);

        bool file_exists(const char *path);
        bool isDirectory(const char *path);
        void removeDirectory(const char* path);
        void to_String_Delete( void );
				void get_body_content(std::string url);
        void to_string_get(Client *cl, std::string path);
				void handle_redirection(Client *cl, locations *var);

				std::string final_url(Client *cl);

        void setBodySize(std::string &path);
        void handleFileRequest(Client *cl, std::string url);
        void handleDirectoryRequest(Client *cl, locations *var);

        void	auto_index(Client *client, std::string uri);
};

void cgi_exec(std::string path, Client *client);

#endif
