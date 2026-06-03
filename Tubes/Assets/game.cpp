#ifdef _WIN32
#include <windows.h>
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <math.h>

#include "game.h"
#include "constants.h"

float frandRange(float lo, float hi) {
    return lo + (float)rand() / RAND_MAX * (hi - lo);
}

void spawnCars(
    Car cars[],
    int *numCars,
    float roadLen,
    int score
) {
    int lane, i;

    *numCars = 0;

    for (lane = 1; lane <= 6 && *numCars < MAX_CARS; lane++) {

        float laneZ =
            -roadLen / 2.0f +
            lane * LANE_WIDTH +
            LANE_WIDTH / 2.0f;

        int n = 2 + rand() % 3;

        float dir =
            (lane % 2 == 0) ? 1.0f : -1.0f;

        float spd =
            CAR_SPEED_BASE *
            (0.8f + (rand() % 60) / 100.0f) *
            (1.0f + score * 0.05f);

        for (i = 0; i < n && *numCars < MAX_CARS; i++) {

            Car *c = &cars[(*numCars)++];

            c->z = laneZ;

            c->x =
                -8.0f +
                i * (16.0f / n) +
                frandRange(0, 2.0f);

            c->speed = spd * dir;

            c->r = 0.3f + frandRange(0, 0.7f);
            c->g = 0.3f + frandRange(0, 0.7f);
            c->b = 0.3f + frandRange(0, 0.7f);

            c->panjang =
                1.2f + (rand() % 4) / 10.0f;
        }
    }
}

void resetGame(
    Chicken *chicken,
    int *gameOver,
    int *gameWon,
    int *score,
    float *roadLen,
    Car cars[],
    int *numCars
) {
    *roadLen = NUM_LANES * LANE_WIDTH;

    chicken->x = 0.0f;
    chicken->z = -(*roadLen) / 2.0f + LANE_WIDTH / 2.0f;
    chicken->bobAngle = 0.0f;
    chicken->alive = 1;

    *gameOver = 0;
    *gameWon = 0;

    spawnCars(
        cars,
        numCars,
        *roadLen,
        *score
    );
}

int checkCollision(
    Chicken chicken,
    Car cars[],
    int numCars
) {
    int i;

    for (i = 0; i < numCars; i++) {

        float dx =
            (float)fabs(chicken.x - cars[i].x);

        float dz =
            (float)fabs(chicken.z - cars[i].z);

        if (
            dx < (cars[i].width / 2.0f + 0.35f)
            &&
            dz < 0.75f
        ) {
            return 1;
        }
    }

    return 0;
}

void updateCars(
    Car cars[],
    int numCars
) {
    int i;

    for (i = 0; i < numCars; i++) {

        cars[i].x += cars[i].speed;

        if (cars[i].x > 11.0f)
            cars[i].x = -11.0f;

        if (cars[i].x < -11.0f)
            cars[i].x = 11.0f;
    }
}