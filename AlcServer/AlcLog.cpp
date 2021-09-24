#include "AlcLog.h"
#include <cstdio>
#include <cstdarg>

void LogInfo(const char* sInfo, ...)
{
	printf("INFO: ");

	va_list va;
	va_start(va, sInfo);

	vprintf(sInfo, va);

	va_end(va);

	printf("\n");
}
void LogError(const char* sError, ...)
{
	printf("ERROR: ");

	va_list va;
	va_start(va, sError);

	vprintf(sError, va);

	va_end(va);

	printf(" !!!\n");
}

void LogWarning(const char* sWarning, ...)
{
	printf("WARNING: ");

	va_list va;
	va_start(va, sWarning);

	vprintf(sWarning, va);

	va_end(va);

	printf(" !\n");
}