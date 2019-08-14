#ifndef UTIL_H_DEFINED
#define UTIL_H_DEFINED

#define DEBUG 1

template<typename ...Args>
void LOGD(Args && ...args)
{
#ifdef DEBUG
	(std::cout << ... << args);
	std::cout << std::endl;
	fflush(stdout);
#endif
}

#endif
