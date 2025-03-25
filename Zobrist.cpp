#include "Zobrist.h"
#include <random>

uint64_t Zobrist::pieceHash[2][6][8][8];
uint64_t Zobrist::sideHash;

void Zobrist::init() {
    std::mt19937_64 rng(5489);

    for (int color = 0; color < 2; ++color) {
        for (int type = 0; type < 6; ++type) {
            for (int x = 0; x < 8; ++x) {
                for (int y = 0; y < 8; ++y) {
                    pieceHash[color][type][x][y] = rng();
                }
            }
        }
    }

    sideHash = rng();
}
