/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maamer <maamer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 21:15:03 by mtellami          #+#    #+#             */
/*   Updated: 2023/08/30 15:59:53 by mtellami         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

// Constrcutor
Response::Response(Cluster *cluster, Client *client) : _cluster(cluster), _client(client) {
}

// Destructor
Response::~Response(){
}

// _to_string
static std::string _to_string(int n)
{
  std::ostringstream oss;
  oss << n;
  return oss.str();
}

// The purpose of this function is to determine the appropriate Content-Type
// The code extracts the file extension from the _path variable of the Request class
std::string Request::getContentType(std::string path)
{
  std::map<std::string, std::string> content;
  content["txt"] = "text/plain";
  content["html"] = "text/html";
  content["css"] = "text/css";
  content["js"] = "text/javascript";
  content["json"] = "application/json";
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
  if (content.find(path.substr(path.find_last_of(".") + 1)) != content.end())
    return content[path.substr(path.find_last_of(".") + 1)];
  else
    return content[""];
}

std::string Response::getStatusMsg(int status)
{
  switch (status)
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
    throw std::runtime_error("Unknown status code" + _to_string(status));
  }
}

std::string Response::get_error_page(std::string page, int code)
{
  std::string line("HTTP/1.1 ");
  line += _to_string(code) + " ";
  line += getStatusMsg(code) + "\n";
  std::string res("Content-Type: text/html\nContent-Length: ");
  res = line + res;

	std::ifstream iss;
	iss.open(page.c_str());
	if (!iss.is_open()) {
    std::cerr << "Cant open the file" << std::endl;
    throw System();
	}

  std::string content;
	std::getline(iss, content, '\0');
	iss.close();

  res += _to_string(content.length()) + "\n\n";
  res += content;
  return res;
}

void Response::bad_request(Client *cl)
{
  std::string res = get_error_page(cl->get_cluster()->get_config().error_pages.find(400)->second , 400);
  int i = send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
	if (i == FAIL || !i)
		cl->set_done_send(true);
}

void Response::payload_too_large(Client *cl)
{
  std::string res = get_error_page(cl->get_cluster()->get_config().error_pages.find(413)->second, 413);
  int i = send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
	if (i == FAIL || !i)
		cl->set_done_send(true);
}

void Response::not_found(Client *cl)
{
  std::string res = get_error_page(cl->get_cluster()->get_config().error_pages.find(404)->second, 404);
  int i = send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
	if (i == FAIL || !i)
		cl->set_done_send(true);
}

void Response::method_not_allowed(Client *cl)
{
  std::string res = get_error_page(cl->get_cluster()->get_config().error_pages.find(405)->second, 405);
  int i = send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
	if (i == FAIL || !i)
		cl->set_done_send(true);
}

bool Response::file_exists(const char *path)
{
  return (access(path, F_OK) == 0);
}

bool Response::isDirectory(const char *path)
{
  DIR *dir = opendir(path);
  if (dir != NULL)
  {
    closedir(dir);
    return true;
  }
  return false;
}

std::string set_cookies() {
	std::string cookie = rand_name();

	std::ifstream iss;
	iss.open("default.d/cookies");
	std::string buffer;
	while (std::getline(iss, buffer)) {
		if (cookie == buffer) {
			iss.close();
			return "";
		}
	}
	iss.close();

	std::fstream fs;
	fs.open("default.d/cookies", std::ios::app);
	fs << cookie << "\n";
	fs.close();

	return cookie;
}

void Response::to_string_get(Client *cl, std::string path)
{
  this->_header += "HTTP/1.1 ";
  this->_header += _to_string(this->_status_code) + " " + getStatusMsg(this->_status_code) + "\r\n";
  this->_header += std::string("Server: WebServ/1.0.0 (Unix)") + "\r\n";
  if (this->_status_code >= MOVED_PERMANENTLY and this->_status_code <= TEMPORARY_REDIRECT)
  {
    if (cl->get_location()->redir_path.empty())
      this->_header += "Location: " + cl->_req->get_url() + "\r\n";
    else
      this->_header += "Location: " + cl->get_location()->redir_path + "\r\n";
  }
	setBodySize(path);
  this->_header += std::string("Accept-Ranges: bytes") + "\r\n";
  this->_header += "Content-Type: " + cl->_req->getContentType(path) + "\r\n";
  this->_header += "Content-Length: " + _to_string(_body_size) + "\r\n";
	std::string cookie = set_cookies();
	if (cookie != "")
		this->_header += "Set-Cookies: " + cookie + "\r\n";
  this->_header += std::string("Connection: close") + "\r\n\r\n";
}

// this function calculates and
// assigns the size of a file to the _body_size
void Response::setBodySize(std::string &path)
{
  struct stat st;
  stat(path.c_str(), &st);
  this->_body_size = st.st_size;
}

void Response::handleFileRequest(Client *cl, std::string url)
{
  if (access(url.c_str(), F_OK) == -1)
  {
    this->_status_code = NOT_FOUND;
		not_found(cl);
  }
  else if (access(url.c_str(), R_OK) == -1)
  {
    this->_status_code = FORBIDDEN;
		std::string res = get_error_page(cl->get_cluster()->get_config().error_pages.find(403)->second, 403);
		int i = send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
		if (i == FAIL || !i)
			cl->set_done_send(true);
  }
  else
  {

    this->setBodySize(url);
		_status_code = 200;
		to_string_get(cl, url);
		get_body_content(url);
		std::string res = _header + _body;
		if (cl->done_cgi() && !cl->stats)
			return;
		if (cl->stats) {
			_header = "";
			to_string_get(cl, cl->filename);
			unlink(cl->filename.c_str());
			res = _header + _body;
		}
		int i = send(cl->get_connect_fd(), res.c_str(), res.size(), 0);
		if (i == FAIL || !i)
			cl->set_done_send(true);
  }
}

void parse_cgi_file(std::string filename) {
	std::ifstream iss;
	iss.open(filename.c_str());
	std::string buffer;
	std::getline(iss, buffer, '\0');
	iss.close();

	std::cout << "BEFORE: \n" << buffer << std::endl << std::endl;

	buffer = buffer.substr(buffer.find("<!DOCTYPE"));

	std::cout << "AFTER: \n" << buffer << std::endl << std::endl;

	std::ofstream oss;
	oss.open(filename.c_str(), std::ios::trunc);
	oss << buffer;
	oss.close();
}

void Response::get_body_content(std::string url) {
	std::string ex = url.substr(url.find_last_of(".") + 1);
	if (ex == "php" || ex == "py") {
		cgi_exec(url, _client);
		if (_client->stats) {
			parse_cgi_file(_client->filename);
			std::ifstream iss;
			iss.open(_client->filename.c_str());
			std::getline(iss, _body, '\0');
			iss.close();
		}
		return;
	}

	std::ifstream iss;
	iss.open(url.c_str(), std::ios::binary);

 	iss.seekg(0, std::ios::end);
  std::streampos fileSize = iss.tellg();
  iss.seekg(0, std::ios::beg);

  std::string Content(fileSize, '\0');
  iss.read(&Content[0], fileSize);
  iss.close();
	_body = Content;
}

//This code is handling a directory request
// It checks whether the server should perform auto-indexing or serve a default index file
void Response::handleDirectoryRequest(Client *cl, locations *var) {
  std::string url = final_url(cl);

	if (!var->def_files[0].empty()) {
		handleFileRequest(cl, url + var->def_files[0]);
	}	else if (var->autoindex) {
		auto_index(cl, url);
	}	else {
		this->_status_code = FORBIDDEN;
		std::string res = get_error_page(cl->get_cluster()->get_config().error_pages.find(403)->second, 403);
		int i = send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
		if (i == FAIL || !i)
			cl->set_done_send(true);
	}
}

void Response::auto_index(Client *client, std::string uri)
{
  DIR *dir;
  struct dirent *ent;

  std::string html = "<html><head><title>auto_index</title></head><body style=\"margin: 50px;\"><h1>Index of " + uri + "</h1><hr><br><br> <table style=\"width:60%\">";
  dir = opendir(uri.c_str());
  if (dir)
  {
    while ((ent = readdir(dir)) != NULL)
    {
      struct stat t_stat;
      int file_size;
      std::ifstream in_file(std::string(uri + ent->d_name).c_str(), std::ios::binary);
      html += "<tr><td><a href=\"" + std::string(ent->d_name);
      if (isDirectory((uri + ent->d_name).c_str()))
        html += "/";
      html += "\">" + std::string(ent->d_name) + "</a></td>";
      stat((uri + ent->d_name).c_str(), &t_stat);
      struct tm *time_info;
      time_info = localtime(&t_stat.st_ctime);
      in_file.seekg(0, std::ios::end);
      file_size = in_file.tellg();
      html += "<td>" + std::string(asctime(time_info)) + "</td><td>" + _to_string(file_size) + "</td>";
    }
    closedir(dir);
  }
  else
  {
    std::string res = get_error_page(client->get_cluster()->get_config().error_pages.find(501)->second, 501);
    return;
  }
  html += "</body> </html>";

  std::string res = "HTTP/1.1 200\r\nConnection: close\r\nContent-Length: " + _to_string(html.length()) + "\r\nContent-Type: text/html\r\n\r\n";
  int i = send(client->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
	if (i == FAIL || !i) {
		client->set_done_send(true);
		return;
	}
  i = send(client->get_connect_fd(), html.c_str(), strlen(html.c_str()), 0);
	if (i == FAIL || !i)
		client->set_done_send(true);
}

void Response::GET(Client *cl, locations *var)
{
	if (!var->redir_path.empty()) {
		_status_code = 301;
		std::string res = get_error_page(cl->get_cluster()->get_config().error_pages.find(301)->second, 301);
		int i = send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
		if (i == FAIL || !i)
			cl->set_done_send(true);
		return ;
	}

  std::string url = final_url(cl);
  if (isDirectory(url.c_str()))
    handleDirectoryRequest(cl, var);
  else
    handleFileRequest(cl, final_url(cl));
}

void Response::POST(Client *cl)
{
  std::string res = get_error_page(cl->get_cluster()->get_config().error_pages.find(201)->second, 201);
  int i = send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
	if (i == FAIL || !i)
		cl->set_done_send(true);
}

void Response::removeDirectory(const char *path)
{
  std::vector<std::string> stack;
  stack.push_back(path);

  while (!stack.empty())
  {
    const std::string currentPath = stack.back();
    stack.pop_back();

    DIR *dir = opendir(currentPath.c_str());
    if (!dir)
    {
      std::cerr << "Error opening directory: " << currentPath << std::endl;
      continue;
    }

    dirent *entry;
    while ((entry = readdir(dir)))
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

void Response::to_String_Delete(void)
{
  this->_header += "HTTP/1.1 ";
  this->_header += _to_string(this->_status_code) + " " + getStatusMsg(this->_status_code) + "\r\n";
  this->_header += std::string("Server: WebServ/1.0.0 (Unix)") + "\r\n";
  this->_header += std::string("Connection: close") + "\r\n\r\n";
}

std::string Response::final_url(Client *cl)
{
  size_t patternLen = cl->get_location()->pattern.length();
  std::string last_url = cl->get_location()->root + "/" +
                         cl->_req->get_url().substr(cl->_req->get_url().find(cl->get_location()->pattern) + patternLen);
	return last_url;
}

void Response::DELETE(Client *cl)
{
  std::string url = final_url(cl);
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
    res = get_error_page(cl->get_cluster()->get_config().error_pages.find(400)->second, 400);
  else
    this->to_String_Delete();
  res = get_error_page(cl->get_cluster()->get_config().error_pages.find(200)->second, 200);
  int i = send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
	if (i == FAIL || !i)
		cl->set_done_send(true);
}
