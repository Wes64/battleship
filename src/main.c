/**********************************************************//**
 * @file main.c
 * @brief Main Battleship program
 * @author Wes64
 **************************************************************/

// Standard library
#include <stddef.h>     // NULL
#include <stdbool.h>    // bool
#include <stdlib.h>		// srand
#include <stdio.h>      // FILE
#include <time.h>		// time
#include <string.h>     // strcmp

// This project
#include "debug.h"      // eprintf, assert
#include "field.h"      // FIELD
#include "ai.h"     	// AI

/// The number of games to play.
static int NumberOfGames = 1;

/// The output log filename.
static const char *OutputFilename = NULL;

/// The output log file or NULL.
static FILE *OutputLog = NULL;

/**********************************************************//**
 * @brief Print the help information to the terminal.
 **************************************************************/
static inline void help(int argc, char **argv) {
	(void)argc;
    printf("%s usage:\n", argv[0]);
    printf("-h:        Print the help screen.\n");
    printf("-o <name>: Write CSV data to the filename.\n");
    printf("-n <int>:  Play this number of games.\n");
}

/**********************************************************//**
 * @brief Parser for the command-line arguments.
 * @param argc: The number of command-line arguments.
 * @param argv: Pointers to the arguments.
 * @return Whether the parser succeeded.
 **************************************************************/
static inline bool parse(int argc, char *argv[]) {
    // Parse arguments
    int i = 1;
    while (i < argc) {
		// Get the current keyword symbol
        const char *keyword = argv[i++];
        if (!strcmp(keyword, "-n")) {
            NumberOfGames = atoi(argv[i++]);
        } else if (!strcmp(keyword, "-o")) {
            OutputFilename = argv[i++];
        } else {
			return false;
		}
    }
    
    // Open the output file
    if (OutputFilename != NULL) {
		OutputLog = fopen(OutputFilename, "w");
		if (!OutputLog) {
			fprintf(stderr, "Failed to open \"%s\"\n", OutputFilename);
			return false;
		}
	} else {
		OutputLog = stdout;
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
	// Set up the random number generation
	srand(time(NULL));
	
    // Parse arguments
    if (!parse(argc, argv)) {
        help(argc, argv);
        return EXIT_FAILURE;
    }
    
    // Set up the csv header row
    fprintf(OutputLog, "Turn,Carrier,Battleship,Submarine,Cruiser,Destroyer\n");
    
    // Play games
    FIELD field;
    for (int i = 0; i < NumberOfGames; i++) {
        // Initialize the field
        field_Clear(&field);
        field_CreateRandom(&field);

        // Play the game
        while (!field_IsWon(&field)) {
			if (!ai_PlayTurn(&field)) {
				eprintf("Failed to play the game.\n");
				return EXIT_FAILURE;
			}
		}

        // Log each game as csv
		fprintf(OutputLog, "%d,%d,%d,%d,%d,%d\n",
			field.turns,
			field.sinkTurn[CARRIER],
			field.sinkTurn[BATTLESHIP],
			field.sinkTurn[SUBMARINE],
			field.sinkTurn[CRUISER],
			field.sinkTurn[DESTROYER]
		);
    }
    
    // Clean up file
    fflush(OutputLog);
    fclose(OutputLog);
    return EXIT_SUCCESS;
}

/*============================================================*/
