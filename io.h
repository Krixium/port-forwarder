#ifndef IO_H
#define IO_H

#include <stdarg.h>
#include <stdbool.h>

#include "res.h"

void logWithLevel(const char *level, const char *format, va_list args);
void Log(const char *format, ...);
void Error(const char *format, ...);

bool parseConfFileForPaths(fwd_path **paths, int *size);

#endif // CONFIG_H