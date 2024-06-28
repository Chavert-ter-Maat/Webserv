#include "cgi.hpp"
#include "defines.h"
#include "dir_listing.hpp"
#include "response.hpp"
#include <algorithm>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>

#define KEEP_ALIVE_TIMOUT 10
#define KEEP_ALIVE_N 100
const std::string UPLOAD_DIR = "./html/uploads/";

Response::Response() : _request(nullptr), _responseString(""), _contentType(""), _body(""), _isCGI(false)
{
}

Response::Response(std::shared_ptr<Request> request) : _request(request),
	_responseString(""), _contentType(""), _body(""), _isCGI(false)
{
	handleRequest();
	printResponse();
}

Response::~Response(){};

Response::Response(const Response &src) : _request(src._request),
	_responseString(src._responseString), _contentType(src._contentType), _body(src._body), _isCGI(src._isCGI)
{
}

Response &Response::operator=(const Response &rhs)
{
	Response temp(rhs);
	temp.swap(*this);
	return (*this);
}

<<<<<<< HEAD
void Response::swap(Response &lhs)
{
	std::swap(_request, lhs._request);
	std::swap(_responseString, lhs._responseString);
	std::swap(_contentType, lhs._contentType);
	std::swap(_body, lhs._body);
	std::swap(_isCGI, lhs._isCGI);
}

