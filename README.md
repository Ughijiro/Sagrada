Sagrada-Inspired Dice Drafting Engine
This project is a simplified C implementation inspired by the board game Sagrada. It models core components of the game, including dice generation, player window grids with placement restrictions, and goal assignment.
Features
- Dice bag initialization with 90 dice (18 of each color), each assigned a number from 1 to 6.
- Shuffling mechanism for randomizing the dice bag.
- Player window grid (4x5) with support for number and color restrictions.
- Private goal assignment with unique color selection.
- Public goals stored as predefined strings.
- Dice pool display for drafting simulation.
- Die placement with validation against cell restrictions.
- File output for saving the state of the dice bag and player window.
- ANSI color codes for colored terminal output.

Notes
- The code includes basic structures for future expansion, such as scoring, adjacency rules, and multi-player support.
- It is intended as a learning or prototyping tool rather than a full game implementation.

How to compile:
gcc -o sagrada main.c
./sagrada

Sample Pattern File Format
The pattern file should contain 4 rows of 5 cells each. Each cell can be:
- 0 for no restriction
- 1 <number> for a number restriction (e.g., 1 5)
- 2 <color> for a color restriction (e.g., 2 R)
At the end of the file, include the number of helper gems.
Example:
0 1 3 2 R 0
2 G 0 1 5 0
0 0 2 B 1 2
1 6 0 0 2 Y
3




