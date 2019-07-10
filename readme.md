# Battleship AI
This project uses an AI heuristic to play the game Battleship.

## Support
This project obeys the gnu99 standard. It can be compiled with `gcc` or `MinGW`, of version `4.8.1` and above.

## Usage
```
battleship.exe -h           // Prints the help screen (this)
battleship.exe -n <number>  // Plays <number> of different games.
battleship.exe -g <file>    // Stores game logging information in the file.
battleship.exe -o <file>    // Stores statistical information in a file.
```

The statistical information file `-o` contains the total turn count, followed by the sink turn for the carrier, battleship, cruiser, submarine, and destroyer. This allows you to track how efficient the AI is. It displays in CSV format.

The game information file `-g` shows each choice made on each turn for every game. It displays in markdown format.

### Ships
The game is played on a 10x10 grid with five ships: the carrier (length 5), battleship (length 4), submarine and cruiser (length 3), and destroyer (length 2).

### Heuristic
The AI always seeks to pick the middlemost untried tile. If `X` represents a known miss and `?` represents an unknown in this situation: `X???X`, the AI will pick the middle `?`. This heuristic is applied in both dimensions of the game board, since ships can be horizontal or vertical.

This heuristic exists because all ships have a length greater than 1; hence, we are always looking to fragment up space as much as possible using our guesses. The benefit is illustrated as such: consider the following situation: `X???X`. If the destroyer (length 2) is horizontal in this space, it must occupy the middle tile. However, if we pick the left or right `?` tile, we could miss. Since the goal is to win in as few turns as possible, we want to know if the destroyer is anywhere in the unknown space as soon as possible.

This heuristic requires us to know a "longest ship remaining" length. If the longest ship left is length 3, we can ignore all the unknowns in the following field:
```
XXXX
X??X
X??X
XXXX
```
This is because of the pigeonhole principle.

The "longest ship remaining" becomes harder to define if there is a ship we've hit, but haven't sunk. In this case, we truly could be looking for exactly one tile, and not a length of tiles. In this case we weight the area in line with the longest ship that hasn't sunk. For example, in `X?OOX` where the ship represented by `O` hasn't sunk yet, we can conclude the rest of the ship is certainly at `?`. This applies in both dimensions.

Through combining all these situations, a greedy algorithm runs with the following hierarchy:
1. Pick tiles in line with a row of other hits that did NOT yet sink a ship.
2. Pick tiles next to other hits that did NOT sink a ship.
3. Pick tiles in the middle of the widest open area (in both dimensions).

It ignores:
- Tiles where a ship couldn't fit, considering which ships are sunk and which ships we hit but didn't sink yet.
- Tiles we already tried.

### Conclusions
You must hit every ship to win the game, so a perfect game requires 17 hits. The worst possible game takes every turn, so 100 tries.

In 10,000 trials, the AI won, on average,in 44 turns. The best game was won in 19 turns, and the worst game was won in 72 turns. The average game boasts a hit rate of 38%, which is significantly better than the 17% rate for guessing randomly.
