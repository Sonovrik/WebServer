//
// Created by Kory Moaning on 3/2/21.
//

#include "CGI.hpp"

int main(){
//	Request req;
//	std::string tmp = "POST /cgi-bin/process.cgi HTTP/1.1\n\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\n\nHost: www.example.com\n\nContent-Type: application/x-www-form-urlencoded\n\n";
//	std::string tmp2 = "POST / HTTP/1.1\r\n";
//	std::string tmp3 = "GET /index.html HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nUser-Agent: curl/7.47.0\r\n    Accept: */*    \r\n\r\n";
//	req = parse_request(tmp3);
	// if (req.parse_start_line(tmp2))
	// 	std::cout << "good" << std::endl;

	CGI qqq;
	qqq.init();
	qqq.creatENV();
	qqq.exec();

	return 0;
}
