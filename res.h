#ifndef RES_H
#define RES_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

typedef struct fowarding_path
{
    struct sockaddr_in in;
    struct sockaddr_in out;
} fwd_path;

void die(const char *msg);

#endif // RES_H