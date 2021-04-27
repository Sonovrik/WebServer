#include "utils.hpp"

void	initErrors(std::vector<Server> &serverList){

	std::vector<Server>::iterator itServ = serverList.begin();
	for (; itServ != serverList.end(); itServ++){
		std::map<int, std::string> errorsNames;
		errorsNames.insert(std::make_pair(400, "Bad Request"));
		errorsNames.insert(std::make_pair(401, "Unauthorized"));
		errorsNames.insert(std::make_pair(403, "Forbidden"));
		errorsNames.insert(std::make_pair(404, "Not Found"));
		errorsNames.insert(std::make_pair(405, "Method Not Allowed"));
		errorsNames.insert(std::make_pair(406, "Not Acceptable"));
		errorsNames.insert(std::make_pair(408, "Request Timeout"));
		errorsNames.insert(std::make_pair(409, "Conflict"));
		errorsNames.insert(std::make_pair(410, "Gone"));
		errorsNames.insert(std::make_pair(411, "Length Required"));
		errorsNames.insert(std::make_pair(413, "Payload Too Large"));
		errorsNames.insert(std::make_pair(414, "URI Too Long"));
		errorsNames.insert(std::make_pair(415, "Unsupported Media Type"));
		errorsNames.insert(std::make_pair(417, "Expectation Failed"));
		errorsNames.insert(std::make_pair(426, "Upgrade Required"));
		errorsNames.insert(std::make_pair(500, "Internal Server Error"));
		errorsNames.insert(std::make_pair(501, "Not Implemented"));
		errorsNames.insert(std::make_pair(502, "Bad Gateway"));
		errorsNames.insert(std::make_pair(503, "Service Unavailable"));
		errorsNames.insert(std::make_pair(504, "Gateway Timeout"));
		errorsNames.insert(std::make_pair(505, "HTTP Version Not Supported"));

		std::map<int, std::string> &servErrors = itServ->get_ErrorPaths();
		
		std::map<int, std::string>::iterator it = errorsNames.begin();
		for (; it != errorsNames.end(); it++){
			if (servErrors.find(it->first) == servErrors.end()){
				std::string page = "<html>\n<title>Error " + to_string(it->first)
					+ "</title>\n<body>\n<h1>" + to_string(it->first)
					+ "</h1>\n<h2>" + it->second
					+ "</h2>\n</body>\n</html>";
				servErrors.insert(std::make_pair(it->first, page));
			}
		}
	}
}
