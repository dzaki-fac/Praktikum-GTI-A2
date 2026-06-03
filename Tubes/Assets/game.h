#ifndef GAME_H
#define GAME_H

#include "car.h"

typedef struct {
    float x, z;
    float bobAngle;
    int alive;
} Chicken;

float frandRange(float lo, float hi);

void spawnCars(
    Car cars[],
    int *numCars,
    float roadLen,
    int score
);

void resetGame(
    Chicken *chicken,
    int *gameOver,
    int *gameWon,
    int *score,
    float *roadLen,
    Car cars[],
    int *numCars
);

int checkCollision(
    Chicken chicken,
    Car cars[],
    int numCars
);

void updateCars(
    Car cars[],
    int numCars
);

#endif