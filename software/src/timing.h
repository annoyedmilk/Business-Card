// ===================================================================================
// Frame pacing
// ===================================================================================
// Both games come from a 16 MHz ATtiny85 and carry no frame timing of their
// own: one pass through the game loop is one step of the game, so unpaced,
// game speed is just however fast the hardware runs.
//
// frame_sync() holds each pass to a fixed wall-clock period, which keeps game
// speed independent of the system clock and of how long a frame took to draw.
//
// The two periods differ because the games do different work per pass: Pacman
// redraws every pass, Tetris every seventh.
//
// These are calibration values, not figures measured from an ATtiny. Tune
// them until the games feel right; larger is slower.
//
// Pacman sat at 12000 while its renderer was slow enough to overrun that
// budget every frame, so the deadline never bound and the game ran at whatever
// the draw happened to take. Cutting the per-pixel work made the deadline
// start binding, and Pacman got visibly faster on hardware as a result. 24000
// restores roughly the previous speed; adjust to taste.

#pragma once

#include <stdint.h>

#define FRAME_US_PACMAN 24000 // Pacman: one full redraw per pass
#define FRAME_US_TETRIS 2000  // Tetris: 7 passes per redraw

// Arm the deadline for the first frame of a loop.
void frame_start(uint32_t period_us);

// Wait until the current frame's time budget is used up, then arm the next.
void frame_sync(uint32_t period_us);
