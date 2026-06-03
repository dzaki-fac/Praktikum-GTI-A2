#ifdef _WIN32
#include <windows.h>
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "game.h"
#include "constants.h"

/* ------------------------------------------------------------------ */
float frandRange(float lo, float hi) {
    return lo + (float)rand() / (float)RAND_MAX * (hi - lo);
}

/* ── Spawn mobil untuk satu lane road ──────────────────────────── */
static void spawnCarsForLane(World *w, int laneIdx, int score) {
    Lane *lane = &w->lanes[laneIdx];

    if (lane->type != LANE_ROAD) {
        lane->carStart = 0;
        lane->carCount = 0;
        return;
    }

    /* slot tersedia di cars[] */
    if (w->numCars >= MAX_CARS) {
        lane->carStart = 0;
        lane->carCount = 0;
        return;
    }

    int n = 2 + rand() % 3;
    if (w->numCars + n > MAX_CARS)
        n = MAX_CARS - w->numCars;

    float dir = (rand() % 2 == 0) ? 1.0f : -1.0f;
    float spd = CAR_SPEED_BASE
                * (0.8f + (rand() % 60) / 100.0f)
                * (1.0f + score * 0.04f);

    lane->carStart = w->numCars;
    lane->carCount = n;

    int i;
    for (i = 0; i < n; i++) {
        Car *c = &w->cars[w->numCars++];

        c->z      = lane->z;
        c->x      = -8.0f + i * (16.0f / n) + frandRange(0, 1.5f);
        c->speed  = spd * dir;
        c->r      = 0.3f + frandRange(0, 0.7f);
        c->g      = 0.3f + frandRange(0, 0.7f);
        c->b      = 0.3f + frandRange(0, 0.7f);
        c->panjang = 1.2f + (rand() % 4) / 10.0f;
        c->width  = 0.9f;
    }
}

/* ── Generate satu lane baru di posisi Z tertentu ─────────────── */
/*  isSpawn = 1 → paksa LANE_GRASS tanpa mobil (lane tempat ayam muncul) */
static void generateLane(World *w, float z, int score, int isSpawn) {
    if (w->laneCount >= MAX_LANES) return;

    Lane *lane = &w->lanes[w->laneCount];
    lane->z        = z;
    lane->scored   = 0;
    lane->markOffset = frandRange(0.0f, 2.5f);

    if (isSpawn) {
        /* Lane spawn: selalu rumput, langsung tandai scored agar tidak +1 */
        lane->type   = LANE_GRASS;
        lane->scored = 1;
    } else {
        int prevIsRoad  = (w->laneCount > 0 &&
                           w->lanes[w->laneCount - 1].type == LANE_ROAD);
        int prevPrevIsRoad = (w->laneCount > 1 &&
                              w->lanes[w->laneCount - 2].type == LANE_ROAD);
        int prevIsGrass = (w->laneCount > 0 &&
                           w->lanes[w->laneCount - 1].type == LANE_GRASS);

        float prob = ROAD_LANE_PROB;

        if (prevIsRoad && prevPrevIsRoad)
            prob = 0.2f;        /* max 2 road berturut-turut, paksa jeda */
        else if (prevIsGrass)
            prob = 1.0f;        /* setelah rumput WAJIB road (no 2 grass berturut) */

        lane->type = ((float)rand() / (float)RAND_MAX < prob)
                     ? LANE_ROAD : LANE_GRASS;
    }

    spawnCarsForLane(w, w->laneCount, score);
    w->laneCount++;
}

/* ── Hapus lane-lane lama di belakang ayam ─────────────────────── */
/*
 * Strategi: geser array lanes[] dan rebuild cars[].
 * Karena MAX_LANES dan MAX_CARS tidak terlalu besar ini masih O(n) wajar.
 */
static void pruneLanesBehind(World *w, float chickenZ) {
    /* Cari index lane paling belakang yang masih perlu di-render */
    int keepFrom = 0;
    int i;
    for (i = 0; i < w->laneCount; i++) {
        if (w->lanes[i].z >= chickenZ - LANES_BEHIND * LANE_WIDTH) {
            keepFrom = i;
            break;
        }
        keepFrom = i + 1;
    }

    if (keepFrom == 0) return;   /* tidak ada yang perlu dihapus */

    /* Geser lanes */
    int newCount = w->laneCount - keepFrom;
    for (i = 0; i < newCount; i++)
        w->lanes[i] = w->lanes[i + keepFrom];
    w->laneCount = newCount;

    /* Rebuild cars[] dari lane yang tersisa */
    w->numCars = 0;
    for (i = 0; i < w->laneCount; i++) {
        Lane *lane = &w->lanes[i];
        if (lane->type != LANE_ROAD || lane->carCount == 0) {
            lane->carStart = 0;
            lane->carCount = 0;
            continue;
        }
        /* carStart/carCount sudah betul relatif terhadap posisi lama;
           kita re-spawn agar data mobil (x, speed, warna) tetap ada.
           Simpan dulu data mobil sebelum rebuild. */
        /* Catatan: data mobil sudah hilang karena kita hapus,
           jadi cukup spawn ulang — posisi x akan random lagi.
           Untuk mempertahankan posisi, perlu copy dulu. */
        lane->carStart = 0;
        lane->carCount = 0;
    }
    /* Re-spawn semua lane road yang tersisa dengan posisi baru */
    /* Biar tidak terlalu acak: simpan dulu snapshot mobil sebelum prune */
    /* (implementasi simpel: spawn ulang — trade-off diterima) */
}

