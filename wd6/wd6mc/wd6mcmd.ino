#define WD6_MIN_NZ_SPEED        ((WD6_MIN_SPEED)+4)

#define WD6_MIN_RPM   6
#define WD6_MAX_RPM   90

int wd6md_setup(void)
{
  g_wd6md_J1.name="J1";
  g_wd6md_J1.md=&g_md_J1;
  g_wd6md_J1.re=&g_wd6re_J1;
  tmr_init(&g_wd6md_J1.tmr_rpm,100);
  g_wd6md_J1.ra_mc=&g_wd6md_J1_mc;
  g_wd6md_J1.overloaded=0;
  
  g_wd6md_J2.name="J2";
  g_wd6md_J2.md=&g_md_J2;
  g_wd6md_J2.re=&g_wd6re_J2;
  tmr_init(&g_wd6md_J2.tmr_rpm,100);
  g_wd6md_J2.ra_mc=&g_wd6md_J2_mc;
  g_wd6md_J2.overloaded=0;
  
  g_wd6md_J3.name="J3";
  g_wd6md_J3.md=&g_md_J3;
  g_wd6md_J3.re=&g_wd6re_J3;
  tmr_init(&g_wd6md_J3.tmr_rpm,100);
  g_wd6md_J3.ra_mc=&g_wd6md_J3_mc;
  g_wd6md_J3.overloaded=0;

  g_wd6md_B1.name="B1";
  g_wd6md_B1.md=&g_md_B1;
  g_wd6md_B1.re=&g_wd6re_B1;
  tmr_init(&g_wd6md_B1.tmr_rpm,100);
  g_wd6md_B1.ra_mc=&g_wd6md_B1_mc;
  g_wd6md_B1.overloaded=0;
  
  g_wd6md_B2.name="B2";
  g_wd6md_B2.md=&g_md_B2;
  g_wd6md_B2.re=&g_wd6re_B2;
  tmr_init(&g_wd6md_B2.tmr_rpm,100);
  g_wd6md_B2.ra_mc=&g_wd6md_B2_mc;
  g_wd6md_B2.overloaded=0;
  
  g_wd6md_B3.name="B3";
  g_wd6md_B3.md=&g_md_B3;
  g_wd6md_B3.re=&g_wd6re_B3;
  tmr_init(&g_wd6md_B3.tmr_rpm,100);
  g_wd6md_B3.ra_mc=&g_wd6md_B3_mc;
  g_wd6md_B3.overloaded=0;
  
  tmr_init(&g_tmr_rmc,20);
  return(0);
}

int wd6md_setrpm(WD6MD *wd6md, int rpm, int dir)
{
  int sd,rpmd,stp;
  
  if(tmr_do(&wd6md->tmr_rpm) != 1) return(0);
  if(wd6md->re->rpm == rpm) return(0);

/*
  if(wd6md->re->rpm == 0){
    if(wd6md->tmr_rpm.cnt%2 == 0) return(0);
  }
*/  
  
//Serial.print(wd6md->name);
  if(wd6md->re->rpm < rpm) {
    sd=1;
//Serial.print("+");
  } else {
    sd=-1;
//Serial.print("-");
  }
 
  rpmd=wd6md->re->rpm-rpm;
  rpmd=abs(rpmd);
//Serial.print(rpmd);
//Serial.print(" ");

  if(wd6md->re->rpm == 0) {
    stp=4;
  } else {
    if(rpmd <= 1) stp=1;
    else if(rpmd <= 2) stp=2;
    else if(rpmd <= 3) stp=4;
    else if(rpmd <= 10) stp=rpmd*2;
    else stp=rpmd*3;
  }
  
  stp=stp*sd;

/*
Serial.print(wd6md->re->rpm);
Serial.print(" ");
Serial.print(rpm);
Serial.print(" ");
Serial.print(stp);
Serial.print(" ");
Serial.println(wd6md->md->getSpeed());
*/
  
  wd6md->md->incSpeed(stp,dir);

  return(1);
}

int wd6md_readcurrent(void)
{
  static char l_idx=0;
  WD6MD *wd6md;
  unsigned int mc;

  if(tmr_do(&g_tmr_rmc) != 1) return(0);
  if(l_idx == 0) {
    wd6md=&g_wd6md_J1;  
  } else if(l_idx == 1) {
    wd6md=&g_wd6md_J2;  
  } else if(l_idx == 2) {
    wd6md=&g_wd6md_J3;  
  } else if(l_idx == 3) {
    wd6md=&g_wd6md_B1;  
  } else if(l_idx == 4) {
    wd6md=&g_wd6md_B2;  
  } else if(l_idx == 5) {
    wd6md=&g_wd6md_B3;  
  } else return(-1);
  mc=wd6md->md->getCurrentMilliamps();  
  wd6md->ra_mc->addValue(mc);
  if(wd6md->ra_mc->getAverage() > 4000) {
    if(wd6md->overloaded < 10) wd6md->overloaded++;
  } else {
    wd6md->overloaded=0;
  }
  l_idx++;
  l_idx%=6;

  return(0);
} 

