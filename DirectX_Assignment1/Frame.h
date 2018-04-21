#pragma once
#include <iostream>
#include <windows.h>

using namespace std;

/*
Frame class
*/
class Frame
{
public:
	void StartCounter();
	double GetCounter();
	void ResetCounter();

private:
	double PCFreq = 0.0;
	__int64 CounterStart = 0;
};