#include "Request/Request.hpp"
#include "Response.hpp"
#include "Server.hpp"

// success -> 201 (Created)

// 303 (See Other)

// almost all of the status codes defined by this specification might be received in a response to POST (except these 206 (Partial Content) 304 (Not Modified) 416 (Range Not Satisfiable))

// If the result of processing a POST would be equivalent to a
//    representation of an existing resource, an origin server MAY redirect
//    the user agent to that resource by sending a 303 (See Other) response
//    with the existing resource's identifier in the Location field.

int main() {
	Request request;
	parseRequest("POST / HTTP/1.1\r\nHost:123\r\n\r\nhello\r\n", request);

	Response response;

}