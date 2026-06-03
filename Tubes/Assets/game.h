#ifndef GAME_H
#define GAME_H

#include "car.h"
#include "constants.h"

/* ── Tipe lane ──────────────────────────────────────────────────── */
typedef enum {
    LANE_GRASS = 0,
    LANE_ROAD  = 1
} LaneType;

/* ── Satu baris lane ────────────────────────────────────────────── */
typedef struct {
    LaneType type;
    float    z;          /* posisi tengah lane di dunia */
    int      scored;     /* sudah dihitung score? */
    float    markOffset; /* offset awal marka kuning (0.0 – 2.5) */

    /* Indeks mobil yang ada di lane ini (di dalam array cars[]) */
    int      carStart;   /* index pertama di cars[] */
    int      carCount;   /* jumlah mobil di lane ini */
} Lane;

/* ── Ayam ───────────────────────────────────────────────────────── */
typedef struct {
    float x, z;
    float bobAngle;
    int   alive;
} Chicken;

/* ── State dunia ────────────────────────────────────────────────── */
typedef struct {
    Lane  lanes[MAX_LANES];
    int   laneCount;          /* jumlah lane aktif saat ini */
    int   nextLaneIndex;      /* index lane berikutnya yang akan di-generate */

    Car   cars[MAX_CARS];
    int   numCars;
} World;

/* ── API ────────────────────────────────────────────────────────── */
float frandRange(float lo, float hi);

/* Inisialisasi world dari nol, ayam dimulai di lane index 0 */
void  initWorld(World *w, Chicken *chicken, int score);

/* Panggil tiap frame: generate lane baru di depan, hapus yg jauh di belakang */
void  updateWorld(World *w, Chicken *chicken, int *score);

/* Gerakkan semua mobil (wrap-around) */
void  updateCars(World *w);

/* Cek tabrakan ayam vs semua mobil aktif */
int   checkCollision(Chicken chicken, World *w);

/* Reset penuh (score di-reset juga dari caller) */
void  resetGame(Chicken *chicken, int *gameOver, int *score, World *w);

#endif
