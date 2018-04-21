#include "Basics.h"

/*
Function to take care of errors
*/
void SetError(char* szFormat, ...) {
	char szBuffer[1024];
	va_list pArgList;

	va_start(pArgList, szFormat);

	_vsntprintf_s(szBuffer, sizeof(szBuffer) / sizeof(char), szFormat, pArgList);

	va_end(pArgList);

	OutputDebugString(szBuffer);
	OutputDebugString("\n");
	printf("error!!");
}