#ifndef CONSTANTS_H
#define CONSTANTS_H

/* Window */
const int WINDOW_W = 900;
const int WINDOW_H = 700;

/* Jalan */
const float LANE_WIDTH = 2.0f;

/* Jumlah lane yang di-render sekaligus (depan + belakang ayam) */
const int LANES_AHEAD  = 10;   /* lane yang di-generate di depan ayam */
const int LANES_BEHIND = 4;    /* lane yang tetap di-render di belakang */

/* Total slot lane maksimum dalam array (circular-ish buffer) */
const int MAX_LANES = 64;

/* Mobil */
const int MAX_CARS = 60;
const float CAR_SPEED_BASE = 0.03f;

/* Probabilitas lane adalah jalan (vs rumput), 0.0–1.0 */
const float ROAD_LANE_PROB = 0.65f;

/* Math */
const float PI = 3.14159265358979f;

#endif
