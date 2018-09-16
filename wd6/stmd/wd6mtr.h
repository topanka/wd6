#ifndef __WD6MTR_H_INCLUDED__
#define __WD6MTR_H_INCLUDED__

#include "sh1tmr.h"

typedef struct tagWD6MTR {
  const char *name;
  VNH5019MD *md;
  WD6RE *re;
  MYTMR tmr_rpm;
} WD6MTR;

#endif /* __WD6MTR_H_INCLUDED__ */
