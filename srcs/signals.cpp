#include "utils.hpp"

void	sigquit(int sig)
{
	std::cout << "\b\b\033SIGQUIT\033[0m" << std::endl;
	SERVER_CLOSE = true;
	exit(0);
}

void	sigint(int sig)
{
	std::cout << "\b\b\033[31mSIGINT\033[0m" << std::endl;
	SERVER_CLOSE = true;
	exit(0);
}