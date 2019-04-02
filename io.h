#ifndef IO_H
#define IO_H

#include <stdarg.h>
#include <stdbool.h>

#include "res.h"

void logWithLevel(const char *level, const char *format, va_list args);
void Log(const char *format, ...);
void Error(const char *format, ...);

bool parseLine(const char *line, char *inAddr, int *inPort, char *outAddr, int *outPort);
bool fillAddr(struct sockaddr_in *out, const char *address, const int port);

bool parseConfFileForPaths(fwd_path **paths, int *size);


#endif // CONFIG_H