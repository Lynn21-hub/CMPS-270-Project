The Battleship Bot project is a robust command-line implementation of the Battleship game that offers a challenging and adaptive AI opponent. The bot employs a dynamic decision-making system built around multiple strategies, such as heatmap-based targeting, radar sweeps, and a phased approach that includes Hunt, Target, and Lock Direction phases.

Key Features:
Heatmap Targeting: The bot uses a heatmap to prioritize cells based on the likelihood of containing ships, dynamically updating probabilities after every hit, miss, and radar result.
Radar Sweeps: The bot performs radar sweeps to detect ships within a 2x2 grid, intelligently focusing on high-probability areas and marking irrelevant zones to prevent redundant actions.
Phased Decision-Making: The bot transitions between phases to optimize its strategy:
Hunt Phase: Searches for ships using heatmap probabilities.
Target Phase: Focuses attacks on a specific ship after detecting a hit, adding surrounding cells to a pending attack list.
Lock Direction Phase: Once two consecutive hits are aligned, the bot locks onto a direction to sink the ship efficiently.
Utility Functions: Functions like addAdjacentCells, adjustHeatmapOnMiss, and determineDirection enable the bot to handle complex decision-making scenarios, ensuring adaptability and responsiveness.
Advanced Moves: Abilities like artillery strikes and smoke screens introduce unique strategic elements, making gameplay more dynamic. Artillery targets 2x2 areas with maximum probability, while smoke screens obstruct radar sweeps.
Error Handling and Debugging: The bot includes detailed debugging messages for insights into decision-making processes, ensuring transparency and easier troubleshooting during development.
This project is highly modular, making it easy to modify or extend functionalities. From strategic gameplay to adaptive intelligence, the Battleship Bot delivers an engaging and competitive experience for players.

