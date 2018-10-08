#include "wd6re.h"

#define WD6RE_ICNC_TIME    400        //interrupt counter not changed, zero speed

void wd6re_setup() {
  // put your setup code here, to run once:
  g_wd6re_J1.g_wd6re_tbi=1;
  g_wd6re_J2.g_wd6re_tbi=1;
  g_wd6re_J3.g_wd6re_tbi=1;
  g_wd6re_B1.g_wd6re_tbi=1;
  g_wd6re_B2.g_wd6re_tbi=1;
  g_wd6re_B3.g_wd6re_tbi=1;
  g_wd6re_J1.g_wd6re_num_tbi=WD6RE_TBI_NUM;
  g_wd6re_J2.g_wd6re_num_tbi=WD6RE_TBI_NUM;
  g_wd6re_J3.g_wd6re_num_tbi=WD6RE_TBI_NUM;
  g_wd6re_B1.g_wd6re_num_tbi=WD6RE_TBI_NUM;
  g_wd6re_B2.g_wd6re_num_tbi=WD6RE_TBI_NUM;
  g_wd6re_B3.g_wd6re_num_tbi=WD6RE_TBI_NUM;
}

uint16_t qe_rpm_tbi(WD6RE *wd6re)
{
  unsigned long tbi;
  unsigned long atbi;
  int i;


/*
  Serial.print("xx ");
  Serial.print(l_r0,DEC);
  Serial.print(" ");
  Serial.println(g_wd6re_ic1,DEC);
*/

  if(wd6re->l_r0 == wd6re->ic) {
    if(g_millis > wd6re->l_t0+WD6RE_ICNC_TIME) {
      wd6re->l_sum=0;
      wd6re->l_tbi[0]=0;
      wd6re->l_idx=0;
      wd6re->l_rpm=0;
      return(wd6re->l_rpm);
    } else {
      return(wd6re->l_rpm);
    }
  } else {
    wd6re->l_t0=g_millis;
    wd6re->l_r0=wd6re->ic;
    noInterrupts();
    tbi=wd6re->g_wd6re_tbi;
    interrupts();
  }

/*
  Serial.print("xx ");
  Serial.print(l_r0,DEC);
  Serial.print(" ");
  Serial.print(g_wd6re_ic1,DEC);
  Serial.print(" ");
  Serial.print(dtx,DEC);
  Serial.print(" ");
  Serial.println(g_millis-l_t0,DEC);
*/

  if(tbi > 1000UL*WD6RE_ICNC_TIME) {
     wd6re->l_sum=0;
     wd6re->l_tbi[0]=0;
     wd6re->l_idx=0;
     wd6re->l_rpm=0;
     return(wd6re->l_rpm);
  }
  wd6re->l_sum+=tbi;
  
  wd6re->l_idx++;
  wd6re->l_idx%=wd6re->g_wd6re_num_tbi;
  if(wd6re->l_tbi[0] == 0) {
    if(wd6re->l_idx == 0) {
      atbi=wd6re->l_sum/wd6re->g_wd6re_num_tbi;
    } else {
      atbi=wd6re->l_sum/wd6re->l_idx;
    }
  } else {
    wd6re->l_sum-=wd6re->l_tbi[wd6re->l_idx];
    atbi=wd6re->l_sum/wd6re->g_wd6re_num_tbi;
  }
  wd6re->l_tbi[wd6re->l_idx]=tbi;


/*
  for(i=0;i < WD6RE_TBI_NUM;i++) {
    Serial.print(l_tbi[i]);
    Serial.print(" ");
  }
   Serial.println("");
*/   
   
  
/*  
  Serial.print("1b ");
  Serial.print(dt,DEC);
  Serial.print(" ");
  Serial.print(dtx,DEC);
  Serial.print(" ");
  Serial.print(l_sum,DEC);
  Serial.print(" ");
  Serial.println(atbi,DEC);
*/  

  wd6re->l_rpm=1000000UL/atbi;
//  l_rpm=2000000UL/atbi;
  return(wd6re->l_rpm);
}

void wd6re_loop()
{
  g_wd6re_B3.rpm=qe_rpm_tbi(&g_wd6re_B3);
  if(g_wd6re_B3.rpm != g_wd6re_B3.rpmo) {
    
    Serial.print(g_millis);
    Serial.print(" rpm=");
    Serial.print(g_wd6re_B3.rpm);
    Serial.print(" ");
    Serial.println(g_wd6re_J3.rpm);
    
    g_wd6re_B3.rpmo=g_wd6re_B3.rpm;
  }

  if(g_wd6re_B3.ico != g_wd6re_B3.ic) {
//    Serial.println(g_wd6re_ic1);
    g_wd6re_B3.ico=g_wd6re_B3.ic;
  }

  g_wd6re_J3.rpm=qe_rpm_tbi(&g_wd6re_J3);
  if(g_wd6re_J3.rpm != g_wd6re_J3.rpmo) {
    
    Serial.print(g_millis);
    Serial.print(" rpm=");
    Serial.print(g_wd6re_B3.rpm);
    Serial.print(" ");
    Serial.println(g_wd6re_J3.rpm);
    
    g_wd6re_J3.rpmo=g_wd6re_J3.rpm;
  }

  if(g_wd6re_J3.ico != g_wd6re_J3.ic) {
//    Serial.println(g_wd6re_ic1);
    g_wd6re_J3.ico=g_wd6re_J3.ic;
  }

  
}

void wd6re_isrJ1(void)
{
  static volatile unsigned long l_t0=0;
  unsigned long mm;
  
  g_wd6re_J1.ic++;
  mm=micros();
  g_wd6re_J1.g_wd6re_tbi=mm-l_t0;
  l_t0=mm;
}

void wd6re_isrJ2(void)
{
  static volatile unsigned long l_t0=0;
  unsigned long mm;
  
  g_wd6re_J2.ic++;
  mm=micros();
  g_wd6re_J2.g_wd6re_tbi=mm-l_t0;
  l_t0=mm;
}

void wd6re_isrJ3(void)
{
  static volatile unsigned long l_t0=0;
  unsigned long mm;
  
  g_wd6re_J3.ic++;
  mm=micros();
  g_wd6re_J3.g_wd6re_tbi=mm-l_t0;
  l_t0=mm;
}

void wd6re_isrB1(void)
{
  static volatile unsigned long l_t0=0;
  unsigned long mm;
  
  g_wd6re_B1.ic++;
  mm=micros();
  g_wd6re_B1.g_wd6re_tbi=mm-l_t0;
  l_t0=mm;
}

void wd6re_isrB2(void)
{
  static volatile unsigned long l_t0=0;
  unsigned long mm;
  
  g_wd6re_B2.ic++;
  mm=micros();
  g_wd6re_B2.g_wd6re_tbi=mm-l_t0;
  l_t0=mm;
}

void wd6re_isrB3(void)
{
  static volatile unsigned long l_t0=0;
  unsigned long mm;
  
  g_wd6re_B3.ic++;
  mm=micros();
  g_wd6re_B3.g_wd6re_tbi=mm-l_t0;
  l_t0=mm;
}
