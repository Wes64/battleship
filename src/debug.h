/**********************************************************//**
 * @file debug.h
 * @author Rena Shinomiya
 * @brief Useful debugging macros. The DEBUG macro enables
 * this module.
 * @version 2.1
 * @date March 2, 2019
 **************************************************************/

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/**********************************************************//**
 * @def eprintf
 * @brief Print an error message to stderr.
 * @param ...: printf-style format string and tokens.
 **************************************************************/
#ifdef DEBUG
#define eprintf(...) {\
    fprintf(stderr, "(%s@%s:%d): ", __func__, __FILE__, __LINE__);\
    fprintf(stderr, __VA_ARGS__);\
}
#else
#define eprintf(...) (void)0
#endif

/**********************************************************//**
 * @def assert
 * @brief Check if a condition is true for debugging.
 * @param expression: The code to execute.
 **************************************************************/
#ifndef assert
#ifdef DEBUG
#define assert(expression) {\
    if (!(expression)) {\
        eprintf("Assertion \"%s\" failed. Abort.\n", #expression);\
        exit(EXIT_FAILURE);\
    }\
}
#else
#define assert(expression) (void)0
#endif
#endif

/**************************************************************/
#endif // _DEBUG_H_
