#ifndef __WD6MD_H_INCLUDED__
#define __WD6MD_H_INCLUDED__

#include "sh1tmr.h"
#include "RunningAverage.h"

typedef struct tagWD6MDAM {
  uint8_t go;
  unsigned long ics;
  unsigned long ic;
} WD6MDAM;

typedef struct tagWD6MD {
  const char *name;
  VNH5019MD *md;
  WD6RE *re;
  RunningAverage *ra_mc;
  unsigned char overloaded;
  WD6MDAM am;
  MYTMR tmr_rpm;
} WD6MD;

typedef struct tagWD6MDTRACE {
  float dist;   // distance
  int msr;              // right speed
  int msl;              // left speed
} WD6MDTRACE;

#endif /* __WD6MD_H_INCLUDED__ */
