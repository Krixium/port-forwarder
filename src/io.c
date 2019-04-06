/*---------------------------------------------------------------------------------------
-- SOURCE FILE:             io.c
--
-- PROGRAM:                 forwarder.out
--
-- FUNCTIONS:
--                          void logWithLevel(const char *level, const char *format, va_list args)
--                          void Log(const char *format, ...)
--                          void Error(const char *format, ...)
--                          bool parseLine(const char *line, char *inAddr, int *inPort, char *outAddr, int *outPort)
--                          bool fillAddr(struct sockaddr_in *out, const char *address, const int port)
--                          bool parseConfFileForPaths(fwd_path **paths, int *size)
--
-- DATE:                    April 1, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNERS:               Benny Wang
--
-- PROGRAMMERS:             Benny Wang
--
-- NOTES:
-- Contains all the functions that are used for parsing the configuration file as well as
-- logging functions.
---------------------------------------------------------------------------------------*/

#define CONF_FILE "./forwarder.conf"
#define LINE_BUFFER_SIZE 64
#define IP_BUFFER_SIZE 16

#include "io.h"

#include <ctype.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "res.h"


/*---------------------------------------------------------------------------------------
-- FUNCTION:                logWithLevel
--
-- DATE:                    April 1, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               void logWithLevel(const char *level, const char *format, va_list args)
--                              char *level: The log serverity level.
--                              const char *format: The string literal for the format.
--                              va_list args: Arguments fitting the string literal.
--
-- NOTES:
-- Generic logging function that prepends timestamp and severity level to logging message.
---------------------------------------------------------------------------------------*/
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

/*---------------------------------------------------------------------------------------
-- FUNCTION:                Log
--
-- DATE:                    April 1, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               void Log(const char *format, ...)
--                              const char *format: String literal to print.
--                              ...: List of arguements fitting the provided string literal.
--
-- NOTES:
-- Logs a message with a normal serverity.
---------------------------------------------------------------------------------------*/
void Log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logWithLevel("  LOG  ", format, args);
    va_end(args);
}

/*---------------------------------------------------------------------------------------
-- FUNCTION:                Error
--
-- DATE:                    April 1, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               void Error(const char *format, ...)
--                              const char *format: String literal to print.
--                              ...: List of arguements fitting the provided string literal.
--
-- NOTES:
-- Logs a message with a error serverity.
---------------------------------------------------------------------------------------*/
void Error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logWithLevel(" ERROR ", format, args);
    va_end(args);
}

/*---------------------------------------------------------------------------------------
-- FUNCTION:                parseLine
--
-- DATE:                    April 1, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               bool parseLine(const char *line, char *inAddr, int *inPort, char *outAddr, int *outPort)
--                              const char *line: The line to parse.
--                              char *inAddr: Pointer to where the incoming address string will be placed.
--                              int *inPort: Pointer to where the incoming port will be placed.
--                              char *outAddr: Pointer to where the outgoing address string will be placed.
--                              int *outPort: Pointer to where the outgoing port will be placed.
--
-- RETURNS:                 True if the line was successfully parsed, false otherwise.
--
-- NOTES:
-- Parses a line with the format "adress:port -> address:port". This function does not
-- tolerate any error in the line format and will return false if the format is not met
-- exactly. Addresses must be in dotted decimal format.
---------------------------------------------------------------------------------------*/
bool parseLine(const char *line, char *inAddr, int *inPort, char *outAddr, int *outPort)
{
    const char *delim = " -> ";
    const int delimSize = 4;
    char portBuffer[6];
    int i;
    int j;
    int secondIpStart;

    bzero(portBuffer, 6);

    for (i = 0; line[i] != 0; i++)
    {
        if (line[i] == delim[0])
        {
            // compare to see if the delim has been found
            for (j = 1; j < delimSize; j++)
            {
                if (line[i + j] != delim[j])
                {
                    // invalid format - delim mismatch
                    return false;
                }
            }

            if (j == delimSize)
            {
                // start of second ip has been found
                secondIpStart = i + 4;
                break;
            }
            else
            {
                // invalid format
                return false;
            }
        }
    }

    if (line[i] == 0)
    {
        // invalid format - delim was not found
        return false;
    }

    // grab first ip
    for (j = 0; line[j] != ':'; j++)
    {
        inAddr[j] = line[j];
    }
    inAddr[j] = 0;
    j++;

    // extract port as a number
    for (i = 0; isdigit(line[j]); i++, j++)
    {
        portBuffer[i] = line[j];
    }
    portBuffer[i] = 0;
    *inPort = atoi(portBuffer);

    // grab second ip
    for (j = 0; line[j + secondIpStart] != ':'; j++)
    {
        outAddr[j] = line[j + secondIpStart];
    }
    outAddr[j] = 0;
    j++;

    // extract port as a number
    for (i = 0; isdigit(line[j + secondIpStart]); i++, j++)
    {
        portBuffer[i] = line[j + secondIpStart];
    }
    portBuffer[i] = 0;
    *outPort = atoi(portBuffer);

    return true;
}

