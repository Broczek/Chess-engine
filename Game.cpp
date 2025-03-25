#include "Game.h"
#include "operations.h"
#include "King.h"
#include "Queen.h"
#include "Rook.h"
#include "Bishop.h"
#include "Knight.h"
#include "Pawn.h"
#include "Zobrist.h"
#include <conio.h>
#include <iostream>
#include <vector>


Game::Game() : cursorX(0), cursorY(0), whiteTurn(true) { Zobrist::init(); }

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
                    lastDoublePawnX = -1;
                    lastDoublePawnY = -1;
                    halfmoveClock++;
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

            bool isEnPassant = false;
            Pawn* movingPawn = dynamic_cast<Pawn*>(chessPiece);

            if (movingPawn) {
                int dir = chessPiece->isWhite() ? -1 : 1;

                if (abs(targetX - originalX) == 1 && targetY - originalY == dir && board.isEmpty(targetX, targetY)) {
                    if (targetX == lastDoublePawnX && targetY == lastDoublePawnY + dir) {
                        isEnPassant = true;
                    }
                }
            }

            if (chessPiece->canMove(targetX, targetY, board) || isEnPassant) {
                ChessPiece* captured = isEnPassant
                    ? board.getChessPiece(targetX, targetY + (chessPiece->isWhite() ? 1 : -1))
                    : board.getChessPiece(targetX, targetY);

                if (isEnPassant) {
                    delete board.getChessPiece(targetX, targetY + (chessPiece->isWhite() ? 1 : -1));
                    board.board[targetY + (chessPiece->isWhite() ? 1 : -1)][targetX] = nullptr;
                }

                board.moveChessPiece(originalX, originalY, targetX, targetY);

                if (isCheck(whiteTurn)) {
                    board.moveChessPiece(targetX, targetY, originalX, originalY);
                    board.board[targetY][targetX] = captured;
                    if (captured) captured->setPosition(targetX, targetY);
                    if (isEnPassant) {
                        board.board[targetY + (chessPiece->isWhite() ? 1 : -1)][targetX] = captured;
                    }

                    draw();
                    Operations::gotoxy(0, 10);
                    Operations::setTextColor(12);
                    std::cout << "Illegal move! Your king is still in check!";
                    _getch();
                }
                else {
                    chessPiece->setMoved(true);

                    if (movingPawn && abs(targetY - originalY) == 2) {
                        lastDoublePawnX = targetX;
                        lastDoublePawnY = targetY;
                    } else {
                        lastDoublePawnX = -1;
                        lastDoublePawnY = -1;
                    }

                    if (movingPawn || captured) {
                        halfmoveClock = 0;
                    } else {
                        halfmoveClock++;
                    }

                    if (movingPawn) {
                        if ((chessPiece->isWhite() && targetY == 0) || (!chessPiece->isWhite() && targetY == 7)) {
                            handlePawnPromotion(targetX, targetY, chessPiece->isWhite());
                        }
                    }

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
    updatePositionHistory();

    while (true) {
        if (isCheckmate(whiteTurn)) {
            draw();
            Operations::gotoxy(0, 10);
            Operations::setTextColor(12);
            std::cout << "Checkmate! " << (!whiteTurn ? "White" : "Black") << " wins!";
            _getch();
            break;
        }

        if (isStalemate(whiteTurn)) {
            draw();
            Operations::gotoxy(0, 10);
            Operations::setTextColor(14);
            std::cout << "Stalemate! It's a draw.";
            _getch();
            break;
        }

        if (isThreefoldRepetition()) {
            draw();
            Operations::gotoxy(0, 10);
            Operations::setTextColor(14);
            std::cout << "Threefold repetition! It's a draw.";
            _getch();
            break;
        }

        if (isInsufficientMaterial()) {
            draw();
            Operations::gotoxy(0, 10);
            Operations::setTextColor(14);
            std::cout << "Draw! Insufficient mating material.";
            _getch();
            break;
        }

        if (halfmoveClock >= 100) {
            draw();
            Operations::gotoxy(0, 10);
            Operations::setTextColor(14);
            std::cout << "Draw! 50-move rule: no pawn moves or captures in last 50 moves.";
            _getch();
            break;
        }
        
        ChessPiece* selected = selectFigure();
        moveFigure(selected);
        updatePositionHistory();

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

uint64_t Game::computeZobristHash() const {
    uint64_t hash = 0;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            ChessPiece* piece = board.getChessPiece(x, y);
            if (piece) {
                int color = piece->isWhite() ? 0 : 1;
                int type = piece->getTypeIndex();
                hash ^= Zobrist::pieceHash[color][type][x][y];
            }
        }
    }
    if (!whiteTurn)
        hash ^= Zobrist::sideHash;
    return hash;
}

void Game::updatePositionHistory() {
    uint64_t hash = computeZobristHash();
    positionHistory[hash]++;
}

bool Game::isThreefoldRepetition() const {
    uint64_t hash = computeZobristHash();
    auto it = positionHistory.find(hash);
    return it != positionHistory.end() && it->second >= 3;
}

bool Game::isStalemate(bool white) {
    if (isCheck(white))
        return false;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            ChessPiece* piece = board.getChessPiece(x, y);
            if (piece && piece->isWhite() == white) {
                for (int ty = 0; ty < 8; ++ty) {
                    for (int tx = 0; tx < 8; ++tx) {
                        if (piece->canMove(tx, ty, board)) {
                            ChessPiece* captured = board.getChessPiece(tx, ty);
                            board.moveChessPiece(x, y, tx, ty);
                            bool inCheck = isCheck(white);
                            board.moveChessPiece(tx, ty, x, y);
                            board.board[ty][tx] = captured;
                            if (captured) captured->setPosition(tx, ty);
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

bool Game::isInsufficientMaterial() const {
    int whiteKnights = 0, blackKnights = 0;
    std::vector<std::pair<int, int>> bishopPositions;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            ChessPiece* piece = board.getChessPiece(x, y);
            if (!piece) continue;

            if (dynamic_cast<King*>(piece)) continue;

            int type = piece->getTypeIndex();

            if (type == 3) {
                bishopPositions.emplace_back(x, y);
            }
            else if (type == 4) {
                piece->isWhite() ? whiteKnights++ : blackKnights++;
            }
            else {
                return false;
            }
        }
    }

    if (bishopPositions.empty() && whiteKnights == 0 && blackKnights == 0)
        return true;

    if (bishopPositions.empty() && whiteKnights + blackKnights == 1)
        return true;

    if (bishopPositions.size() == 1 && whiteKnights + blackKnights == 0)
        return true;

    if (!bishopPositions.empty() && whiteKnights + blackKnights == 0) {
        bool firstColor = (bishopPositions[0].first + bishopPositions[0].second) % 2;
        for (const auto& [x, y] : bishopPositions) {
            if ((x + y) % 2 != firstColor)
                return false;
        }
        return true;
    }

    return false;
}

void Game::handlePawnPromotion(int x, int y, bool isWhite) {
    while (true) {
        draw();
        Operations::gotoxy(0, 10);
        Operations::setTextColor(11);

        std::string queenIcon = isWhite ? "♛" : "♕";
        std::string rookIcon = isWhite ? "♜" : "♖";
        std::string bishopIcon = isWhite ? "♝" : "♗";
        std::string knightIcon = isWhite ? "♞" : "♘";

        std::cout << "Promote pawn to ("
                  << queenIcon << " - q, "
                  << rookIcon << " - r, "
                  << bishopIcon << " - b, "
                  << knightIcon << " - k): ";

        char choice = _getch();
        delete board.board[y][x];

        switch (choice) {
        case 'q':
            board.board[y][x] = new Queen(x, y, isWhite);
            return;
        case 'r':
            board.board[y][x] = new Rook(x, y, isWhite);
            return;
        case 'b':
            board.board[y][x] = new Bishop(x, y, isWhite);
            return;
        case 'k':
            board.board[y][x] = new Knight(x, y, isWhite);
            return;
        default:
            Operations::gotoxy(0, 11);
            Operations::setTextColor(12);
            std::cout << "Invalid choice! Use q, r, b, k";
            _getch();
            break;
        }
    }
}