/*
 * Versi yang lebih baik: simpan snapshot cars sebelum geser.
 * Kita gunakan pendekatan copy-then-rebuild.
 */
static void pruneLanesBehindFull(World *w, float chickenZ) {
    int keepFrom = w->laneCount; /* default: tidak ada yang dihapus */
    int i;

    for (i = 0; i < w->laneCount; i++) {
        if (w->lanes[i].z >= chickenZ - LANES_BEHIND * LANE_WIDTH) {
            keepFrom = i;
            break;
        }
    }

    if (keepFrom == 0) return;

    /* Snapshot mobil per lane yang akan dipertahankan */
    /* Kita simpan seluruh cars[] sementara */
    Car   tmpCars[MAX_CARS];
    int   tmpCount = w->numCars;
    memcpy(tmpCars, w->cars, sizeof(Car) * tmpCount);

    /* Geser lanes */
    int newLaneCount = w->laneCount - keepFrom;
    Lane tmpLanes[MAX_LANES];
    memcpy(tmpLanes, w->lanes + keepFrom, sizeof(Lane) * newLaneCount);
    memcpy(w->lanes, tmpLanes, sizeof(Lane) * newLaneCount);
    w->laneCount = newLaneCount;

    /* Rebuild cars[] dari snapshot */
    w->numCars = 0;
    for (i = 0; i < w->laneCount; i++) {
        Lane *lane = &w->lanes[i];
        if (lane->type != LANE_ROAD || lane->carCount == 0) continue;

        int oldStart = lane->carStart;
        int n        = lane->carCount;

        /* Pastikan index lama masih valid */
        if (oldStart + n > tmpCount) {
            lane->carStart = 0;
            lane->carCount = 0;
            continue;
        }

        lane->carStart = w->numCars;
        int j;
        for (j = 0; j < n && w->numCars < MAX_CARS; j++) {
            w->cars[w->numCars++] = tmpCars[oldStart + j];
        }
        lane->carCount = w->numCars - lane->carStart;
    }
}

/* ── initWorld ─────────────────────────────────────────────────── */
void initWorld(World *w, Chicken *chicken, int score) {
    int i;

    memset(w, 0, sizeof(World));

    float startZ = 0.0f;   /* ayam muncul di Z=0 */

    /* 5 lane rumput di belakang spawn — pre-fill array dari belakang,
       lalu geser supaya urutan Z tetap ascending */
    for (i = 5; i >= 1; i--) {
        Lane *back  = &w->lanes[w->laneCount];
        back->z      = startZ - i * LANE_WIDTH;
        back->type   = LANE_GRASS;
        back->scored = 1;   /* tidak menambah score */
        back->carStart = 0;
        back->carCount = 0;
        w->laneCount++;
    }

    /* Lane spawn: rumput aman, langsung scored */
    generateLane(w, startZ, score, 1);

    /* Lane di depan ayam */
    for (i = 1; i <= LANES_AHEAD; i++)
        generateLane(w, startZ + i * LANE_WIDTH, score, 0);

    /* Ayam berdiri di lane spawn */
    chicken->x        = 0.0f;
    chicken->z        = startZ;
    chicken->bobAngle = 0.0f;
    chicken->alive    = 1;
}

/* ── updateWorld ───────────────────────────────────────────────── */
void updateWorld(World *w, Chicken *chicken, int *score) {
    int i;

    /* ① Cek score: tiap lane yang baru dilewati ayam */
    for (i = 0; i < w->laneCount; i++) {
        Lane *lane = &w->lanes[i];
        if (!lane->scored && chicken->z >= lane->z) {
            lane->scored = 1;
            (*score)++;
        }
    }

    /* ② Generate lane baru di depan jika kurang */
    float frontZ = -99999.0f;
    for (i = 0; i < w->laneCount; i++)
        if (w->lanes[i].z > frontZ)
            frontZ = w->lanes[i].z;

    while (frontZ < chicken->z + LANES_AHEAD * LANE_WIDTH
           && w->laneCount < MAX_LANES) {
        frontZ += LANE_WIDTH;
        generateLane(w, frontZ, *score, 0);
    }

    /* ③ Buang lane yang terlalu jauh di belakang */
    pruneLanesBehindFull(w, chicken->z);
}

/* ── updateCars ────────────────────────────────────────────────── */
void updateCars(World *w) {
    int i;
    for (i = 0; i < w->numCars; i++) {
        w->cars[i].x += w->cars[i].speed;
        if (w->cars[i].x >  11.0f) w->cars[i].x = -11.0f;
        if (w->cars[i].x < -11.0f) w->cars[i].x =  11.0f;
    }
}

/* ── checkCollision ────────────────────────────────────────────── */
int checkCollision(Chicken chicken, World *w) {
    int i;
    for (i = 0; i < w->numCars; i++) {
        float dx = (float)fabs(chicken.x - w->cars[i].x);
        float dz = (float)fabs(chicken.z - w->cars[i].z);
        if (dx < (w->cars[i].panjang / 2.0f + 0.35f) && dz < 0.75f)
            return 1;
    }
    return 0;
}

/* ── resetGame ─────────────────────────────────────────────────── */
void resetGame(Chicken *chicken, int *gameOver, int *score, World *w) {
    *gameOver = 0;
    *score    = 0;
    initWorld(w, chicken, 0);
}