/*---------------------------------------------------------------------------------------
-- FUNCTION:                fillAddr
--
-- DATE:                    April 1, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               bool fillAddr(struct sockaddr_in *out, const char *address, const int port)
--                              struct sockaddr_in *out: Pointer to the struct to fill.
--                              const char *address: The address string to parse.
--                              const int port: The desired port to parse.
--
-- RETURNS:                 True if the struct was filled, false otherwise.
--
-- NOTES:
-- Sets the given sockaddr_in to an internet struct with the given address and port.
---------------------------------------------------------------------------------------*/
bool fillAddr(struct sockaddr_in *out, const char *address, const int port)
{
    struct hostent *hp;
    out->sin_family = AF_INET;
    out->sin_port = htons(port);
    if ((hp = gethostbyname(address)) == NULL)
    {
        return false;
    }
    bcopy(hp->h_addr_list[0], (char *)&out->sin_addr, hp->h_length);
    return true;
}

/*---------------------------------------------------------------------------------------
-- FUNCTION:                parseConfFileForPaths
--
-- DATE:                    April 1, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               bool parseConfFileForPaths(fwd_path **paths, int *size)
--                              fwd_path **paths: Pointer to the array of paths structs.
--                              int *size: Pointer to the size of the fwd_path array.
--
-- RETURNS:                 True if the file was parsed, false otherwise.
--
-- NOTES:
-- Given a pointer for the destination and size of a fwd_path array and a file, this
-- function will parse all the linse of the file following the format "address:port -> address:port"
-- and place them in the paths variable and set size to the size of paths.
---------------------------------------------------------------------------------------*/
bool parseConfFileForPaths(fwd_path **paths, int *size)
{
    FILE *confFile;
    char lineBuffer[LINE_BUFFER_SIZE];
    char inIp[IP_BUFFER_SIZE];
    char outIp[IP_BUFFER_SIZE];
    int inPort;
    int outPort;
    unsigned int limit;
    fwd_path tmp;

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
        // clear buffers
        bzero(&tmp, sizeof(fwd_path));
        bzero(inIp, IP_BUFFER_SIZE);
        bzero(outIp, IP_BUFFER_SIZE);

        // extract line
        if (!fgets(lineBuffer, LINE_BUFFER_SIZE, confFile))
        {
            // break if eof or read errr occurs
            break;
        }

        // parse the line that was read
        if (!parseLine(lineBuffer, inIp, &inPort, outIp, &outPort))
        {
            Error("Could not read line, skipping");
            continue;
        }

        Log("Scanned %s:%d -> %s:%d", inIp, inPort, outIp, outPort);

        // populate the addr struct for incoming
        if (!fillAddr(&(tmp.in), inIp, inPort))
        {
            Error("Could not get host for %s, skipping", inIp);
        }

        // populate the addr struct for outgoing
        if (!fillAddr(&(tmp.out), outIp, outPort))
        {
            Error("Could not get host for %s, skipping", outPort);
        }

        // resize array if needed
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

    // resize the array to be the exact size
    if ((*paths = realloc(*paths, sizeof(fwd_path) * (*size))) == NULL)
    {
        Error("Could not allocate memory");
        die("realloc");
    }

    return true;
}