/**********************************************************//**
 * @file ai.c
 * @brief Implementation of Battleship AI
 * @author Rena Shinomiya
 * @date April 3, 2017
 **************************************************************/

#include <limits.h>
#include <math.h>
#include <stdbool.h>

#include "ai.h"
#include "debug.h"
#include "field.h"

/**********************************************************//**
 * @brief Get the length of the longest ship remaining.
 * @param field: The field to check.
 * @param full: Output parameter for the minimum length
 * that is not next to any hits.
 * @param partial: Output parameter for the minimum length
 * that is next to some hits.
 * @return The length of the longest ship that could be left
 * on the field.
 **************************************************************/
static inline void ai_GetMinimumLength(const FIELD *field, int *full, int *partial) {
    // Get the minimum length remaining
    int lengthMin = INT_MAX;
    int partialMin = INT_MAX;
    for (SHIP ship=0; ship<N_SHIPS; ship++) {
        // Check if the ship is actually afloat.
        int health = field_GetShipHealth(field, ship);
        if (health > 0) {
            // Get the minimum fragment size if the ship is hit.
            // A "fragment" is the continuous piece of a ship that
            // we HAVE NOT found out information for yet.
            int length = field_GetShipLength(ship);
            if (health < length) {
                // Longest fragment is when everything is in
                // the middle of the ship.
                // I.E. if we have a ship of length 5, that was hit
                // two times, we could have:
                // OXOXO (fragment length of 1)
                // OXXOO (fragment length 1 and 2)
                // XXOOO (fragment length of 3)
                // Minimum fragment length is 1 == (int)log2(5-2)
                //
                // If we have a ship of length 5 that was hit once:
                // XOOOO (fragment length 4)
                // OXOOO (fragment length 3 and 1)
                // OOXOO (fragment length 2)
                // Minimum fragment length is 2 == (int)log2(5-1). It is
                // NOT 1 because that also exists alongside a fragment of 3.
                int fragmentMin = (int)log2(length-health);
                if (fragmentMin == 0) {
                    fragmentMin = 1;
                }

                // Update minimum length for a ship that has hits (the partial).
                if (fragmentMin < partialMin) {
                    partialMin = fragmentMin;
                }
            } else if (length < lengthMin) {
                // An unhit ship remains. Update minimum for a ship with no hits.
                lengthMin = length;
                if (lengthMin < partialMin) {
                    partialMin = lengthMin;
                }
            }
        }
    }

    // Output parameters. Invariant that partialMin <= lengthMin.
    assert(partialMin <= lengthMin);
    if (full) {
        *full = lengthMin;
    }
    if (partial) {
        *partial = partialMin;
    }
}

/**********************************************************//**
 * @brief Play one turn of a game.
 * @param field: The field to take a turn on.
 * @return Whether the gameplay succeeded.
 **************************************************************/