int  wd6md_setspeed1(int16_t ms, int16_t *ms_p, WD6MD *wd6md1, WD6MD *wd6md2, WD6MD *wd6md3)
{
  uint16_t rpm;
  int dir=1,msd;

  if((ms >= 0) && (ms < WD6_MIN_SPEED)) ms=0;
  else if((ms < 0) && (-ms < WD6_MIN_SPEED)) ms=0;
  if(ms < 0) dir=-1;  
  msd=ms-*ms_p;
  msd=abs(msd);
//  if(*ms_p == ms) {
  if(msd <= 1) {
    if(*ms_p == 0) {
      wd6md1->md->setSpeed(0);
      wd6md2->md->setSpeed(0);
      wd6md3->md->setSpeed(0);
      *ms_p=ms;
      return(0);
    }
    if((rpm=wd6md1->re->rpm) == 0) {
      if((rpm=wd6md2->re->rpm) == 0) {
        rpm=wd6md3->re->rpm;
      } 
      if(rpm < WD6_MIN_RPM) rpm=WD6_MIN_RPM;
    }
    if(abs(ms) <= WD6_MIN_NZ_SPEED) {
      rpm=WD6_MIN_RPM;
    }
//    Serial.print("RPM: ");
//    Serial.println(rpm);
    wd6md_setrpm(wd6md1,rpm,dir);
    wd6md_setrpm(wd6md2,rpm,dir);
    wd6md_setrpm(wd6md3,rpm,dir);
//    *ms_p=ms;
    return(0);
  }

//  if((ms > *ms_p) && (wd6md1->md->getSpeed() >= ms)) return(0);
//  if((ms < *ms_p) && (wd6md1->md->getSpeed() <= ms)) return(0);
  
  wd6md1->md->setSpeed(ms);
  msd=wd6md2->md->getSpeed()-ms;
  msd=abs(msd);
  if(msd > 4) {
    wd6md2->md->setSpeed(ms);
  }
  msd=wd6md3->md->getSpeed()-ms;
  msd=abs(msd);
  if(msd > 4) {
    wd6md3->md->setSpeed(ms);
  }
  
  *ms_p=ms;
  return(0);
}

int wd6md_am_go(unsigned long dist, int16_t msr, int16_t msl)
{
  WD6MD *wd6md1;
  long x;
  uint8_t ic;
  
  if(g_wd6md_am_go == 0) {
    wd6md_setspeed(0,0);
    return(0);
  }
  wd6md1=&g_wd6md_J2;
  if(wd6md1->am.go == 0) {
    wd6md1->am.go=1;
    wd6md1->am.ics=1;
    wd6md1->am.ic=wd6md1->re->ic;
  } else {
    ic=wd6md1->re->ic;
    x=(long)ic-wd6md1->am.ic;
    if(x < 0) x+=256;
    wd6md1->am.ic=ic;
    wd6md1->am.ics+=x;
  }
  wd6md_setspeed(msr,msl);
  if(wd6md1->am.ics >= (abs(dist)/1.57)) {
    wd6md_setspeed(0,0);
    wd6md1->am.go=0;
    g_wd6md_am_go=0;
    return(1);
  }

  return(0);
}

int wd6md_am_turn(unsigned long turn, int16_t ms)
{
  unsigned long dist;

  dist=3;
  if(ms >= 0) {
    wd6md_am_go(dist,ms,-ms);
  } else {
    wd6md_am_go(dist,-ms,ms);
  }

  return(0);
}

int wd6md_am(void)
{
  int ret;
  WD6MDTRACE tr[3]={
    {300.0,400,400},
    {200.0,-400,-400},
    {100.0,-150,-150}
//    {30,-100,-100}
  };
  
  if(g_cb_b6pBE == 11) {
    g_wd6md_am=1;
    g_wd6md_am_go=1;
    g_wd6md_am_goidx=0;
    return(0);
  }      
  if(g_cb_b6pBE == 41) {
    g_wd6md_am=0;
    g_wd6md_am_go=0;
    g_wd6md_am_goidx=-1;
    return(0);
  }      
  if(g_wd6md_am == 0) return(0);
  if((g_wd6md_am_goidx >= sizeof(tr)/sizeof(tr[0])) || (g_wd6md_am_goidx < 0)) {
    g_wd6md_am=0;
    g_wd6md_am_go=0;
    g_wd6md_am_goidx=-1;
    return(0);
  }
  ret=wd6md_am_go(tr[g_wd6md_am_goidx].dist,tr[g_wd6md_am_goidx].msr,tr[g_wd6md_am_goidx].msl);
  if(ret == 1) {
    g_wd6md_am_go=1;
    g_wd6md_am_goidx++;
    delay(100);
  } 

  return(0);
}

int wd6md_setspeed(int16_t msr, int16_t msl)
{
  static int16_t l_m1s=0;
  static int16_t l_m2s=0;

  wd6md_setspeed1(msr,&l_m1s,&g_wd6md_J1,&g_wd6md_J2,&g_wd6md_J3);
  wd6md_setspeed1(msl,&l_m2s,&g_wd6md_B1,&g_wd6md_B2,&g_wd6md_B3);

  return(0);
}

int  wd6md_mm(void)
{
  if((g_wd6md_am != 0) && ((g_cb_m1s != 0) || (g_cb_m2s != 0))) {
    g_wd6md_am=0;
    g_wd6md_am_go=0;
    g_wd6md_am_goidx=-1;
    wd6md_setspeed(0,0);
    return(0);
  }
  if(g_wd6md_am == 1) return(0);
  
  wd6md_setspeed(g_cb_m1s,g_cb_m2s);

  return(0);
}

int  wd6md_setspeedx(void)
{
  static int16_t l_m1s=0;
  static int16_t l_m2s=0;
  static unsigned long l_millis=0;
  static int ms=65;

  if(g_millis <= l_millis+2000) return(0);
  l_millis=g_millis;
  wd6md_setspeed1(ms,&l_m1s,&g_wd6md_J1,&g_wd6md_J2,&g_wd6md_J3);
  wd6md_setspeed1(ms,&l_m2s,&g_wd6md_B1,&g_wd6md_B2,&g_wd6md_B3);
  g_cb_m1s=ms;
  ms++;
  
  return(0);
}