void Response::handleRequest()
{
	std::string request_method = _request->get_requestMethod();
	try
	{
		if (request_method == "GET")
			handleGetRequest();
		else if (request_method == "POST")
			handlePostRequest();
		else if (request_method == "DELETE")
			handleDeleteRequest();
		else
			buildResponse(static_cast<int>(StatusCode::METHOD_NOT_ALLOWED),
				"Method Not Allowed", "");
	}
	catch (const std::exception &e)
	{
		buildResponse(static_cast<int>(StatusCode::INTERNAL_SERVER_ERROR),
			"Internal Server Error", "");
	}

/* void Response::handleRequest() {
	int		return_code = 0;
  std::string request_method = request->get_requestMethod();
  try {
	request_path = security.isFilePermissioned(request->get_uri(), return_code);
	if (return_code)
		request_path = security.getErrorPage(return_code); // wrong place
	std::cout << "\nPATH:" << request_path << std::endl;
    if (request_method == "GET")
      handleGetRequest(request);
    else if (request_method == "POST")
      handlePostRequest(request);
    else if (request_method == "DELETE")
      handleDeleteRequest(request);
    else
      buildResponse(static_cast<int>(StatusCode::METHOD_NOT_ALLOWED),
                    "Method Not Allowed", "");
  } catch (const std::exception &e) {
    buildResponse(static_cast<int>(StatusCode::INTERNAL_SERVER_ERROR),
                  "Internal Server Error", "");
  }
} */

bool Response::handleGetRequest()
{
	std::stringstream buffer;
	std::filesysm::path path = "html/";
	// std::filesystem::path resourcePath = _request->get_uri();


	if (!request->getPath.empty() && resourcePath.has_extension())
	{
		auto it = _contentTypes.find(resourcePath.extension());
		if (it == _contentTypes.end())
		{
			_responseString = buildResponse(static_cast<int>(StatusCode::UNSUPPORTED_MEDIA_TYPE),
					"Unsupported Media Type", "");
			return (false);
		}
		_contentType = it->second;
		if (_interpreters.find(resourcePath.extension()) == _interpreters.end())
		{
			// path.append(resourcePath.string());
			std::ifstream file(path, std::ios::binary);
			if (!file)
			{
				_responseString = buildResponse(static_cast<int>(StatusCode::NOT_FOUND),
						"Not Found", "");
				return (false);
			}
			buffer << file.rdbuf();
			_body = buffer.str();
		}
		else
		{
			_isCGI = true;
			// path.append(resourcePath.string());
			cgi CGI(_request, _interpreters.at(resourcePath.extension()), path);
			_body = CGI.executeCGI();
		}
	}
	else // dir listing on?
	{
		// path.append(resourcePath.string());
		_body = list_dir(path, _request->get_uri(), _request->get_referer());
	}
	_responseString = buildResponse(static_cast<int>(StatusCode::OK), "OK",
			_body, _isCGI);
	return (true);
}

bool Response::handlePostRequest()
{
	std::string requestBody = _request->get_body();
	std::string requestContentType = _request->get_contentType();
	std::filesystem::path resourcePath = _request->get_uri();
	std::filesystem::path path = "html/";

	if (!resourcePath.empty() && resourcePath.string()[0] == '/')
		resourcePath = resourcePath.string().substr(1);
	if (resourcePath.empty())
		resourcePath = "index.html";
	std::cout << "ReourcePath:" << resourcePath << std::endl;
	if (resourcePath.has_extension())
	{
		if (_interpreters.find(resourcePath.extension()) != _interpreters.end())
		{
			_isCGI = true;
			path.append(resourcePath.string());
			cgi CGI(_request, _interpreters.at(resourcePath.extension()), path);
			_body = CGI.executeCGI();
		}
		else
		{
			buildResponse(static_cast<int>(StatusCode::NO_CONTENT),
				"No Content", "");
			return (true);
		}
	}
	if (requestContentType == "multipart/form-data")
	{
		handle_multipart();
		return (true);
	}
	buildResponse(static_cast<int>(StatusCode::OK), "OK", _body, _isCGI);
	return (true);
}

void Response::handle_multipart()
{
	size_t	pos;
	size_t	start;
	size_t	end;
	size_t	header_end;
	size_t	filename_pos;
	size_t	filename_end;

	std::string body = _request->get_body();
	std::string boundary = _request->get_boundary();
	std::cout << "<multipart/form-data>" << std::endl;
	pos = 0;
	while (pos < body.size())
	{
		start = std::search(body.begin() + pos, body.end(), boundary.begin(),
				boundary.end()) - body.begin();
		if (start == body.size())
			break ;
		end = std::search(body.begin() + start + boundary.length(), body.end(),
				boundary.begin(), boundary.end()) - body.begin();
		if (end == body.size())
			break ;
		std::string part(body.begin() + start + boundary.length(), body.begin()
			+ end);
		header_end = part.find("\r\n\r\n");
		std::string headers = part.substr(0, header_end);
		std::string content = part.substr(header_end + 4);
		filename_pos = headers.find("filename=\"");
		if (filename_pos != std::string::npos)
		{
			filename_end = headers.find("\"", filename_pos + 10);
			std::string filename = headers.substr(filename_pos + 10,
					filename_end - filename_pos - 10);
			std::cout << "filename: " << filename << std::endl;
			std::cout << "content: " << content << std::endl;
			std::ofstream file(UPLOAD_DIR + filename, std::ios::binary);
			if (file.is_open())
			{
				file.write(content.c_str(), content.length());
				file.close();
				buildResponse(static_cast<int>(StatusCode::OK),
					"File uploaded succesfully!", "");
				// std::cout << "Content-Type: text/html/r/n/r/n";
				// std::cout << "<html><body>File uploaded
				// succesfully!</body></html>";
			}
			else
			{
				// std::cout << "Content-Type: text/html/r/n/r/n";
				// std::cout << "<html><body>Error: file upload
				// failed!</body></html>";
				buildResponse(static_cast<int>(StatusCode::INTERNAL_SERVER_ERROR),
					"Error: file upload failed!", "");
			}
		}
		pos = end;
	}
}

bool Response::handleDeleteRequest()
{
	std::filesystem::path path = _request->get_uri();
	buildResponse(static_cast<int>(StatusCode::OK), "OK", _request->get_body());
	return (true);
}

std::unordered_map<std::string,
	std::string> Response::get_args(std::string requestBody,
	std::string contentType)
{
	size_t	pos;
	size_t	boundaryPos;
	size_t	cdPos;
	size_t	namePos;
	size_t	nameEnd;
	size_t	filenamePos;
	size_t	filenameEnd;

	std::unordered_map<std::string, std::string> args;
	if (contentType == "application/x-www-form-urlencoded")
	{
		std::string token;
		std::istringstream tokenStream(requestBody);
		while (std::getline(tokenStream, token, '&'))
		{
			pos = token.find('=');
			if (pos != std::string::npos)
			{
				std::string key = token.substr(0, pos);
				std::string value = token.substr(pos + 1);
				args[key] = value;
			}
		}
	}
	else if (contentType == "multipart/form-data")
	{
		boundaryPos = contentType.find("boundary=");
		if (boundaryPos == std::string::npos)
			return (args);
		std::string boundary = contentType.substr(boundaryPos + 9);
		std::vector<std::string> parts = get_parts(requestBody, boundary);
		if (parts.empty())
			return (args);
		for (const std::string &part : parts)
		{
			if ((cdPos = part.find("content-disposition: for-data;")) == std::string::npos)
				return (args); // error ?
			namePos = part.find("name=\"");
			if (namePos == std::string::npos)
				return (args); // error ?
			nameEnd = part.find("\"", namePos);
			if (nameEnd == std::string::npos)
				return (args); // error ?
			std::string name = part.substr(namePos + 6, nameEnd - namePos - 6);
			std::string filename;
			filenamePos = part.find("filename=\"", cdPos + 31);
			if (filenamePos != std::string::npos)
			{
				filenameEnd = part.find("\"", filenamePos + 10);
				if (filenameEnd != std::string::npos)
					filename = part.substr(filenamePos + 10, filenameEnd
							- filenamePos - 10);
			}
		}
	}
	return (args);
}

std::vector<std::string> Response::get_parts(std::string requestBody,
	std::string boundary)
{
	size_t	pos;
	size_t	start;

	pos = 0;
	std::vector<std::string> parts;
	while ((pos = requestBody.find("--" + boundary, pos)) != std::string::npos)
	{
		start = pos + boundary.length() + 4;
		pos = requestBody.find("--" + boundary, start);
		if (pos != std::string::npos)
			parts.push_back(requestBody.substr(start, pos - start - 2));
	}
	return (parts);
}

std::string Response::buildResponse(int status, const std::string &message,
	const std::string &body, bool isCGI)
{
	_responseString.append("HTTP/1.1 " + std::to_string(status) + " " + message
		+ "\r\n");
	if (!body.empty() && !isCGI)
	{
		_responseString.append("Content-Length: "
			+ std::to_string(body.length()) + "\r\n");
	}
	if (_request->get_keepAlive())
	{
		_responseString.append("Keep-Alive: timeout="
			+ std::to_string(KEEP_ALIVE_TIMOUT) + ", max="
			+ std::to_string(KEEP_ALIVE_N) + "\r\n");
	}
	if (isCGI)
	{
		_responseString.append(body);
	}
	else
	{
		_responseString.append("Content-Type: " + get_contentType() + "\r\n");
		_responseString.append("\r\n" + body);
	}
	return (_responseString);
}

std::string Response::get_response()
{
	return (_responseString);
}
std::string Response::get_contentType()
{
	return (_contentType);
}

void Response::printResponse()
{
	std::cout << MSG_BORDER << "[Response]" << MSG_BORDER << std::endl;
	std::cout << _responseString << std::endl;
}
