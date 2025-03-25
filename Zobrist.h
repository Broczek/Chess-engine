#pragma once
#include <cstdint>

class Zobrist {
public:
    static uint64_t pieceHash[2][6][8][8];
    static uint64_t sideHash;

    static void init();
};