#pragma once

#define ASSERT(x) if (!(x)) __debugbreak();
#define glCall(x) clearError(); x; ASSERT(logCall(#x, __FILE__, __LINE__));

void clearError();

bool logCall(const char* func, const char* file, int line);

void setupDebugHooks();

void logTickStart();
void logTickEnd();