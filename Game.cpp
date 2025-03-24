#include "Game.h"
#include "operations.h"
#include "King.h"
#include <conio.h>
#include <iostream>

Game::Game() : cursorX(0), cursorY(0), whiteTurn(true) {}

Game::~Game() {}

void Game::initialize() {
    Operations::clearScreen();
    Operations::hideCursor();
    board.setupBoard();
}

void Game::draw() {
    board.drawBoard();
    Operations::gotoxy(0, 9);
    std::cout << (whiteTurn ? "White's turn" : "Black's turn");
}

ChessPiece* Game::selectFigure() {
    cursorX = 0;
    cursorY = 0;

    while (true) {
        draw();
        Operations::gotoxy(cursorX * 3 + 4, cursorY + 1);
        Operations::setTextColor(14);
        std::cout << "X";

        int key = _getch();

        if (key == 224) {
            int arrow = _getch();
            if (arrow == 72 && cursorY > 0) cursorY--;
            else if (arrow == 80 && cursorY < 7) cursorY++;
            else if (arrow == 75 && cursorX > 0) cursorX--;
            else if (arrow == 77 && cursorX < 7) cursorX++;
        }
        else if (key == 13) {
            ChessPiece* ChessPiece = board.getChessPiece(cursorX, cursorY);
            if (ChessPiece != nullptr && ChessPiece->isWhite() == whiteTurn)
                return ChessPiece;
        }
    }
}

void Game::moveFigure(ChessPiece* chessPiece) {
    int originalX = chessPiece->getX();
    int originalY = chessPiece->getY();
    int targetX = originalX, targetY = originalY;

    while (true) {
        draw();
        Operations::gotoxy(targetX * 3 + 4, targetY + 1);
        Operations::setTextColor(10);
        std::cout << "*";

        int key = _getch();

        if (key == 'q' && targetX > 0 && targetY > 0) { targetX--; targetY--; }
        else if (key == 'w' && targetY > 0) { targetY--; }
        else if (key == 'e' && targetX < 7 && targetY > 0) { targetX++; targetY--; }
        else if (key == 'a' && targetX > 0) { targetX--; }
        else if (key == 'd' && targetX < 7) { targetX++; }
        else if (key == 'z' && targetX > 0 && targetY < 7) { targetX--; targetY++; }
        else if (key == 'x' && targetY < 7) { targetY++; }
        else if (key == 'c' && targetX < 7 && targetY < 7) { targetX++; targetY++; }
        else if (key == 's') return;
        else if (key == 13) {
            int diffX = abs(targetX - chessPiece->getX());
        
            if (dynamic_cast<King*>(chessPiece) && diffX == 2 && targetY == chessPiece->getY()) {
                bool kingside = targetX > chessPiece->getX();
                ChessPiece* rook = board.getChessPiece(kingside ? 7 : 0, chessPiece->getY());
        
                if (canCastle(chessPiece, rook, kingside)) {
                    performCastle(chessPiece, kingside);
                    whiteTurn = !whiteTurn;
                    return;
                }
                else {
                    draw();
                    Operations::gotoxy(0, 10);
                    Operations::setTextColor(12);
                    std::cout << "Castling not possible!";
                    _getch();
                    return;
                }
            }
        
            if (chessPiece->canMove(targetX, targetY, board)) {
                ChessPiece* capturedChessPiece = board.getChessPiece(targetX, targetY);
                board.moveChessPiece(originalX, originalY, targetX, targetY);
        
                if (isCheck(whiteTurn)) {
                    board.moveChessPiece(targetX, targetY, originalX, originalY);
                    board.board[targetY][targetX] = capturedChessPiece;
                    if (capturedChessPiece) capturedChessPiece->setPosition(targetX, targetY);
        
                    draw();
                    Operations::gotoxy(0, 10);
                    Operations::setTextColor(12);
                    std::cout << "Illegal move! Your king is still in check!";
                    _getch();
                }
                else {
                    chessPiece->setMoved(true);
                    whiteTurn = !whiteTurn;
                    return;
                }
            }
        }   
    }
}

bool Game::isCheck(bool white) const {
    auto [kingX, kingY] = board.findKing(white);
    return board.isAttacked(kingX, kingY, !white);
}

bool Game::isCheckmate(bool white) {
    if (!isCheck(white))
        return false;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            ChessPiece* chessPiece = board.getChessPiece(x, y);
            if (chessPiece && chessPiece->isWhite() == white) {
                for (int ty = 0; ty < 8; ty++) {
                    for (int tx = 0; tx < 8; tx++) {
                        if (chessPiece->canMove(tx, ty, board)) {
                            ChessPiece* target = board.getChessPiece(tx, ty);
                            board.moveChessPiece(x, y, tx, ty);
                            bool inCheck = isCheck(white);
                            board.moveChessPiece(tx, ty, x, y);
                            board.board[ty][tx] = target;
                            if (target) target->setPosition(tx, ty);

                            if (!inCheck)
                                return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

void Game::run() {
    initialize();

    while (true) {
        if (isCheckmate(whiteTurn)) {
            draw();
            Operations::gotoxy(0, 10);
            Operations::setTextColor(12);
            std::cout << "Checkmate! " << (!whiteTurn ? "White" : "Black") << " wins!";
            _getch();
            break;
        }
        
        ChessPiece* selected = selectFigure();
        moveFigure(selected);

        if (isCheck(!whiteTurn)) {
            draw();
            Operations::gotoxy(0, 10);
            Operations::setTextColor(12);
            std::cout << (!whiteTurn ? "White" : "Black") << " is in check!";
            _getch();
        }
    }
}

bool Game::canCastle(ChessPiece* king, ChessPiece* rook, bool kingside) {
    if (!king || !rook) return false;

    if (king->hasMoved() || rook->hasMoved()) return false;

    int y = king->isWhite() ? 7 : 0;
    int kingX = king->getX();
    int rookX = kingside ? 7 : 0;
    int direction = kingside ? 1 : -1;

    for (int x = king->getX() + direction; x != rook->getX(); x += direction)
        if (!board.isEmpty(x, y)) return false;

    if (isCheck(king->isWhite())) return false;

    for (int x = king->getX(); x != king->getX() + (direction * 3); x += direction)
        if (board.isAttacked(x, king->getY(), !king->isWhite())) return false;

    if (king->hasMoved() || rook->hasMoved()) return false;

    return true;
}

void Game::performCastle(ChessPiece* king, bool kingside) {
    int y = king->isWhite() ? 7 : 0;
    ChessPiece* rook = board.getChessPiece(kingside ? 7 : 0, y);
    if (!rook) return;

    board.moveChessPiece(4, y, kingside ? 6 : 2, y);
    king->setMoved(true);

    board.moveChessPiece(kingside ? 7 : 0, y, kingside ? 5 : 3, y);
    rook->setMoved(true);
}


