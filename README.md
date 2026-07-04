# Raycast-based 3D ASCII Renderer

A simple renderer that creates the illusion of three dimensional space using ASCII characters of varying "brightness"

For example:
<img width="1159" height="705" alt="Screenshot 2026-07-04 at 11 06 37 AM" src="https://github.com/user-attachments/assets/5323bb0a-ba6b-4b0f-8af0-33516fa306b1" />
*An example scene with a cubic wall in front of the player and a minimap in the top left corner.*

## Features

- **Rendering of a simple text-based map**: Calculates distance from the first person player to coordinates on a predefined map to determine the brightness of each pixel
- **ncurses graphics**: Directly draws characters in the terminal
- **"Sprite" rendering**: "Sprite" used very loosely. Can render an ASCII "image" from a text file as an object in the 3D world (as a "billboard" that constantly faces the player)
- **Minimap**: A simple minimap is displayed in the corner to make it easier for the player to navigate around

## Build and Run

For best results, ensure your terminal is set to 11px font and enlarge the window to around 250x70 before running.

Build:
```bash
g++ main.cpp -lncurses -o build/build -std=c++17
```
Run:
```bash
build/build
```

## Future Development Plans
- **Makefile**: Set up Makefile for easier building.
- **Dialogue System**: Another ncurses window in the bottom of the screen to enable dialogue with NPCS or narration
- **Combat System**: RPG-style turn based combat with the enemies you encounter
- **Story** a full narrative for the game

