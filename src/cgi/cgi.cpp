#include "cgi.hpp"
#include "cgiParsing.hpp"
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>

// REFERENCE :( chapter 4:
// http://www.faqs.org/rfcs/rfc3875.html

const std::string	CGI_DIR = "/var/www/cgi-bin";
const size_t		BUFF_SIZE = 1024;

cgi::cgi() : _request(nullptr), _interpreter(""), _path("")
{
}

cgi::cgi(std::shared_ptr<Request> request,
		 std::string interpreter,
		 std::filesystem::path path) :
	_request(request), _interpreter(interpreter), _path(path)
{
}

cgi::~cgi()
{
}

// fd in added for request body:
std::string cgi::executeCGI(const std::string &args)
{
	int		pipefd_out[2];
	int		pipefd_in[2];
	int		status;
	int		bytes_read;
	pid_t	pid;
	char	buffer[BUFF_SIZE];
	int		exit_status;

	std::string result;

	if (args.empty())
		;

	cgiParsing vars(_request->get_headers(), environ, _request, _path,
					_interpreter); // parses all the magic
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
		// add argv variables: might have to be at the envpp
		for (const std::string &arg : vars.get_argv())
		{
			_argv.push_back(const_cast<char *>(arg.c_str()));
		}
		_argv.push_back(NULL);
		// add envp variables:
		for (const std::string &arg : vars.get_envp())
		{
			_envp.push_back(const_cast<char *>(arg.c_str()));
		}
		_envp.push_back(NULL);
		// LAUNCH
		execve(_argv.data()[0], _argv.data(), _envp.data());
		std::cout << std::strerror(errno) << std::endl;
		throw "Exec failed";
	}
	else
	{
		close(pipefd_out[1]);
		close(pipefd_in[0]);
		write(pipefd_in[1], vars.get_stdin().c_str(),
			vars.get_stdin().length());
		close(pipefd_in[1]);
		// for chunking check if body is empty otherwise keep.
		while ((bytes_read = read(pipefd_out[0], buffer, sizeof(buffer)
					- 1)) > 0)
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

// std::vector<char *> cgi::createEnv(std::string name, std::string value,
// 	std::string additive)
// {
// 	_envp.push_back();
// }

std::vector<char *> &cgi::get_argv()
{
	return (_argv);
}

std::vector<char *> &cgi::get_envp()
{
	return (_envp);
}
