#pragma once

#include "request.hpp"
#include <filesystem>
#include <memory>
#include <vector>

class cgi
{
  public:
	cgi();
	cgi(std::shared_ptr<Request> request, std::string interpreter,
		 std::filesystem::path path);
	~cgi();

	void createArgs(std::vector<char *> &argv, std::string &path,
		std::string &args);

	bool createEnv(char **environ);

	bool add_to_env(std::string name, std::string value, std::string prefix);
	bool validate_key(std::string key);
	bool addToArgv(std::string name, std::string value, std::string additive);

	std::string executeCGI();

	std::vector<std::string> &getArgv();
	std::vector<char *> getCstrArgv();
	std::vector<std::string> &getEnvp();
	std::vector<char *> getCstrEnvp();

  private:
	std::shared_ptr<Request> _request;
	std::string _interpreter;
	std::filesystem::path _path;
	std::vector<std::string> _argv;
	std::vector<std::string> _envp;

	static const inline std::vector<std::string> _metaVarNames = {
    "AUTH_TYPE",      "CONTENT_LENGTH",  "CONTENT_TYPE", "GATEWAY_INTERFACE",
    "PATH_INFO",      "PATH_TRANSLATED", "QUERY_STRING", "REMOTE_ADDR",
    "REMOTE_HOST",    "REMOTE_IDENT",    "REMOTE_USER",  "REQUEST_METHOD",
    "SCRIPT_NAME",    "SERVER_NAME",     "SERVER_PORT",  "SERVER_PROTOCOL",
    "SERVER_SOFTWARE"};

	static inline std::vector<std::string> _acceptedPrefixes = {"X_", "HTTP_"};
};
