/*=========================================================*//**
 * @file main.c
 * @brief Main application for Battleship program
 *//*=========================================================*/

// Standard library
#include <stdio.h>      // fprintf, stderr, sscanf, FILE, remove
#include <string.h>     // strcmp
#include <stdlib.h>     // NULL
#include <errno.h>      // errno
#include <assert.h>     // assert

// Cross-compiling
#ifdef _WIN32
#define SEP "\\"

#ifdef __MINGW32__
#include <unistd.h>     // mkdir

#else
#include <windows.h>		// CreateDirectory
#define mkdir(dir) CreateDirectory(dir, NULL)

#endif // __MINGW32__

#else   // !_WIN32
#include <sys/stat.h>   // mkdir
#define SEP "/"
#define mkdir(dir) mkdir(dir, 0766)

#endif  // _WIN32

// This project
#include "audrey.h"
#include "field.h"

/*============================================================*
 * Main definitions
 *============================================================*/

// Configuration
static int num_games = 1;
static FILE *log = NULL;
static const char *dir_name = NULL;
static int ceiling = 0;

/*=========================================================*//**
 * @brief Print the help screen
 *//*=========================================================*/
void help(void) {
    // Output options and help
    printf(
        "Battleship usage:\n"
        "-h:\t\tPrint the help screen\n"
        "-l <name>:\tWrite CSV data to the filename\n"
        "-n <int>:\tPlay this number of games\n"
        "-d <name>:\tWrite game information to the directory\n"
        "-c <int>:\tPrint only games taking more than <int> turns\n"
    );
}

/*=========================================================*//**
 * @brief Parser for command-line arguments
 * @param argc      Number of arguments
 * @param argv      Pointers to arguments
 * @return 0 on success, else error code
 *//*=========================================================*/
int parse(int argc, char *argv[]) {
    // Parse the arguments
    
    // Parse arguments
    int i = 1;
    while (i < argc) {
        const char *keyword = argv[i++];
        
        // Parse keyword meaning
        if (!strcmp(keyword, "-d")) {
            // Set the verbose directory flag
            dir_name = argv[i++];
        
        } else if (!strcmp(keyword, "-n")) {
            // Set the number of games
            if (sscanf(argv[i++], "%d", &num_games) < 1) {
                fprintf(stderr, "-l: Invalid argument\n");
                return -1;
            }
        
        } else if (!strcmp(keyword, "-l")) {
            // Close the old log
            if (log && !fclose(log)) {
                fprintf(stderr, "-o: Failed to close file\n");
                return -1;
            }
            
            // Open new log
            log = fopen(argv[i++], "w");
            
        } else if (!strcmp(keyword, "-h")) {
            // Print help
            return -1;
            
        } else if (!strcmp(keyword, "-c")) {
            // Set the ceiling
            ceiling = atoi(argv[i++]);
        
        } else {
            // Invalid keyword
            fprintf(stderr, "%s: Invalid keyword\n", keyword);
            return -1;
        }
    }
    
    // Make the directory
    if (dir_name && mkdir(dir_name) == -1 && errno != EEXIST) {
        fprintf(stderr, "Cannot make output directory\n");
        dir_name = NULL;
        return -1;
    }
    
    // Success
    return 0;
}

/*=========================================================*//**
 * @brief The main function for Battleship
 * @param argc      The number of arguments
 * @param argv      Pointers to argument strings
 * @return 0 on success, else error code
 *//*=========================================================*/
int main(int argc, char *argv[]) {
    // Main function
    
    // Parse arguments
    if (parse(argc, argv)) {
        help();
        return -1;
    }
    
    // Set up the log
    if (log) {
        fprintf(log, "Turn,Carrier,Battleship,Submarine,Cruiser,Destroyer\n");
    }
    
    // Play games
    Audrey player;
    
    int i = 0;
    while (i++ < num_games) {
        if (audrey_Create(&player)) {
            fprintf(stderr, "audrey_Create failed\n");
            return -1;
        }

        if (field_LoadRandom(&player.field)) {
            fprintf(stderr, "field_LoadRandom failed\n");
            return -1;
        }

        if (field_ReadyPlay(&player.field)) {
            fprintf(stderr, "field_ReadyPlay failed\n");
            return -1;
        }
        
        // Log individual game in the directory
        FILE *game;
        const int BUF_SIZE = 1024;
        char filename[BUF_SIZE];
        
        if (dir_name) {
            // Generate log file name
            sprintf(filename, "%s%sgame%d.log", dir_name, SEP, i);
            game = fopen(filename, "w");
            
        } else {
            game = NULL;
        }

        if (audrey_Play(&player, game)) {
            fprintf(stderr, "audrey_Play failed\n");
            return -1;
        }
        
        // Close file
        if (game && fclose(game)) {
            fprintf(stderr, "fclose failed\n");
            return -1;
        }
        
        // Remove the file if not enough turns
        if (game && player.turns < ceiling) {
            if (remove(filename)) {
                perror("remove failed");
                return -1;
            }
        }

        // Log each game as csv
        if (log) {
            fprintf(log, "%d,%d,%d,%d,%d,%d\n",
                player.turns,
                player.sink_turn[CARRIER],
                player.sink_turn[BATTLESHIP],
                player.sink_turn[SUBMARINE],
                player.sink_turn[CRUISER],
                player.sink_turn[DESTROYER]
            );
        }
    }
    
    // Flush logged output
    fflush(log);

    // Done
    return 0;
}

/*============================================================*/
