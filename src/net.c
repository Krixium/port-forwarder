/*---------------------------------------------------------------------------------------
-- SOURCE FILE:             net.c
--
-- PROGRAM:                 forwarder.out
--
-- FUNCTIONS:
--                          int uwuCreateTCPSocket(int *sock)
--                          int createConnectedSocket(int *sock, struct sockaddr_in *addr)
--                          int uwuCreateBoundSocket(int *sock, const short port)
--                          int uwuAcceptSocket(const int listenSocket, int *newSocket, struct sockaddr_in *client)
--                          int uwuSetSocketTimeout(const size_t sec, const size_t usec, const int sock)
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
-- Contains all functions related to network IO.
---------------------------------------------------------------------------------------*/

#include "net.h"

#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

/*--------------------------------------------------------------------------------------------------
-- FUNCTION:                uwuCreateTCPSocket
--
-- DATE:                    February 11, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               int uwuCreateTCPSocket(int *sock)
--                              int *sock: The pointer that will hold the new socket.
--
-- RETURNS:                 1 if the socket was created without error, 0 otherwise.
--
-- NOTES:
-- Creates a TCP socket. The newly created socket is set to reuse.
--------------------------------------------------------------------------------------------------*/
int uwuCreateTCPSocket(int *sock)
{
    int arg = 1;

    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        return 0;
    }

    if (setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &arg, sizeof(arg)) == -1)
    {
        return 0;
    }

    return 1;
}


/*--------------------------------------------------------------------------------------------------
-- FUNCTION:                createConnectedSocket
--
-- DATE:                    April 1, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               int createConnectedSocket(int *sock, struct sockaddr_in *addr)
--                              int *sock: The pointer that will hold the connected socket.
--                              struct sockaddr_in *addr: Pointer to address struct.
--
-- RETURNS:                 1 if the socket was created and connected without error, 0 otherwise.
--
-- NOTES:
-- Creates a connected TCP socket to address:port and stores the newly connected socket to sock.
--------------------------------------------------------------------------------------------------*/
int createConnectedSocket(int *sock, struct sockaddr_in *addr)
{
    if (!uwuCreateTCPSocket(sock))
    {
        return 0;
    }

    if (connect(*sock, (struct sockaddr *)addr, sizeof(*addr)) == -1)
    {
        return 0;
    }

    return 1;
}

/*--------------------------------------------------------------------------------------------------
-- FUNCTION:                uwuCreateBoundSocket
--
-- DATE:                    February 11, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               int uwuCreateBoundSocket(int *sock, const short port)
--                              int *sock: The pointer that will hold the bound socket.
--                              const short port: The port to listen on.
--
-- RETURNS:                 1 if the socket was created and bounded without error, 0 otherwise.
--
-- NOTES:
-- Creates a bounded TCP socket on port and stores the newly bounded socket to sock. The newly
-- created socket is set to reuse.
--------------------------------------------------------------------------------------------------*/
int uwuCreateBoundSocket(int *sock, const short port)
{
    struct sockaddr_in server;

    if (!uwuCreateTCPSocket(sock))
    {
        return 0;
    }

    bzero(&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(*sock, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        return 0;
    }

    return 1;
}

/*--------------------------------------------------------------------------------------------------
-- FUNCTION:                uwuAcceptSocket
--
-- DATE:                    February 11, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               int uwuAcceptSocket(const int listenSocket, int *newSocket, struct sockaddr_in *client)
--                              const int listenSocket: The listening socket.
--                              int *newSocket: The pointer where the new socket will be stored.
--                              struct sockaddr_in *client: The pointer to store client information, can be NULL.
--
-- RETURNS:                 1 if the new connection was accepted, 0 otherwise.
--
-- NOTES:
-- Accepts a new connection. If client information is not important, NULL can be passed in for client.
--------------------------------------------------------------------------------------------------*/
int uwuAcceptSocket(const int listenSocket, int *newSocket, struct sockaddr_in *client)
{
    struct sockaddr_in tmp;

    unsigned int length = sizeof(struct sockaddr_in);

    if (client == NULL)
    {
        bzero(&tmp, length);
        if ((*newSocket = accept(listenSocket, (struct sockaddr *)&tmp, &length)) == -1)
        {
            return 0;
        }
    }
    else
    {
        bzero(client, length);
        if ((*newSocket = accept(listenSocket, (struct sockaddr *)client, &length)) == -1)
        {
            return 0;
        }
    }

    return 1;
}

/*--------------------------------------------------------------------------------------------------
-- FUNCTION:                uwuSetSocketTimeout
--
-- DATE:                    February 11, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               int uwuSetSocketTimeout
--                              const size_t sec: The amount of seconds to wait before timing out.
--                              const size_t sec: The amount of nanosecond to wait before timing out.
--
-- RETURN:                  1 if the socket options were set, 0 otherwise.
--
-- NOTES:
-- Sets the receive timeout and send timeout of a socket.
--------------------------------------------------------------------------------------------------*/
int uwuSetSocketTimeout(const size_t sec, const size_t usec, const int sock)
{
    struct timeval timeout;
    timeout.tv_sec = sec;
    timeout.tv_usec = usec;

    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        return 0;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        return 0;
    }

    return 1;
}