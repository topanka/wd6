int stmdmtr_setup(void)
{
  g_mtrJ3.name="J3";
  g_mtrJ3.md=&g_md_J3;
  g_mtrJ3.re=&g_wd6re_J3;
  tmr_init(&g_mtrJ3.tmr_rpm,100);
  
  g_mtrB3.name="B3";
  g_mtrB3.md=&g_md_B3;
  g_mtrB3.re=&g_wd6re_B3;
  tmr_init(&g_mtrB3.tmr_rpm,100);
  
  return(0);
}

int stmdmtr_setrpm(WD6MTR *mtr, int rpm)
{
  if(tmr_do(&mtr->tmr_rpm) != 1) return(0);
  if(mtr->re->rpm == rpm) return(0);
  if(mtr->re->rpm < rpm) {
    mtr->md->incSpeed(1);
    
Serial.print(mtr->name);
Serial.print("+");
Serial.print(g_millis);
Serial.print(" ");
Serial.print(mtr->re->rpm);
Serial.print(" ");
Serial.println(mtr->md->getSpeed());

  } else {
    
Serial.print(mtr->name);
Serial.print("-");
Serial.print(g_millis);
Serial.print(" ");
Serial.print(mtr->re->rpm);
Serial.print(" ");
Serial.println(mtr->md->getSpeed());

    mtr->md->incSpeed(-1);
  }

  return(1);
}
