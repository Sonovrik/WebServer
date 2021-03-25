#include "utils.hpp"

static std::map<std::string, std::string> types;

void	initMimeTypes(void){
	types.insert(std::make_pair(".aac", "audio/aac"));
	types.insert(std::make_pair(".abw", "application/x-abiword"));
	types.insert(std::make_pair(".arc", "application/x-freearc"));
	types.insert(std::make_pair(".avi", "video/x-msvideo"));
	types.insert(std::make_pair(".azw", "application/vnd.amazon.ebook"));
	types.insert(std::make_pair(".bin", "application/octet-stream"));
	types.insert(std::make_pair(".bmp", "image/bmp"));
	types.insert(std::make_pair(".bz", "application/x-bzip"));
	types.insert(std::make_pair(".bz2", "application/x-bzip2"));
	types.insert(std::make_pair(".csh", "application/x-csh"));
	types.insert(std::make_pair(".css", "text/css"));
	types.insert(std::make_pair(".csv", "text/csv"));
	types.insert(std::make_pair(".doc", "application/msword"));
	types.insert(std::make_pair(".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"));
	types.insert(std::make_pair(".eot", "application/vnd.ms-fontobject"));
	types.insert(std::make_pair(".epub", "application/epub+zip"));
	types.insert(std::make_pair(".gz", "application/gzip"));
	types.insert(std::make_pair(".gif", "image/gif"));
	types.insert(std::make_pair(".htm", "text/html"));
	types.insert(std::make_pair(".html", "text/html"));
	types.insert(std::make_pair(".ico", "image/vnd.microsoft.icon"));
	types.insert(std::make_pair(".ics", "text/calendar"));
	types.insert(std::make_pair(".jar", "application/java-archive"));
	types.insert(std::make_pair(".jpeg", "image/jpeg"));
	types.insert(std::make_pair(".jpg", "image/jpeg"));
	types.insert(std::make_pair(".js", "text/javascript"));
	types.insert(std::make_pair(".json", "application/json"));
	types.insert(std::make_pair(".jsonld", "application/ld+json"));
	types.insert(std::make_pair(".mid", "audio/midi"));
	types.insert(std::make_pair(".midi", "audio/x-midi"));
	types.insert(std::make_pair(".mjs", "text/javascript"));
	types.insert(std::make_pair(".mp3", "audio/mpeg"));
	types.insert(std::make_pair(".mpeg", "video/mpeg"));
	types.insert(std::make_pair(".mpkg", "application/vnd.apple.installer+xml"));
	types.insert(std::make_pair(".odp", "application/vnd.oasis.opendocument.presentation"));
	types.insert(std::make_pair(".ods", "application/vnd.oasis.opendocument.spreadsheet"));
	types.insert(std::make_pair(".odt", "application/vnd.oasis.opendocument.text"));
	types.insert(std::make_pair(".oga", "audio/ogg"));
	types.insert(std::make_pair(".ogv", "video/ogg"));
	types.insert(std::make_pair(".ogx", "application/ogg"));
	types.insert(std::make_pair(".opus", "audio/opus"));
	types.insert(std::make_pair(".otf", "font/otf"));
	types.insert(std::make_pair(".png", "image/png"));
	types.insert(std::make_pair(".pdf", "application/pdf"));
	types.insert(std::make_pair(".php", "text/html"));
	types.insert(std::make_pair(".ppt", "application/vnd.ms-powerpoint"));
	types.insert(std::make_pair(".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"));
	types.insert(std::make_pair(".rar", "application/vnd.rar"));
	types.insert(std::make_pair(".rtf", "application/rtf"));
	types.insert(std::make_pair(".sh", "application/x-sh"));
	types.insert(std::make_pair(".svg", "image/svg+xml"));
	types.insert(std::make_pair(".swf", "application/x-shockwave-flash"));
	types.insert(std::make_pair(".tar", "application/x-tar"));
	types.insert(std::make_pair(".tif", "image/tiff"));
	types.insert(std::make_pair(".tiff", "image/tiff"));
	types.insert(std::make_pair(".ts", "video/mp2t"));
	types.insert(std::make_pair(".ttf", "font/ttf"));
	types.insert(std::make_pair(".txt", "text/plain"));
	types.insert(std::make_pair(".vsd", "application/vnd.visio"));
	types.insert(std::make_pair(".wav", "audio/wav"));
	types.insert(std::make_pair(".weba", "audio/webm"));
	types.insert(std::make_pair(".webm", "video/webm"));
	types.insert(std::make_pair(".webp", "image/webp"));
	types.insert(std::make_pair(".woff", "font/woff"));
	types.insert(std::make_pair(".woff2", "font/woff2"));
	types.insert(std::make_pair(".xhtml", "application/xhtml+xml"));
	types.insert(std::make_pair(".xls", "application/vnd.ms-excel"));
	types.insert(std::make_pair(".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
	types.insert(std::make_pair(".xml", "text/xml"));
	types.insert(std::make_pair(".xul", "application/vnd.mozilla.xul+xml"));
	types.insert(std::make_pair(".zip", "application/zip"));
	types.insert(std::make_pair(".3gp", "video/3gpp"));
	types.insert(std::make_pair(".3g2", "video/3gpp2"));
	types.insert(std::make_pair(".7z", "application/x-7z-compressed"));
}

std::string const		findMimeType(const std::string &ext){
	std::map<std::string, std::string>::iterator it;
	if ((it = types.find(ext)) == types.end())
		return "application/octet-stream";
	else
		return it->second;
}