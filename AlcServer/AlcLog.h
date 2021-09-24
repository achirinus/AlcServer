#pragma once

LogInfo(const char* sInfo);
LogError(const char* sError);
LogWarning(const char* sWarning);

LogInfo(const char* sInfo, ...);
LogError(const char* sError, ...);
LogWarning(const char* sWarning, ...);