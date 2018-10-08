#ifndef __WD6MD_H_INCLUDED__
#define __WD6MD_H_INCLUDED__

#include "sh1tmr.h"

typedef struct tagWD6MD {
  const char *name;
  VNH5019MD *md;
  WD6RE *re;
  MYTMR tmr_rpm;
} WD6MD;

#endif /* __WD6MD_H_INCLUDED__ */
