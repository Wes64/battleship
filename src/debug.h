/**********************************************************//**
 * @file debug.h
 * @author Wes64
 * @brief Useful debugging macros. Defines debug message
 * printing as well as memory allocation tracing.
 * The TRACE macro enables memory tracing.
 * The VERBOSE macro enables debug printing.
 * The DEBUG macro enables assertions.
 **************************************************************/

#ifndef _DEBUG_H_
#define _DEBUG_H_

// Standard library
#include <stddef.h> // size_t
#include <stdlib.h> // exit, EXIT_FAILURE
#include <stdio.h>  // fprintf, stderr, FILE

/**********************************************************//**
 * @def eprintf
 * @brief Print an error message to stderr.
 * @param ...: printf-style format string and tokens.
 **************************************************************/
#ifdef VERBOSE
#define eprintf(...) {\
    fprintf(stderr, "%s (%s@%d): ", __func__, __FILE__, __LINE__);\
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
        eprintf("Assertion \"%s\" failed. Abort.\n", #expression)\
        exit(EXIT_FAILURE);\
    }\
}
#else
#define assert(expression) (void)0
#endif
#endif

/*============================================================*
 * Malloc tracing
 *============================================================*/
#ifdef TRACE
#define malloc(size) debug_malloc(size, __FILE__, __LINE__, __func__)
#define free(ptr) debug_free(ptr, __FILE__, __LINE__, __func__)
#define calloc(nmemb, size) debug_calloc(nmemb, size, __FILE__, __LINE__, __func__)
#define realloc(ptr, size) debug_realloc(ptr, size, __FILE__, __LINE__, __func__)
#endif

/**********************************************************//**
 * @brief Trace a call to malloc
 * @param size: Size of block being allocated
 * @param file: The file invoking this call.
 * @param line: The line number of the file.
 * @param function: The function invoking this call
 * @return Result of malloc(size)
 **************************************************************/
extern void *debug_malloc(size_t size, const char *file, int line, const char *function);

/**********************************************************//**
 * @brief Trace a call to free
 * @param ptr: The block of memory to free
 * @param file: The file invoking this call.
 * @param line: The line number of the file.
 * @param function: The function invoking this call
 **************************************************************/
extern void debug_free(void *ptr, const char *file, int line, const char *function);

/**********************************************************//**
 * @brief Trace a call to calloc
 * @param nmemb: Number of members to allocate
 * @param size: Size of each member
 * @param file: The file invoking this call.
 * @param line: The line number of the file.
 * @param function: The function invoking this call
 * @return Result of calloc(nmemb, size)
 **************************************************************/
extern void *debug_calloc(size_t nmemb, size_t size, const char *file, int line, const char *function);

/**********************************************************//**
 * @brief Trace a call to realloc
 * @param ptr: The block to reallocate
 * @param size: Size of block being allocated
 * @param file: The file invoking this call.
 * @param line: The line number of the file.
 * @param function: The function invoking this call
 * @return Result of realloc(size)
 **************************************************************/
extern void *debug_realloc(void *ptr, size_t size, const char *file, int line, const char *function);

/*============================================================*/
#endif // _DEBUG_H_
