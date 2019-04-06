/*---------------------------------------------------------------------------------------
-- SOURCE FILE:             res.c
--
-- PROGRAM:                 forwarder.out
--
-- FUNCTIONS:
--                          void die(const char *msg)
--
-- DATE:                    March 20, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNERS:               Benny Wang
--
-- PROGRAMMERS:             Benny Wang
--
-- NOTES:
-- Generic resource functions.
---------------------------------------------------------------------------------------*/

#include "res.h"

#include <stdlib.h>

#include "io.h"

/*--------------------------------------------------------------------------------------------------
-- FUNCTION:                die
--
-- DATE:                    February 11, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               void die(const char *msg)
--                              const char *msg: Error message.
--
-- NOTES:
-- Logs an error message and then exits the program with exit code 1.
--------------------------------------------------------------------------------------------------*/
void die(const char *msg)
{
    Error(msg);
    exit(EXIT_FAILURE);
}

