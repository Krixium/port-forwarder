#include "main.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#include "res.h"

int main(int argc, char *argv[])
{
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

    // do stuff with forwarding paths here

    // parseConfFileForPaths does malloc/realloc
    free(paths);

    Log("Stopping");
    return 0;
}

