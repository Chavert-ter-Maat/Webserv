#pragma once

#include "request.hpp"
#include <filesystem>
#include <memory>
#include <vector>

class cgi
{
  public:
	cgi();
	cgi(std::shared_ptr<Request> request,
		 std::string interpreter,
		 std::filesystem::path path);
	~cgi();

	void createArgs(std::vector<char *> &argv, std::string &path,
		std::string &args);

	std::vector<char *> createEnv(std::string, std::string value,
		std::string additive);

	std::string executeCGI(const std::string &args);

	std::vector<char *> &get_argv();
	std::vector<char *> &get_envp();

  private:
	std::shared_ptr<Request> _request;
	std::string _interpreter;
	std::filesystem::path _path;
	std::vector<char *> _argv;
	std::vector<char *> _envp;
};
