#ifndef UTIL_H_DEFINED
#define UTIL_H_DEFINED

#include <chrono>
#include <functional>

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

/* Function Timer */
template <typename Time = std::chrono::microseconds, typename Clock = std::chrono::high_resolution_clock>
struct func_timer
{
	template <typename F, typename... Args>
	static Time duration(F&& f, Args... args)
	{
		auto start = Clock::now();
		std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
		auto end = Clock::now();

		return std::chrono::duration_cast<Time>(end-start);
	}
};

#endif
