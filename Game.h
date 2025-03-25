#pragma once
#include "Board.h"
#include "Zobrist.h"
#include <unordered_map>
#include <cstdint>

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

    void draw();
    void moveFigure(ChessPiece* ChessPiece);
    void updatePositionHistory();
    void performCastle(ChessPiece* king, bool kingside);
    void handlePawnPromotion(int x, int y, bool isWhite);

    uint64_t computeZobristHash() const;

    int lastDoublePawnX = -1;
    int lastDoublePawnY = -1;
    int halfmoveClock = 0;

};