bool ai_PlayTurn(FIELD *field) {
    int fullMin;
    int partialMin;
    ai_GetMinimumLength(field, &fullMin, &partialMin);

    // Assign probability of finding a new "hit" at each tile, and
    // choose the tile with the most probability.
    int probabilityMax = -1;
    int tileX = -1;
    int tileY = -1;
    for (int x=0; x<FIELD_SIZE; x++) {
        for (int y=0; y<FIELD_SIZE; y++) {
            // Skip tiles we already tried (essentially assigns probability
            // of -1 to that tile, which skips it).
            if (field_GetStatus(field, x, y) != UNTRIED) {
                continue;
            }

            // Calculate view extents from the current tile
            // These are all guaranteed >= 1 if the tile is UNTRIED.
            // We are looking for the number of untried tiles that grow
            // left, right, up, and down from our current tile.
            int viewLeft  = field_GetExtent(field, LEFT,  x, y, UNTRIED);
            int viewRight = field_GetExtent(field, RIGHT, x, y, UNTRIED);
            int viewUp    = field_GetExtent(field, UP,    x, y, UNTRIED);
            int viewDown  = field_GetExtent(field, DOWN,  x, y, UNTRIED);
            assert(viewLeft >= 1);
            assert(viewRight >= 1);
            assert(viewUp >= 1);
            assert(viewDown >= 1);

            // Find any nearby hits, beginning at our neighbors
            // and extending outwards.
            int nearLeft  = field_GetExtent(field, LEFT,  x-1, y,   HIT);
            int nearRight = field_GetExtent(field, RIGHT, x+1, y,   HIT);
            int nearUp    = field_GetExtent(field, UP,    x,   y-1, HIT);
            int nearDown  = field_GetExtent(field, DOWN,  x,   y+1, HIT);
            assert(nearLeft >= 0);
            assert(nearRight >= 0);
            assert(nearUp >= 0);
            assert(nearDown >= 0);

            // If we are actually next to a hit, we need to use the partialMin, which
            // is always less than or equal to the fullMin. partialMin means "the minimum
            // number of continuous unhit tiles on a ship we have already hit". fullMin
            // means "the the minimum size of an unhit ship".
            //
            // Example: if we have sunk 4 ships and all that's left is the submarine (of
            // length 3, partialMin==fullMin==3). That means in the situation below:
            // XXXX
            // X??X
            // XXXX
            // It's meaningless to pick the ? tiles because the submarine couldn't
            // possibly fit.
            //
            // However, if it's the same situation but we hit some of the submarine...
            // XXXX
            // X??O
            // XXXX
            // We could find the submarine at ?. This is because partialMin is now 1.
            int nearHorizontal = nearLeft + nearRight;
            int nearVertical = nearUp + nearDown;
            bool blockedHorizontal;
            if (nearLeft > 0 || nearRight > 0) {
                blockedHorizontal = (viewRight+viewLeft) <= (partialMin-nearHorizontal);
            } else {
                blockedHorizontal = (viewRight+viewLeft) <= (fullMin-nearHorizontal);
            }
            bool blockedVertical;
            if (nearUp > 0 || nearDown > 0) {
                blockedVertical = (viewUp+viewDown) <= (partialMin-nearVertical);
            } else {
                blockedVertical = (viewUp+viewDown) <= (fullMin-nearVertical);
            }

            // Determine the probability at the tile. If the ship couldn't possibly fit
            // at the tile, probability is zero.
            int probability = 0;
            if (!blockedHorizontal || !blockedVertical) {
                // Weight probability towards the center. I.e. in the following situation:
                // X???X
                // We want to pick the middle ? above the left or right ? because picking
                // the middle completely rules out if a ship of length 2 exists there.
                // Ex: it could be XO[O]XX or XX[O]OX. Picking the middle would always be
                // [O] but picking the left or right could be X.
                probability = viewLeft*viewRight + viewUp*viewDown;
                // Weight a lot if near to other hits. FIELD_SIZE*FIELD_SIZE is the max
                // probability, which weights tiles next to hits significantly higher.
                // This means if we get a hit, we pursue that ship until it sinks.
                probability += (nearHorizontal+nearVertical)*(FIELD_SIZE*FIELD_SIZE);
            }
            assert(probability >= 0);

            // Check probability maximum, and pick the best one.
            if (probability > probabilityMax) {
                probabilityMax = probability;
                tileX = x;
                tileY = y;
            }
        }
    }

    // Sanity check before making the attack
    assert(tileX != -1);
    assert(tileY != -1);
    assert(field_GetStatus(field, tileX, tileY) == UNTRIED);

    // Make attack
    STATUS result = field_Attack(field, tileX, tileY);
    if (result == ERROR) {
        eprintf("Attack at (%d, %d) failed.\n", tileX, tileY);
        return false;
    }

    // Sanity check after attacking
    assert(field_GetStatus(field, tileX, tileY) != UNTRIED);
    assert(field_GetStatus(field, tileX, tileY) == result);
    return true;
}

/**************************************************************/
