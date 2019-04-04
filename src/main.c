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
    Log("Listening for connection ...");

    while (1)
    {
        if (!uwuAcceptSocket(listenSocket, &inSocket, &incomingStruct))
        {
            die("No incoming connection");
        }
        Log("Connection accpeted from %s", inet_ntoa(incomingStruct.sin_addr));

        if (incomingStruct.sin_addr.s_addr != path->in.sin_addr.s_addr)
        {
            close(inSocket);
            Error("Invalid incoming address, skipping");
            continue;
        }

        Log("Connecting to destination host");
        if (!createConnectedSocket(&outSocket, &path->out))
        {
            die("Could not connect to outgoing server");
        }

        close(listenSocket);
        Log("Connected to destination host %s, closing listening socket", inet_ntoa(path->out.sin_addr));

        if (fork())
        {
            Log("Waiting for data from incoming host ...");
            while (1)
            {
                numRead = recv(inSocket, buffer, READ_BUFFER_SIZE, 0);
                send(outSocket, buffer, numRead, 0);

                if (numRead <= 0)
                {
                    close(inSocket);
                    Log("Closing connection to %s", inet_ntoa(path->in.sin_addr));
                    exit(0);
                }
            }
        }
        else
        {
            Log("Waiting for data from outgoing host ...");
            while (1)
            {
                numRead = recv(outSocket, buffer, READ_BUFFER_SIZE, 0);
                send(inSocket, buffer, numRead, 0);

                if (numRead <= 0)
                {
                    close(outSocket);
                    Log("Closing connection to %s", inet_ntoa(path->out.sin_addr));
                    exit(0);
                }
            }
        }
    }
}

