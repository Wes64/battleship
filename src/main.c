/**********************************************************//**
 * @file main.c
 * @brief Main Battleship program
 * @author Wes64
 * @mainpage
 * @htmlinclude main.html
 **************************************************************/

// Standard library
#include <stddef.h>     // NULL
#include <stdio.h>      // fprintf, stderr, sscanf, FILE, remove
#include <stdbool.h>    // bool
#include <string.h>     // strcmp
#include <errno.h>      // errno

// This project
#include "debug.h"      // eprintf, assert
#include "audrey.h"     // AUDREY
#include "field.h"      // FIELD

/// @def mkdir
/// @brief Make a directory in a platform-independent manner.
#ifdef _WIN32
#ifdef __MINGW32__
#include <unistd.h>     // mkdir
#else
#include <windows.h>    // CreateDirectory
#define mkdir(dir) CreateDirectory(dir, NULL)
#endif
#else
#include <sys/stat.h>   // mkdir
#define mkdir(dir) mkdir(dir, 0766)
#endif

/// The number of games to play.
static int num_games = 1;

/// The output log file or NULL.
static FILE *log = NULL;

/// The directory name to store game files or NULL.
static const char *dir_name = NULL;

/// The maximum number of turns.
static int ceiling = 0;

/**********************************************************//**
 * @brief Print the help information to the terminal.
 **************************************************************/
static void help(void) {
    printf("Battleship usage:\n");
    printf("-h:\t\tPrint the help screen\n");
    printf("-l <name>:\tWrite CSV data to the filename\n");
    printf("-n <int>:\tPlay this number of games\n");
    printf("-d <name>:\tWrite game information to the directory\n");
    printf("-c <int>:\tPrint only games taking more than <int> turns\n");
}

/**********************************************************//**
 * @brief Parser for the command-line arguments.
 * @param argc: The number of command-line arguments.
 * @param argv: Pointers to the arguments.
 * @return Whether the parser succeeded.
 **************************************************************/
static bool parse(int argc, char *argv[]) {

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
            // Set the log file
            if (log && !fclose(log)) {
                eprintf("Failed to close file.\n");
                return false;
            }
            log = fopen(argv[i++], "w");
            
        } else if (!strcmp(keyword, "-h")) {
            // Open the help screen
            return false;
            
        } else if (!strcmp(keyword, "-c")) {
            // Set the turn ceiling
            ceiling = atoi(argv[i++]);
        
        } else {
            // Bad keyword detection
            eprintf("Invalid keyword: %s\n", keyword);
            return false;
        }
    }
    
    // Make the directory
    if (dir_name && mkdir(dir_name) == -1 && errno != EEXIST) {
        eprintf("Cannot make output directory\n");
        dir_name = NULL;
        return false;
    }
    return true;
}

/**********************************************************//**
 * @brief Battleship main driver function.
 * @param argc: The number of command-line arguments.
 * @param argv: Pointers to the arguments.
 * @return Exit code.
 **************************************************************/
int main(int argc, char *argv[]) {

    // Parse arguments
    if (!parse(argc, argv)) {
        help();
        return EXIT_FAILURE;
    }
    
    // Set up the csv log
    if (log) {
        fprintf(log, "Turn,Carrier,Battleship,Submarine,Cruiser,Destroyer\n");
    }
    
    // Play games
    AUDREY player;
    FILE *game = NULL;
    const int BUF_SIZE = 1024;
    char filename[BUF_SIZE];
    for (int i = 0; i < num_games; i++) {
        
        // Initialize AI
        audrey_Create(&player);
        field_LoadRandom(&player.field);
        
        // Log individual game in the directory
        if (dir_name) {
            sprintf(filename, "%s/game%d.log", dir_name, i);
            game = fopen(filename, "w");
        }

        // Play the game
        if (!audrey_Play(&player, game)) {
            eprintf("Failed to play the game.\n");
            return EXIT_FAILURE;
        }
        
        // Close file
        if (game) {
            if (fclose(game)) {
                perror("fclose");
                return EXIT_FAILURE;
            } else if (player.turns < ceiling) {
                if (remove(filename)) {
                    perror("remove");
                    return EXIT_FAILURE;
                }
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
    fflush(log);
    fclose(log);
    return EXIT_SUCCESS;
}

/*============================================================*/
