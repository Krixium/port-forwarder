#define READ_BUFFER_SIZE 65535
#define LISTEN_TIMEOUT_SECS 60
#define LISTEN_TIMEOUT_USECS 0

#include "main.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "net.h"

int main(int argc, char *argv[])
{
    int *pids;

    // number of paths
    int pathSize = 0;

    // array of paths
    fwd_path *paths;

    Log("Starting forwarder");

    // Parse log file, your job to free paths
    if (!parseConfFileForPaths(&paths, &pathSize))
    {
        die("Could not parse file");
    }

    if ((pids = malloc(sizeof(int) * pathSize)) == NULL)
    {
        die("Could not allocate memory");
    }

    // do stuff with forwarding paths here
    for (int i = 0; i < pathSize; i++)
    {
        pids[i] = fork();
        if (pids[i])
        {
            // parent
            continue;
        }
        else
        {
            // child
            childRoutine(paths + i);
        }
    }

    for (int i = 0; i < pathSize; i++)
    {
        waitpid(pids[i], NULL, 0);
    }

    // parseConfFileForPaths does malloc/realloc
    free(paths);

    Log("Stopping");
    return 0;
}

void childRoutine(fwd_path *path)
{
    char buffer[READ_BUFFER_SIZE];

    int numRead;

    int listenSocket;
    int inSocket;
    int outSocket;
    struct sockaddr_in incomingStruct;

    if (!uwuCreateBoundSocket(&listenSocket, ntohs(path->in.sin_port)))
    {
        die("Could not bind incoming socket");
    }

    if (!uwuSetSocketTimeout(LISTEN_TIMEOUT_SECS, LISTEN_TIMEOUT_USECS, listenSocket))
    {
        die("Could not set socket timeout for listen socket");
    }

    listen(listenSocket, 5);

    while (1)
    {
        if (!uwuAcceptSocket(listenSocket, &inSocket, &incomingStruct))
        {
            die("No incoming connection");
        }

        if (incomingStruct.sin_addr.s_addr != path->in.sin_addr.s_addr)
        {
            Error("Invalid incoming address, skipping");
            continue;
        }

        if (!createConnectedSocket(&outSocket, &path->out))
        {
            die("Could not connect to outgoing server");
        }

        close(listenSocket);

        if (fork())
        {
            while (1)
            {
                numRead = uwuReadAllFromSocket(inSocket, buffer, READ_BUFFER_SIZE);
                send(outSocket, buffer, numRead, 0);

                if (numRead <= 0)
                {
                    close(inSocket);
                    close(outSocket);
                    exit(0);
                }
            }
        }
        else
        {
            while (1)
            {
                numRead = uwuReadAllFromSocket(outSocket, buffer, READ_BUFFER_SIZE);
                send(inSocket, buffer, numRead, 0);

                if (numRead <= 0)
                {
                    close(inSocket);
                    close(outSocket);
                    exit(0);
                }
            }
        }
    }
}

