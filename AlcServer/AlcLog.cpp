#include "AlcLog.h"
#include <cstdio>
#include <cstdarg>

LogInfo(const char* sInfo)
{
	printf("INFO: %s\n", sInfo);
}
LogError(const char* sError)
{
	printf("ERROR: %s !!!\n", sError);
}
LogWarning(const char* sWarning)
{
	printf("Warning: %s !\n", sWarning);
}

LogInfo(const char* sInfo, ...)
{
	printf("INFO: ");

	va_list va;
	va_start(va, sInfo);

	vprintf(sInfo, va);

	va_end(va);

	printf("\n");
}
LogError(const char* sError, ...)
{
	printf("ERROR: ");

	va_list va;
	va_start(va, sInfo);

	vprintf(sInfo, va);

	va_end(va);

	printf(" !!!\n");
}
LogWarning(const char* sWarning, ...)
{
	printf("WARNING: ");

	va_list va;
	va_start(va, sInfo);

	vprintf(sInfo, va);

	va_end(va);

	printf(" !\n");
}