#include <istream>
#include "utils.hpp"

// Bad Request
static std::string error_400(void) {
	return "<html>\n<title>Error 400</title>\n<body>\n<h1>400</h1>\n<h2>Bad Request</h2>\n</body>\n</html>";
}

// Forbidden
static std::string error_403(void) {
	return "<html>\n<title>Error 403</title>\n<body>\n<h1>403</h1>\n<h2>Payment Required</h2>\n</body>\n</html>";
}

// Not Found
static std::string error_404(void) {
	return "<html>\n<title>Error 404</title>\n<body>\n<h1>404</h1>\n<h2>Not Found</h2>\n</body>\n</html>";
}

// Method Not Allowed
static std::string error_405(void) {
	return "<html>\n<title>Error 405</title>\n<body>\n<h1>405</h1>\n<h2>Method Not Allowed</h2>\n</body>\n</html>";
}

// Not Acceptable
static std::string error_406(void) {
	return "<html>\n<title>Error 406</title>\n<body>\n<h1>406</h1>\n<h2>Not Acceptable</h2>\n</body>\n</html>";
}

// Request Timeout
static std::string error_408(void) {
	return "<html>\n<title>Error 408</title>\n<body>\n<h1>408</h1>\n<h2>Request Timeout</h2>\n</body>\n</html>";
}

// Conflict
static std::string error_409(void) {
	return "<html>\n<title>Error 409</title>\n<body>\n<h1>409</h1>\n<h2>Conflict</h2>\n</body>\n</html>";
}

// Gone - access to the target resource is no longer available at the origin server and that this condition is likely to be permanent
static std::string error_410(void) {
	return "<html>\n<title>Error 410</title>\n<body>\n<h1>410</h1>\n<h2>Gone</h2>\n</body>\n</html>";
}

// Length Required
static std::string error_411(void) {
	return "<html>\n<title>Error 411</title>\n<body>\n<h1>411</h1>\n<h2>Length Required</h2>\n</body>\n</html>";
}

// Payload Too Large
static std::string error_413(void) {
	return "<html>\n<title>Error 413</title>\n<body>\n<h1>413</h1>\n<h2>Payload Too Large</h2>\n</body>\n</html>";
}

// URI Too Long
static std::string error_414(void) {
	return "<html>\n<title>Error 414</title>\n<body>\n<h1>414</h1>\n<h2>URI Too Long</h2>\n</body>\n</html>";

}

// Unsupported Media Type
static std::string error_415(void) {
	return "<html>\n<title>Error 415</title>\n<body>\n<h1>415</h1>\n<h2>URI Too Long</h2>\n</body>\n</html>";
}

// Expectation Failed
static std::string error_417(void) {
	return "<html>\n<title>Error 417</title>\n<body>\n<h1>417</h1>\n<h2>Expectation Failed</h2>\n</body>\n</html>";
}


// Upgrade Required (Requirde: header must be sent)
static std::string error_426(void) {
	return "<html>\n<title>Error 426</title>\n<body>\n<h1>426</h1>\n<h2>Upgrade Required</h2>\n</body>\n</html>";
}

// Internal Server Error
static std::string error_500(void) {
	return "<html>\n<title>Error 500</title>\n<body>\n<h1>500</h1>\n<h2>Internal Server Error</h2>\n</body>\n</html>";
}
// Not Implemented
static std::string error_501(void) {
	return "<html>\n<title>Error 501</title>\n<body>\n<h1>501</h1>\n<h2>Not Implemented</h2>\n</body>\n</html>";
}
// Bad Gateway
static std::string error_502(void) {
	return "<html>\n<title>Error 502</title>\n<body>\n<h1>502</h1>\n<h2>Bad Gateway</h2>\n</body>\n</html>";
}
// Service Unavailable
static std::string error_503(void) {
	return "<html>\n<title>Error 503</title>\n<body>\n<h1>503</h1>\n<h2>Service Unavailable</h2>\n</body>\n</html>";
}
// Gateway Timeout
static std::string error_504(void) {
	return "<html>\n<title>Error 504</title>\n<body>\n<h1>504</h1>\n<h2>Gateway Timeout</h2>\n</body>\n</html>";
}

//HTTP Version Not Supported
static std::string error_505(void) {
	return "<html>\n<title>Error 505</title>\n<body>\n<h1>505</h1>\n<h2>HTTP Version Not Supported</h2>\n</body>\n</html>";
}

std::string	get_errorPage(int code) {
	switch(code) {
		case 400 : return error_400();
		case 403 : return error_403();
		case 405 : return error_405();
		case 406 : return error_406();
		case 408 : return error_408();
		case 409 : return error_409();
		case 410 : return error_410();
		case 411 : return error_411();
		case 413 : return error_413();
		case 414 : return error_414();
		case 415 : return error_415();
		case 417 : return error_417();
		case 426 : return error_426();
		case 500 : return error_500();
		case 501 : return error_501();
		case 502 : return error_502();
		case 503 : return error_503();
		case 504 : return error_504();
		case 505 : return error_505();
	}
	return "";
}