#define WD6_MIN_SPEED 65

int wd6md_setup(void)
{
  g_wd6md_J1.name="J1";
  g_wd6md_J1.md=&g_md_J1;
  g_wd6md_J1.re=&g_wd6re_J1;
  tmr_init(&g_wd6md_J1.tmr_rpm,100);
  g_wd6md_J1.curr=&g_rar_J1;
  g_wd6md_J1.overloaded=0;
  
  g_wd6md_J2.name="J2";
  g_wd6md_J2.md=&g_md_J2;
  g_wd6md_J2.re=&g_wd6re_J2;
  tmr_init(&g_wd6md_J2.tmr_rpm,100);
  g_wd6md_J2.curr=&g_rar_J2;
  g_wd6md_J2.overloaded=0;
  
  g_wd6md_J3.name="J3";
  g_wd6md_J3.md=&g_md_J3;
  g_wd6md_J3.re=&g_wd6re_J3;
  tmr_init(&g_wd6md_J3.tmr_rpm,100);
  g_wd6md_J3.curr=&g_rar_J3;
  g_wd6md_J3.overloaded=0;

  g_wd6md_B1.name="B1";
  g_wd6md_B1.md=&g_md_B1;
  g_wd6md_B1.re=&g_wd6re_B1;
  tmr_init(&g_wd6md_B1.tmr_rpm,100);
  g_wd6md_B1.curr=&g_rar_B1;
  g_wd6md_B1.overloaded=0;
  
  g_wd6md_B2.name="B2";
  g_wd6md_B2.md=&g_md_B2;
  g_wd6md_B2.re=&g_wd6re_B2;
  tmr_init(&g_wd6md_B2.tmr_rpm,100);
  g_wd6md_B2.curr=&g_rar_B2;
  g_wd6md_B2.overloaded=0;
  
  g_wd6md_B3.name="B3";
  g_wd6md_B3.md=&g_md_B3;
  g_wd6md_B3.re=&g_wd6re_B3;
  tmr_init(&g_wd6md_B3.tmr_rpm,100);
  g_wd6md_B3.curr=&g_rar_B3;
  g_wd6md_B3.overloaded=0;
  
  tmr_init(&g_tmr_rmc,20);
  return(0);
}

int wd6md_setrpm(WD6MD *wd6md, int rpm, int dir)
{
  int sd,rpmd,stp;
  
  if(tmr_do(&wd6md->tmr_rpm) != 1) return(0);
  wd6md->curr->update();
  if(wd6md->re->rpm == rpm) return(0);
  if(wd6md->re->rpm < rpm) {
    sd=1;
//    wd6md->md->incSpeed(1,dir);
/*    
Serial.print(wd6md->name);
Serial.print("+");
Serial.print(g_millis);
Serial.print(" ");
Serial.print(wd6md->re->rpm);
Serial.print(" ");
Serial.println(wd6md->md->getSpeed());
*/
  } else {
/*    
Serial.print(wd6md->name);
Serial.print("-");
Serial.print(g_millis);
Serial.print(" ");
Serial.print(wd6md->re->rpm);
Serial.print(" ");
Serial.println(wd6md->md->getSpeed());
*/
    sd=-1;
//    wd6md->md->incSpeed(-1,dir);
  }
  rpmd=abs(wd6md->re->rpm-rpm);
  if(rpmd <= 1) stp=1;
  else if(rpmd <= 2) stp=2;
  else if(rpmd <= 3) stp=3;
  else if(rpmd <= 10) stp=rpmd*2;
  else stp=rpmd*3;
  stp=stp*sd;
  wd6md->md->incSpeed(stp,dir);

  return(1);
}

int wd6md_readcurrent(void)
{
  static char l_idx=0;
  WD6MD *wd6md;

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
  wd6md->curr->update();
  if(wd6md->curr->getRawValue() > 4000) {
    if(wd6md->overloaded < 10) wd6md->overloaded++;
  } else {
    wd6md->overloaded=0;
  }
  l_idx++;
  l_idx%=6;
} 

int  wd6md_setspeed1(int16_t ms, int16_t *ms_p, WD6MD *wd6md1, WD6MD *wd6md2, WD6MD *wd6md3)
{
  uint16_t rpm;
  int dir=1;

  if((ms >= 0) && (ms < WD6_MIN_SPEED)) ms=0;
  else if((ms < 0) && (-ms < WD6_MIN_SPEED)) ms=0;
  if(ms < 0) dir=-1;  
  if(*ms_p == g_cb_m1s) {
    if(*ms_p == 0) {
      wd6md1->md->setSpeed(0);
      wd6md2->md->setSpeed(0);
      wd6md3->md->setSpeed(0);
      return(0);
    }
    if((rpm=wd6md1->re->rpm) == 0) {
      if((rpm=wd6md2->re->rpm) == 0) {
        rpm=wd6md3->re->rpm;
      } 
//      wd6md_setrpm(wd6md1,rpm,dir);
    }
    wd6md_setrpm(wd6md1,rpm,dir);
    wd6md_setrpm(wd6md2,rpm,dir);
    wd6md_setrpm(wd6md3,rpm,dir);
    return(0);
  }
  wd6md1->md->setSpeed(ms);
  if(abs(wd6md2->md->getSpeed()-ms) > 5) {
    wd6md2->md->setSpeed(ms);
  }
  if(abs(wd6md3->md->getSpeed()-ms) > 5) {
    wd6md3->md->setSpeed(ms);
  }
  
  *ms_p=ms;
  return(0);
}

int  wd6md_setspeed(void)
{
  static int16_t l_m1s=0;
  static int16_t l_m2s=0;
  
  wd6md_setspeed1(g_cb_m1s,&l_m1s,&g_wd6md_J1,&g_wd6md_J2,&g_wd6md_J3);
  wd6md_setspeed1(g_cb_m2s,&l_m2s,&g_wd6md_B1,&g_wd6md_B2,&g_wd6md_B3);

  return(0);
}
