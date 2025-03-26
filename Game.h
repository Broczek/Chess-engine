#pragma once
#include "Board.h"
#include "Zobrist.h"
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <chrono>

class ChessPiece;

struct Move {
    int fromX, fromY;
    int toX, toY;
    ChessPiece* movedPiece = nullptr;
    ChessPiece* capturedPiece = nullptr;
};

class Game {
    public:
    Game();
    ~Game();

    void initialize();
    void run();

private:
    Board board;
    ChessPiece* selectFigure();
    
    bool whiteTurn;
    bool isCheck(bool white) const;
    bool isCheckmate(bool white);
    bool isStalemate(bool white);
    bool isThreefoldRepetition() const;
    bool canCastle(ChessPiece* king, ChessPiece* rook, bool kingside);
    bool isInsufficientMaterial() const;
    bool playerIsWhite = true;
    bool timeUp = false;

    void draw();
    void chooseSide();
    void moveFigure(ChessPiece* ChessPiece);
    void updatePositionHistory();
    void performCastle(ChessPiece* king, bool kingside);
    void handlePawnPromotion(int x, int y, bool isWhite);
    void startLogging();
    void logMove(const std::string& move);
    void saveGameResult(const std::string& result);
    void makeComputerMove();

    std::ofstream gameLog;
    std::vector<std::string> moveHistory;
    std::vector<Move> generateSortedMoves(bool forComputer);
    std::string getAlgebraicNotation(ChessPiece* piece, int toX, int toY, bool capture);
    std::unordered_map<uint64_t, int> positionHistory;
    std::unordered_map<uint64_t, int> transpositionTable;
    std::chrono::steady_clock::time_point searchStartTime;

    uint64_t computeZobristHash() const;
    
    int cursorX, cursorY;
    int evaluatePosition(const Board& board) const;
    int minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer, bool isRoot = false);

    int lastDoublePawnX = -1;
    int lastDoublePawnY = -1;
    int halfmoveClock = 0;
    int maxSearchDepth = 4;
    int timeLimitMillis = 30000;

    Move getBestMove(int depth);
};
