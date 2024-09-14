This project is an advanced digital implementation of the classic strategy game Battleship developed in C, created as part of the CMPS 270 Software Construction course. The game offers both traditional gameplay and advanced features like radar sweeps, torpedo strikes, and more. The project is divided into two phases: a two-player console-based version in phase one, and a bot-based human vs. AI gameplay in phase two.

Features
Phase 1: Two-Player Mode
Grid Setup: Each player has a 10x10 grid where ships are placed. Ships include:
Carrier (5 cells)
Battleship (4 cells)
Destroyer (3 cells)
Submarine (2 cells)
Game Mechanics:
Players take turns guessing the location of the opponent's ships by firing at specific coordinates.
Special moves like Radar Sweeps, Smoke Screens, Artillery, and Torpedo are unlocked based on game conditions.
The game tracks hits and misses, with different levels of tracking difficulty.
Interactive Console: The game is fully console-based, with prompts guiding players through ship placement and each turn.



Phase 2: Human vs AI Bot
Bot Implementation: In phase two, a bot will replace one player, introducing a single-player experience. The bot uses strategic algorithms that perform better than random, making the game more challenging.
Difficulty Levels: Players can choose between Easy, Medium, and Hard bots, each with varying levels of strategy.
Bonus Features (Optional)
Difficulty Levels for the Bot: Add Easy, Medium, and Hard difficulty levels, with clear differences in the bot’s strategic behavior.
Battle Royale Tournament: Compete with other bots in a knockout tournament, with the top 3 bots receiving bonus points!


Project Structure
Battleship.c: Contains the core gameplay mechanics, including grid initialization, ship placement, and player turns.
Bot.c: Implements the AI bot logic, with different difficulty levels for phase two.
main.c: Entry point for the program, controlling the game flow and interaction.
README.md: This file, explaining the project setup, gameplay, and features.