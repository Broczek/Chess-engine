#pragma once
#include "Board.h"
#include "Zobrist.h"
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>

class ChessPiece;

class Game {
private:
    Board board;
    int cursorX, cursorY;
    bool whiteTurn;
    std::unordered_map<uint64_t, int> positionHistory;

public:
    Game();
    ~Game();

    void initialize();
    void run();

private:
    ChessPiece* selectFigure();
    
    bool isCheck(bool white) const;
    bool isCheckmate(bool white);
    bool isStalemate(bool white);
    bool isThreefoldRepetition() const;
    bool canCastle(ChessPiece* king, ChessPiece* rook, bool kingside);
    bool isInsufficientMaterial() const;
    bool playerIsWhite = true;

    void draw();
    void chooseSide();
    void moveFigure(ChessPiece* ChessPiece);
    void updatePositionHistory();
    void performCastle(ChessPiece* king, bool kingside);
    void handlePawnPromotion(int x, int y, bool isWhite);
    void startLogging();
    void logMove(const std::string& move);
    void saveGameResult(const std::string& result);

    std::ofstream gameLog;
    std::vector<std::string> moveHistory;
    std::string getAlgebraicNotation(ChessPiece* piece, int toX, int toY, bool capture);
    uint64_t computeZobristHash() const;

    int lastDoublePawnX = -1;
    int lastDoublePawnY = -1;
    int halfmoveClock = 0;

};
