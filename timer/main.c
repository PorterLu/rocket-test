#include "timer.h"
#include <stdio.h>
#include <stdint.h>

#define MTIMER_BASE 0x2004000
#define MTIMECMP MTIMER_BASE
#define MTIME (MTIMER_BASE + 0x7ff8)
#define MSWI_BASE 0x2000000
#define SSWI_BASE 0x2f00000
#define TIMER_INTERVAL 0x1000
#define MSTATUS_MIE (1 << 3)
#define MSTATUS_SIE (1 << 1)
#define MIE_MTIE (1 << 7)
#define MIE_MSIE (1 << 3)

typedef uint64_t reg_t;

static uint64_t _tick = 0;

static inline reg_t r_mstatus() {
  reg_t x;
  asm volatile("csrr %0, mstatus": "=r"(x));
  return x;
}

static inline void w_mstatus(reg_t x) {
  asm volatile("csrw mstatus, %0": :"r"(x));
}

static inline reg_t r_mie() {
  reg_t x;
  asm volatile("csrr %0, mie": "=r"(x));
  return x;
}

static inline void w_mie(reg_t x) {
  asm volatile("csrw mie, %0": : "r"(x));
}

void timer_load(int interval) {
  *(uint64_t*)(MTIMECMP) = *(uint64_t*)MTIME + interval;
  printf("set next interrupt|| mtime:%lx mtimecmp:%lx\n", *(uint64_t*)MTIME, *(uint64_t*)(MTIMECMP));
}

void init_time_interrupt() {
  timer_load(TIMER_INTERVAL);
  w_mie(r_mie() | MIE_MTIE);
  w_mstatus(r_mstatus() | MSTATUS_MIE);
}

void timer_handler() {
  _tick ++;
  printf("tick: %d\n", _tick);
  timer_load(TIMER_INTERVAL);
}

void software_interrupt_test() {
  printf("set software interrupt\n");
  w_mie(r_mie() | MIE_MSIE);
  *(uint32_t*)MSWI_BASE = 1;
}

void software_handler() {
  printf("clear software interrupt\n");
  *(uint32_t*)MSWI_BASE = 0;
}

int main() {
  software_interrupt_test();
  init_time_interrupt();
  while(1) {
    //printf("mtime:%lx mtimecmp:%lx\n", *(uint64_t*)MTIME, *(uint64_t*)(MTIMECMP));
  }
}
