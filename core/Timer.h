#ifndef TIMER_H
#define TIMER_H


#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>
#include <mmsystem.h>
#elif __APPLE__
#pragma error "Timer needs to be implemented for mac"
#elif __linux
#pragma error "Timer needs to be implemented for linux"
#endif

namespace core
{
	//
	// This class is able to measure elapsed time from a certain start point
	//
	class Timer
	{
	public:
		Timer();
		~Timer();
		void start();
		void stop();
		void reset();
		float elapsedSeconds();

	private:
		bool                  m_running;
	#if defined(_WIN64) || defined(_WIN32)
		unsigned long           m_time0; // time in ms
	#elif __APPLE__
	#pragma error "Timer needs to be implemented for mac"
	#elif __linux
	#pragma error "Timer needs to be implemented for linux"
	#endif
		float                 m_elapsed; // time in s
	};

	inline void sleep( int sleepMs )
	{
	#if defined(_WIN64) || defined(_WIN32)
		Sleep(sleepMs);
	#elif __APPLE__
	#pragma error "sleep needs to be implemented for mac"
	#elif __linux
		usleep(sleepMs * 1000);   // usleep takes sleep time in us
	#endif
	}


	#endif //TIMER_H

}
