#include "cgi.hpp"
#include "cgiParsing.hpp"
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>

// REFERENCE :( chapter 4:
// http://www.faqs.org/rfcs/rfc3875.html

const std::string CGI_DIR = "/var/www/cgi-bin";
const size_t	BUFF_SIZE = 1024;

cgi::cgi() : _request(nullptr), _interpreter(""), _path("")
{
}

cgi::cgi(std::shared_ptr<Request> request, std::string interpreter,
	std::filesystem::path path) : _request(request), _interpreter(interpreter),
	_path(path)
{
	createEnv(environ);
}

cgi::~cgi()
{
}

// fd in added for request body:
std::string cgi::executeCGI()
{
	int		pipefd_in[2];
	int		pipefd_out[2];
	int		status;
	int		bytes_read;
	pid_t	pid;
	char	buffer[BUFF_SIZE];
	int		exit_status;
	std::string result;

	
	if (pipe(pipefd_out) == -1 || pipe(pipefd_in) == -1)
	{
		throw "Pipe Failed";
	}

	pid = fork();
	if (pid < 0)
	{
		throw "Fork Failed";
	}
	else if (pid == 0)
	{
		// stdin of request body:
		close(pipefd_out[0]);
		close(pipefd_in[1]);
		dup2(pipefd_out[1], STDOUT_FILENO);
		dup2(pipefd_in[0], STDIN_FILENO);
		close(pipefd_out[1]);
		close(pipefd_in[0]);

		// convert _argv to cstring-argv
		std::vector<char *>cstrArgv = getCstrArgv();

		// add envp variables:
		std::vector<char *>cstrEnvp = getCstrEnvp();

		// add argv variables: might have to be at the envpp
		// for (const std::string &arg : _request->get_requestArgs())
		// {
		// 	_argv.push_back(const_cast<char *>(arg.c_str()));
		// }
		// _argv.push_back(NULL);

		// LAUNCH
		execve(_path.c_str(), cstrArgv.data(), cstrEnvp.data());
		std::cout << std::strerror(errno) << std::endl;
		throw "Exec failed";
	}
	else
	{
		close(pipefd_out[1]);
		close(pipefd_in[0]);
		write(pipefd_in[1], _request->get_body().c_str(), _request->get_body().length());
		close(pipefd_in[1]);

		// for chunking check if body is empty otherwise keep.
		while ((bytes_read = read(pipefd_out[0], buffer, sizeof(buffer) - 1)) > 0)
		{
			std::string part(buffer, bytes_read);
			result.append(part);
		}
		if (bytes_read == -1)
		{
			throw "Read failed";
		}
		close(pipefd_out[0]);

		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
		{
			exit_status = WEXITSTATUS(status);
			if (exit_status != 0)
			{
				std::cout << "exit_status: " << exit_status << std::endl;
			}
		}
		return (result);
	}
}

bool cgi::createEnv(char **environ)
{
	(void)environ;
	add_to_env("REQUEST_METHOD", _request->get_requestMethod(), "");
	add_to_env("SERVER_PROTOCOL", "HTTP/1.1", "");
	add_to_env("SCRIPT_FILENAME", _path, "");
	if (!_request->get_boundary().empty())
		add_to_env("BOUNDARY", _request->get_boundary(), "");
	add_to_env("GATEWAY_INTERFACE", "CGI/1.1", "");
	add_to_env("REDIRECT_STATUS", "true", "");


	// for (const auto &var : _request->get_requestArgs())
	// 	add_to_uri(var, "", "");

	// add_to_uri(_path, "", "");

	// if (_interpreter.compare(""))
	// 	add_to_uri(_interpreter, "", "");

	for (const auto &[key, value] : _request->get_headers())
		add_to_env(key, value, "");
	// dismantle_body(_request->get_body(), _request->get_boundary());
	return (true);
}

bool cgi::add_to_env(std::string name, std::string value, std::string prefix)
{
	std::string newVar;
	std::vector<std::string> customVarNames;

	if (validate_key(prefix + name))
	{
		newVar = prefix + name;
		for (auto &c : newVar)
			c = toupper(c);
		if (value.compare(""))
			newVar += "=" + value;
		std::replace(newVar.begin(), newVar.end(), '-', '_');
		_envp.push_back(newVar);
		return (true);
	}
	return (false);
}

bool cgi::validate_key(std::string key)
{
	for (auto &c : key)
		c = toupper(c);

	auto it = std::find(_metaVarNames.begin(), _metaVarNames.end(), key);
	if (it == _metaVarNames.end())
		return (true);

	for (auto &prefix: _acceptedPrefixes)
	{
		if (key.find(prefix) == 0)
			return (true);
	}
	return (false);
}

bool cgi::addToArgv(std::string name, std::string value,
	std::string additive)
{
	std::string temp;
	temp = additive + name;
	if (value.compare(""))
		temp += "=" + value;
	_argv.push_back(temp); // uri[name] = value;
	return (true);
}

std::vector<std::string> &cgi::getArgv()
{
	return (_argv);
}

std::vector<std::string> &cgi::getEnvp()
{
	return (_envp);
}

std::vector<char *> cgi::getCstrArgv()
{
	std::vector<char*> cstrArgv;

	for(const auto& var : _argv)
    	cstrArgv.push_back(const_cast<char*>(var.c_str()));
	return (cstrArgv);
}

std::vector<char *> cgi::getCstrEnvp()
{
	std::vector<char*> cstrEnvp;

	for(const auto& var : _envp)
    	cstrEnvp.push_back(const_cast<char*>(var.c_str()));
	return (cstrEnvp);
}
