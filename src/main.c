/**********************************************************//**
 * @file main.c
 * @brief Controls the envionment and execution of battleship
 * games. Runs a certain number of games, and outputs the game
 * data to a file.
 * @author Rena Shinomiya
 * @date April 21, 2016
 **************************************************************/

#include <stdbool.h> 
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

#include "ai.h"
#include "debug.h"
#include "field.h"

/**************************************************************/
/// The number of games to play.
static int NumberOfGames = 1;

/// The output log file or NULL.
static FILE *OutputLog = NULL;

/// The game data log file, or NULL.
static FILE *GameLog = NULL;

/**********************************************************//**
 * @brief Print the help information to the terminal.
 * @param argc: Number of command-line arguments in argv.
 * @param argv: Command-line arguments from the terminal.
 **************************************************************/
static inline void help(int argc, char **argv) {
    (void)argc;
    printf("%s usage:\n", argv[0]);
    printf("-h:        Print the help screen.\n");
    printf("-o <name>: Write CSV data to the filename.\n");
    printf("-n <int>:  Play this number of games.\n");
    printf("-g <name>: Write game data to the filename.\n");
}

/**********************************************************//**
 * @brief Reads information from the command-line arguments and
 * stores it in static variables; used for configuration.
 * stores it in static variables; used for configuration.
 * @param argc: Number of command-line arguments in argv.
 * @param argv: Command-line arguments from the terminal.
 * @return True if no invalid keywords were encountered.
 * @detail Use -h to print the help screen; -n <int> to play
 * <int> number of games, and -o <name> to write to <name>
 * file.
 **************************************************************/
static inline bool parse(int argc, char *argv[]) {
    const char *outputFilename = NULL;
    const char *gameFilename = NULL;
    
    // Parse arguments
    int i = 1;
    while (i < argc) {
        // Get the current keyword symbol
        const char *keyword = argv[i++];
        if (!strcmp(keyword, "-n")) {
            NumberOfGames = atoi(argv[i++]);
        } else if (!strcmp(keyword, "-o")) {
            outputFilename = argv[i++];
        } else if (!strcmp(keyword, "-g")) {
            gameFilename = argv[i++];
        } else {
            // If -h is found, returns false so we print help
            // (this is a shortcut).
            return false;
        }
    }

    // Open the output file, or configure stdout.
    if (outputFilename != NULL) {
        OutputLog = fopen(outputFilename, "w");
        if (!OutputLog) {
            fprintf(stderr, "Failed to open \"%s\"\n", outputFilename);
            return false;
        }
    } else {
        OutputLog = stdout;
    }
    
    // Open the game log, or configure stdout.
    if (gameFilename != NULL) {
        GameLog = fopen(gameFilename, "w");
        if (!GameLog) {
            fprintf(stderr, "Failed to open \"%s\"\n", gameFilename);
            return false;
        }
    } else {
        GameLog = stdout;
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
    // If we can't parse the command-line arguments, print the help
    // screen and then stop.
    if (!parse(argc, argv)) {
        help(argc, argv);
        return EXIT_FAILURE;
    }

    // Set stuff up before we start logging games.,..
    srand(time(NULL));
    fprintf(OutputLog, "Turn,Carrier,Battleship,Submarine,Cruiser,Destroyer\n");
    for (int i=0; i<NumberOfGames; i++) {
        // Initialize the field
        FIELD field;
        field_Clear(&field);
        field_CreateRandom(&field);
        
        // Write each game to the game log.
        fprintf(GameLog, "# Game %d\n", i+1);

        // Have the AI take turns until the field is won.
        while (!field_IsWon(&field)) {
            if (!ai_PlayTurn(&field)) {
                eprintf("Failed to play the game.\n");
                return EXIT_FAILURE;
            }
            
            // Write each turn to the game log.
            fprintf(GameLog, "## Turn %d\n", field.turns);
            field_Print(&field, GameLog);
            fprintf(GameLog, "\n");
        }

        // Log each game as csv output
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
    fflush(GameLog);
    fclose(GameLog);
    return EXIT_SUCCESS;
}

/**************************************************************/
