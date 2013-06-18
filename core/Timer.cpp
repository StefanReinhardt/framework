#include "Timer.h"
#include <iostream>

#if defined(_WIN64) || defined(_WIN32)
#pragma message("Adding library: winmm.lib (needed for multimedia timers)" )
#pragma comment( lib, "winmm.lib" )
#endif

namespace core
{

	//
	//
	//
	Timer::Timer() : m_running(false), m_elapsed(0.0)
	{
#if defined(_WIN64) || defined(_WIN32)
		timeBeginPeriod(1);
#elif __APPLE__
#pragma error "Timer::Timer needs to be implemented for mac"
#elif __linux
#pragma error "Timer::Timer needs to be implemented for linux"
#endif
	}


	//
	//
	//
	Timer::~Timer()
	{
#if defined(_WIN64) || defined(_WIN32)
		timeEndPeriod(1);
#elif __APPLE__
#pragma error "Timer::~Timer needs to be implemented for mac"
#elif __linux
#pragma error "Timer::~Timer needs to be implemented for linux"
#endif
	}

	//
	//
	//
	void Timer::start()
	{
#if defined(_WIN64) || defined(_WIN32)
		m_time0 = timeGetTime();
#elif __APPLE__
#pragma error "Timer::start needs to be implemented for mac"
#elif __linux
#pragma error "Timer::start needs to be implemented for linux"
#endif
		m_running = true;
	}

	void Timer::stop()
	{
		m_running = false;
#if defined(_WIN64) || defined(_WIN32)
		m_elapsed += timeGetTime() - m_time0;
#elif __APPLE__
#pragma error "Timer::stop needs to be implemented for mac"
#elif __linux
#pragma error "Timer::stop needs to be implemented for linux"
#endif
	}

	void Timer::reset()
	{
		m_running = false;
		m_elapsed = 0;
	}



	//
	//
	//
	float Timer::elapsedSeconds()
	{
		if( m_running )
		{
			stop();
			start();
		}
#if defined(_WIN64) || defined(_WIN32)
		return  m_elapsed/1000.0f; // convert from ms in s
#elif __APPLE__
#pragma error "Timer::elapsedSeconds needs to be implemented for mac"
#elif __linux
#pragma error "Timer::elapsedSeconds needs to be implemented for linux"
#endif
	}

}
