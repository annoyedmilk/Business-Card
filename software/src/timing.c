#include "timing.h"
#include "ch32fun.h"

// Deadline for the current frame, in free-running SysTick ticks.
static uint32_t frame_deadline;

void frame_start(uint32_t period_us)
{
  frame_deadline = funSysTick32() + Ticks_from_Us(period_us);
}

void frame_sync(uint32_t period_us)
{
  uint32_t period = Ticks_from_Us(period_us);
  // If we overran by more than a whole period, give up on catching up and
  // resynchronise, otherwise a slow frame makes the next few run flat out.
  if ((int32_t)(funSysTick32() - frame_deadline) > (int32_t)period)
    frame_deadline = funSysTick32();
  else
    while ((int32_t)(funSysTick32() - frame_deadline) < 0)
      ;
  frame_deadline += period;
}
