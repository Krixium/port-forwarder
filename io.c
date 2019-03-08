#define CONF_FILE "./fowarder.conf"
#define IP_BUFFER_SIZE 16

#include "io.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "res.h"

void logWithLevel(const char *level, const char *format, va_list args)
{
    char timestamp[100];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(timestamp, sizeof(timestamp) - 1, "%d/%m/%Y-%H:%M:%S", t);
    printf("[ %s ] [%s] ", timestamp, level);

    vprintf(format, args);

    printf("\n");
}

void Log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logWithLevel("  LOG  ", format, args);
    va_end(args);
}

void Error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logWithLevel(" ERROR ", format, args);
    va_end(args);
}

bool parseConfFileForPaths(fwd_path **paths, int *size)
{
    FILE *confFile;
    char inIp[IP_BUFFER_SIZE];
    char outIp[IP_BUFFER_SIZE];
    int inPort;
    int outPort;
    unsigned int limit;
    fwd_path tmp;
    struct hostent *hp;

    Log("Opening conf file: %s", CONF_FILE);
    if ((confFile = fopen(CONF_FILE, "r")) == NULL)
    {
        Error("Could not open: %s", CONF_FILE);
        return false;
    }

    Log("Parsing log file");
    limit = 1;
    *size = 0;
    if ((*paths = malloc(sizeof(fwd_path) * limit)) == NULL)
    {
        Error("Could not allocate memory");
        die("malloc");
    }
    while (true)
    {
        bzero(&tmp, sizeof(fwd_path));
        bzero(inIp, IP_BUFFER_SIZE);
        bzero(outIp, IP_BUFFER_SIZE);

        if (fscanf(confFile, "%s %d %s %d", inIp, &inPort, outIp, &outPort) < 4)
        {
            break;
        }
        Log("Scanned %s:%d -> %s:%d", inIp, inPort, outIp, outPort);

        tmp.in.sin_family = AF_INET;
        tmp.in.sin_port = htons(inPort);
        if ((hp = gethostbyname(inIp)) == NULL)
        {
            Error("Could not get host for %s, skipping", inIp);
            continue;
        }
        bcopy(hp->h_addr_list[0], (char *)&tmp.in.sin_addr, hp->h_length);

        tmp.out.sin_family = AF_INET;
        tmp.out.sin_port = htons(outPort);
        if ((hp = gethostbyname(outIp)) == NULL)
        {
            Error("Could not get host for %s, skipping", outIp);
            continue;
        }
        bcopy(hp->h_addr_list[0], (char *)&tmp.out.sin_addr, hp->h_length);

        if (*size < limit)
        {
            bcopy(&tmp, *paths + *size, sizeof(fwd_path));
            (*size)++;
        }
        else
        {
            limit *= 2;
            if ((*paths = realloc(*paths, sizeof(fwd_path) * limit)) == NULL)
            {
                Error("Could not allocate memory");
                die("realloc");
            }
            bcopy(&tmp, *paths + *size, sizeof(fwd_path));
            (*size)++;
        }
    }

    Log("Closing conf file: %s", CONF_FILE);
    if (fclose(confFile))
    {
        Error("Could not close: %s", CONF_FILE);
    }
    Log("Finished parsing log file");

    if ((*paths = realloc(*paths, sizeof(fwd_path) * (*size))) == NULL)
    {
        Error("Could not allocate memory");
        die("realloc");
    }

    return true;
}