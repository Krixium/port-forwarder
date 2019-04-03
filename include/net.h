#ifndef NET_H
#define NET_H

#include <sys/types.h>
#include <netdb.h>

int uwuCreateTCPSocket(int *sock);
int createConnectedSocket(int *sock, struct sockaddr_in *addr);
int uwuCreateBoundSocket(int *sock, const short port);
int uwuAcceptSocket(const int listenSocket, int *newSocket, struct sockaddr_in *client);
int uwuReadAllFromSocket(const int sock, char *buffer, const unsigned int size);
int uwuSetSocketTimeout(const size_t sec, const size_t usec, const int sock);

#endif // NET_H