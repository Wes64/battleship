/*=========================================================*//**
 * @file main.c
 * @brief Main application for Battleship program
 *//*=========================================================*/

// Standard library
#include <stdio.h>      // fprintf, stderr, sscanf
#include <string.h>     // strcmp

// This project
#include "audrey.h"
#include "field.h"

/*============================================================*
 * Main definitions
 *============================================================*/

// Configuration
static int verbose = 0;
static int num_games = 1;
static FILE *log = NULL;

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
        if (!strcmp(keyword, "-v")) {
            // Set the verbose flag
            verbose = 1;
        
        } else if (!strcmp(keyword, "-n")) {
            // Set the number of games
            if (sscanf(argv[i++], "%d", &num_games) < 1) {
                fprintf(stderr, "-n: Invalid argument\n");
                return -1;
            }
        
        } else if (!strcmp(keyword, "-o")) {
            // Close the old log
            if (log && !fclose(log)) {
                fprintf(stderr, "-o: Failed to close file\n");
                return -1;
            }
            
            // Open new log
            log = fopen(argv[i++], "w");
        
        } else {
            // Invalid keyword
            fprintf(stderr, "%s: Invalid keyword\n", keyword);
            return -1;
        }
    }
    
    // Success
    return 0;
}

/*=========================================================*//**
 * @brief Log a single game in the output log in CSv format
 * @param audrey        Game result to log
 * @return 0 on success, else error code
 *//*=========================================================*/
int log_PrintGame(const Audrey *audrey) {
    // Log a single game
    
    if (!log) {
        return -1;
    }
    
    fprintf(log, "%d,%d,%d,%d,%d,%d\n",
        audrey->turns,
        audrey->sink_turn[CARRIER],
        audrey->sink_turn[BATTLESHIP],
        audrey->sink_turn[SUBMARINE],
        audrey->sink_turn[CRUISER],
        audrey->sink_turn[DESTROYER]
    );
    return 0;
}

/*=========================================================*//**
 * @brief Print the CSV header into the log file
 * @return 0 on success, else error code
 *//*=========================================================*/
int log_PrintHeader(void) {
    // Print the CSV header into the log
    if (!log) {
        return -1;
    }
    
    fprintf(log, "Turn,Carrier,Battleship,Submarine,Cruiser,Destroyer\n");
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
        return -1;
    }
    
    // Set up the log
    log_PrintHeader();
    
    // Play games
    Audrey player;
    
    int i = 0;
    while (i++ < num_games) {
        if (audrey_Create(&player) || audrey_Play(&player)) {
            // Audrey error
            return -1;
        }
        // Log message
        log_PrintGame(&player);
    }

    // Done
    return 0;
}

/*============================================================*/