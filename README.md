# C++ Chess Engine

This is a console-based chess game written in C++, featuring a playable chess engine. You can play a full game of chess against a computer AI directly in your Windows console. The project implements all standard chess rules and a minimax-based AI for the computer's moves.

## Features

* **Full Chess Logic**: The engine correctly implements all the rules of chess:
    * Standard piece movements (Pawn, Rook, Knight, Bishop, Queen, King).
    * Check, checkmate, and stalemate detection.
    * Special moves: Castling, pawn promotion, and en passant.
* **Draw Detection**: The game ends in a draw under the following conditions:
    * Stalemate
    * Threefold repetition
    * Insufficient material
    * 50-move rule
* **Player vs. Computer**: Choose to play as either White or Black against the AI.
* **Game Logging**: Every game is automatically saved to a `.txt` file with algebraic notation, including the date, players, and result.

## AI Engine Details

The computer opponent is powered by a classic chess engine architecture:

* **Minimax Algorithm**: The core of the AI is a minimax search function to find the best possible move.
* **Alpha-Beta Pruning**: To optimize the minimax search, alpha-beta pruning is implemented to cut off branches of the search tree that will not influence the final decision.
* **Evaluation Function**: The position evaluation function considers several factors:
    * Material balance (based on standard piece values).
    * Piece mobility.
    * Development of pieces.
    * Control of the center.
    * King safety.
* **Zobrist Hashing**: Used for efficient position tracking and to implement a transposition table.
* **Transposition Table**: Stores evaluations of previously analyzed positions to avoid re-calculating them, speeding up the search.
* **Move Ordering**: Moves are sorted (e.g., captures first) to make alpha-beta pruning more effective.
