#include "Frame.h"

/*
Starts the counter 
*/
void Frame::StartCounter()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		cout << "QueryPerformanceFrequency failed!\n";

	PCFreq = double(li.QuadPart) / 1000.0;

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}

/*
Gets the current time
*/
double Frame::GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart - CounterStart) / PCFreq;
}

/*
Resets the counter
*/
void Frame::ResetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}