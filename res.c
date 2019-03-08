#include "res.h"

#include <stdlib.h>

#include "io.h"

void die(const char *msg)
{
    Error(msg);
    exit(EXIT_FAILURE);
}

