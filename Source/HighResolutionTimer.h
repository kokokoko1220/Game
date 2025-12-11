#pragma once

#include <windows.h>

class HighResolutionTimer
{
public:
	HighResolutionTimer() : delta_time(-1.0), paused_time(0), stopped(false)
	{
		LONGLONG counts_per_sec;
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&counts_per_sec));
		seconds_per_count = 1.0 / static_cast<double>(counts_per_sec);

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&this_time));
		base_time = this_time;
		last_time = this_time;
	}

	
	float TimeStamp() const  
	{
		

		if (stopped)
		{
			return static_cast<float>(((stop_time - paused_time) - base_time)*seconds_per_count);
		}

		
		else
		{
			return static_cast<float>(((this_time - paused_time) - base_time)*seconds_per_count);
		}
	}

	float TimeInterval() const  
	{
		return static_cast<float>(delta_time);
	}

	void Reset() 
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&this_time));
		base_time = this_time;
		last_time = this_time;

		stop_time = 0;
		stopped = false;
	}

	void Start() 
	{
		LONGLONG start_time;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&start_time));

		
		if (stopped)
		{
			paused_time += (start_time - stop_time);
			last_time = start_time;
			stop_time = 0;
			stopped = false;
		}
	}

	void Stop() 
	{
		if (!stopped)
		{
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&stop_time));
			stopped = true;
		}
	}

	void Tick()
	{
		if (stopped)
		{
			delta_time = 0.0;
			return;
		}

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&this_time));
		
		delta_time = (this_time - last_time)*seconds_per_count;

		
		last_time = this_time;

		
		if (delta_time < 0.0)
		{
			delta_time = 0.0;
		}
	}

private:
	double seconds_per_count;
	double delta_time;

	LONGLONG base_time;
	LONGLONG paused_time;
	LONGLONG stop_time;
	LONGLONG last_time;
	LONGLONG this_time;

	bool stopped;
};
