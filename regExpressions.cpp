#include <regex>
#include <iostream>

// std::regexmatch возвращает true только тогда, когда совпадает ​​вся входная последовательность
// std::regexsearch вернет true, даже если только часть последовательности соответствует регулярному выражению

std::regex reg0("(/)[\\w-]+(/)");

std::regex reg1("/(|[^/]+)(/folder/)");

// /(|[^/]+)/ - any sumbols but not /


int main() {
	std::string location("/put-test/folder/");
	if (std::regex_match(location, reg1))
		std::cout << "FOUND: " << location << std::endl;
	else
		std::cout << "NOT FOUND" << std::endl;

// if (std::regex_match("subject", std::regex("(sub)(.*)") ))
//     std::cout << "string literal matched\n";

	return 0;
}

