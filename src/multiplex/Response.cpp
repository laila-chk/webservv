/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maamer <maamer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 21:15:03 by mtellami          #+#    #+#             */
/*   Updated: 2023/08/17 19:23:34 by mtellami         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <cstring>
Response::Response(Cluster *cluster) : _cluster(cluster) {
}

Response::~Response() {
}


//The purpose of this function is to determine the appropriate Content-Type 
//The code extracts the file extension from the _path variable of the Request class
std::string Request::getContentType()
{
    std::map<std::string, std::string> content;
    content["txt"] = "text/plain";
    content["html"] = "text/html";
    content["css"] = "text/css";
    content["js"] = "text/javascript";
    content["json"] ="application/json";
    content["xml"] = "application/xml";
    content["pdf"] = "application/pdf";
    content["zip"] = "application/zip";
    content["jpeg"] = "image/jpeg";
    content["png"] = "image/png";
    content["gif"] = "image/gif";
    content["ogg"] = "audio/ogg";
    content["wav"] = "audio/wav";
    content["mp3"] = "audio/mp3";
    content["webm"] = "video/webm";
	content["mp4"] = "video/mp4";
    content[""] = "application/octet-stream";
    if (content.find(_start_line[1].substr(_start_line[1].find_last_of(".") + 1)) != content.end())
		return content[_start_line[1].substr(_start_line[1].find_last_of(".") + 1)];
	else
		return content[""];

}

std::string Response::getStatusMsg(int status)
{
    switch(status)
	{
		case OK:
			return "OK";
		case CREATED:
			return "Created";
		case NO_CONTENT:
			return "No Content";
		case BAD_REQUEST:
			return "Bad Request";
		case FORBIDDEN:
			return "Forbidden";
		case NOT_FOUND:
			return "Not Found";
        case MOVED_PERMANENTLY:
            return "Moved permanently";
		case NOT_IMPLEMENTED:
			return "Not Implemented";
		case METHOD_NOT_ALLOWED:
			return "Not Allowed";
		case REQUEST_ENTITY_TOO_LARGE:
			return "Request Entity Too Large";
		case REQUEST_URI_TOO_LARGE:
			return "Request URI Too Large";
    case INTERNAL_SERVER_ERROR:
      return "Internal server error";
		default:
			throw std::runtime_error("Unknown status code" + std::to_string(status));
	}
}

// void Response::setBodySize(off_t size)
// {
// 	this->_body_size = size;
// }
// void Response::toString(std::string const  &type)
// {
// 	this->_header += "HTTP/1.1 ";
// 	this->_header += std::to_string(this->_status_code) + " " + getStatusMsg(this->_status_code);
// 	this->_header += "\r\n";
// 	if (this->_location != "")
// 	{
// 		this->_header += "Location: " + this->_location + "\r\n";
// 		this->_header += std::string("Connection: close") + "\r\n\r\n";
// 		return ;
// 	}
// 	this->_header += "Content-Type: " + type + "\r\n";
// 	this->_header += "Content-Length: " + std::to_string(this->_body_size) + "\r\n";
// 	this->_header += std::string("Connection: close") + "\r\n\r\n";
// }


// int Response:: get_methode(Config &config_file)
// {

// }

std::string Response::get_error_page(std::string page, int code) {
  std::string line("HTTP/1.1 ");
  line += std::to_string(code) + " ";
  line += getStatusMsg(code) + "\n";
  std::string res("Content-Type: text/html\nContent-Length: ");
  res = line + res;
  std::string content;
  char buffer[2];
  int fd = open(page.c_str(), O_RDONLY);
  if (fd == FAIL) {
    std::cerr << "Cant open the file" << std::endl;
    throw System();
  }
  while (true) {
    int i = read(fd, buffer, 1);
    if (i == FAIL)
      throw System();
    if (!i)
      break;
    content += std::string(buffer, i);
  }
  res += std::to_string(content.length()) + "\n\n";
  res += content;
  return res;
}

void  Response::bad_request(Client *cl) {
    std::string res = get_error_page("src/response_pages/400.html", 400);
    send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
}

void  Response::payload_too_large(Client *cl) {
  std::string res = get_error_page("src/response_pages/413.html", 413);
  send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
}

void  Response::not_found(Client *cl) {
  std::string res = get_error_page("src/response_pages/404.html", 404);
  send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
}

void  Response::method_not_allowed(Client *cl) {
  std::string res = get_error_page("src/response_pages/405.html", 405);
  send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
}


char *Response::joinRootAndPattern(const char *root, const char *pattern) {
	size_t rootLen = std::strlen(root);
	size_t patternLen = std::strlen(pattern);
	char *fullPath = new char[rootLen + patternLen + 2];
	std::strcpy(fullPath, root);
	std::strncat(fullPath, "/", 2);
	std::strncat(fullPath, pattern, patternLen);
	return (fullPath);
}

bool Response::file_exists(const char *path) {
	return (access(path, F_OK) == 0);
}

bool Response::isDirectory(const char *path) {
	DIR* dir = opendir(path);
	if (dir != NULL)
	{
		closedir(dir);
		return true;
	}
	return false;
}
// If autoindex is enabled for a specific directory, the server will generate an HTML page that lists the contents (files 
//and subdirectories) of that directory and send it as the response to the user's request.
void Response::getListOfFiles(const char *path, std::vector<std::string> &list) {
	DIR* dir = opendir(path);
	if (dir != NULL)
	{
		struct dirent* entry;
		entry = readdir(dir);
		while (entry)
		{
			if (entry->d_type == DT_REG || entry->d_type == DT_DIR)
			{
				list.push_back(entry->d_name);
			}
			entry = readdir(dir);
		}
		closedir(dir);
	}
}

// main mathods
void  Response::GET(Client *cl) {
  // url :  
  // join root and pattern
  // file ()
  // directory
  // redirection : _start_lien[1] = value in return;
  std::string res("HTTP/1.1 200 OK\n\
  Content-Type: text/html\n\
  Content-Length: 20\n\n\
  <h1 style=\"font-size:5rem\">GET REQUEST</h1>");
  send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
}

void Response::POST(Client *cl) {
  std::string res = get_error_page("src/response_pages/201.html", 201);
  send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
}

void Response:: removeDirectory(const char* path)
{
    std::vector<std::string> stack;
    stack.push_back(path);

    while (!stack.empty())
    {
        const std::string currentPath = stack.back();
        stack.pop_back();

        DIR* dir = opendir(currentPath.c_str());
        if (!dir)
        {
            std::cerr << "Error opening directory: " << currentPath << std::endl;
            continue;
        }

        dirent* entry;
        while ((entry = readdir(dir)) != nullptr)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            std::string file_path = currentPath + "/" + entry->d_name;

            if (entry->d_type == DT_DIR)
                stack.push_back(file_path);
            else
                remove(file_path.c_str());
        }

        closedir(dir);
        rmdir(currentPath.c_str());
  }
}

void Response::to_String_Delete( void )
{
	this->_header += "HTTP/1.1 ";
	this->_header += std::to_string(this->_status_code) + " " + getStatusMsg(this->_status_code) + "\r\n";
	this->_header += std::string("Server: WebServ/1.0.0 (Unix)") + "\r\n";
	this->_header += std::string("Connection: close") + "\r\n\r\n";
}
void Response::DELETE(Client *cl) {
  std::string url = cl->get_location()->root + cl->get_req()->get_url();
  //std:: string url = joinRootAndPattern(cl->get_location()->root.c_str(), cl->get_req()->get_url().c_str());
  std::string res;
    if (isDirectory(url.c_str()))
    {
        removeDirectory(url.c_str());
        this->_status_code = NO_CONTENT;
    }
    else
    {
        if (access(url.c_str(), F_OK) == -1)
            this->_status_code = FORBIDDEN;
        else if (access(url.c_str(), W_OK) == -1)
            this->_status_code = FORBIDDEN;
        else if (remove(url.c_str()) == 0)
            this->_status_code = NO_CONTENT;
        else
            this->_status_code = INTERNAL_SERVER_ERROR;
    }
    if (this->_status_code >= BAD_REQUEST)
        res = get_error_page("src/response_pages/400.html", 400);
    else
        this->to_String_Delete();

  // std::string url = cl->get_location()->root + cl->get_req()->get_url();
  // std::string res;

  // if (!access(url.c_str(), F_OK)) {
  //   if (!isDirectory(url.c_str())) {
	// 		unlink(url.c_str());
  //     res = get_error_page("src/response_pages/200.html", 200);
  //   } else {
  //     res = get_error_page("src/response_pages/403.html", 403);
  //   }
  // } else {
  //   res = get_error_page("src/response_pages/404.html", 404);
  // }
  // send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
}

