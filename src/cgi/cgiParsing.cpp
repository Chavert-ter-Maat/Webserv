// #include "cgiParsing.hpp"
// #include "cgi.hpp"
// #include <algorithm>
// #include <iostream>
// #include <vector>
//
// std::vector<std::string> meta_variables_names = {
//     "AUTH_TYPE",      "CONTENT_LENGTH",  "CONTENT_TYPE", "GATEWAY_INTERFACE",
//     "PATH_INFO",      "PATH_TRANSLATED", "QUERY_STRING", "REMOTE_ADDR",
//     "REMOTE_HOST",    "REMOTE_IDENT",    "REMOTE_USER",  "REQUEST_METHOD",
//     "SCRIPT_NAME",    "SERVER_NAME",     "SERVER_PORT",  "SERVER_PROTOCOL",
//     "SERVER_SOFTWARE"};
//
// bool cgiParsing::dismantle_body(std::string body, std::string boundary) {
//   std::string contentDisposition;
//   std::string contentType;
//
//   if (boundary.empty())
//     ;
//   this->_body = body;
//   return (false);
// }
//
// //$_GET['name'] = hoi php file is argv name=hoi :: $_SERVER['name'] = doei php
// // file is env var name=doei :: educated guess is that _POST['var'] = something
// // goes into the stdin var=something
// cgiParsing::cgiParsing(
//     std::unordered_map<std::string, std::string> headers, char **environ,
//     std::shared_ptr<Request> _request, const std::string &path,
//     const std::string &interpreter) // ServerStruct &serverinfo
// {
//
//   if (environ)
//     ;
//   _customVarNames.push_back("X_");
//   _customVarNames.push_back("");
//
//
//   if (!_request->get_requestMethod().compare("POST")) {
//     add_to_envpp("REQUEST_METHOD", _request->get_requestMethod(), "");
//     add_to_envpp("GATEWAY_INTERFACE", "CGI/1.1", "");
//   }
//   add_to_envpp("REDIRECT_STATUS", "true", "");
//   add_to_envpp("SCRIPT_FILENAME", path, "");
//   add_to_envpp("BOUNDARY", _request->get_boundary(), "");
//
//   for (const auto &[key, value] : headers)
//     add_to_envpp(key, value, "");
//
//   if (interpreter.compare(""))
//     add_to_uri(interpreter, "", "");
//
//   add_to_uri(path, "", "");
//
//   for (const auto &var : _request->get_requestArgs())
//     add_to_uri(var, "", "");
//
//   dismantle_body(_request->get_body(), _request->get_boundary());
// }
//
// cgiParsing::~cgiParsing(void) {}
//
//
//
// bool cgiParsing::validate_key(std::string key,
//                               std::vector<std::string> customVarNames) {
//   int i;
//
//   for (auto &c : key)
//     c = toupper(c);
//   i = meta_variables_names.size();
//   while (i--)
//     if (!meta_variables_names[i].compare(key))
//       return true;
//   i = customVarNames.size();
//   while (i--)
//     if (key.find(customVarNames[i]) != std::string::npos)
//       return true;
//   return false;
// }
//
// bool cgiParsing::add_to_uri(std::string name, std::string value,
//                             std::string additive) {
//   std::string temp;
//
//   temp = additive + name;
//   if (value.compare(""))
//     temp += "=" + value;
//   _uri.push_back(temp); // uri[name] = value;
//   return true;
// }
//
// std::vector<std::string> &cgiParsing::get_argv() { return (_uri); }
//
// std::vector<std::string> &cgiParsing::get_envp() { return (_envp); }
//
// std::string &cgiParsing::get_stdin() { return (_body); }
