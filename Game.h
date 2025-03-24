#pragma once
#include "ChessPiece.h"
#include "Board.h"

class Game {
private:
    Board board;
    int cursorX, cursorY;
    bool whiteTurn;

public:
    Game();
    ~Game();

    void initialize();
    void run();

private:
    void draw();
    ChessPiece* selectFigure();
    void moveFigure(ChessPiece* ChessPiece);

    bool isCheck(bool white) const;
    bool isCheckmate(bool white);

    bool canCastle(ChessPiece* king, ChessPiece* rook, bool kingside);
    void performCastle(ChessPiece* king, bool kingside);
};
