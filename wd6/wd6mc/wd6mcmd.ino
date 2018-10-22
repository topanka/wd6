int wd6md_setup(void)
{
  g_wd6md_J1.name="J1";
  g_wd6md_J1.md=&g_md_J1;
  g_wd6md_J1.re=&g_wd6re_J1;
  tmr_init(&g_wd6md_J1.tmr_rpm,100);
  
  g_wd6md_J2.name="J2";
  g_wd6md_J2.md=&g_md_J2;
  g_wd6md_J2.re=&g_wd6re_J2;
  tmr_init(&g_wd6md_J2.tmr_rpm,100);
  
  g_wd6md_J3.name="J3";
  g_wd6md_J3.md=&g_md_J3;
  g_wd6md_J3.re=&g_wd6re_J3;
  tmr_init(&g_wd6md_J3.tmr_rpm,100);

  g_wd6md_B1.name="B1";
  g_wd6md_B1.md=&g_md_B1;
  g_wd6md_B1.re=&g_wd6re_B1;
  tmr_init(&g_wd6md_B1.tmr_rpm,100);
  
  g_wd6md_B2.name="B2";
  g_wd6md_B2.md=&g_md_B2;
  g_wd6md_B2.re=&g_wd6re_B2;
  tmr_init(&g_wd6md_B2.tmr_rpm,100);
  
  g_wd6md_B3.name="B3";
  g_wd6md_B3.md=&g_md_B3;
  g_wd6md_B3.re=&g_wd6re_B3;
  tmr_init(&g_wd6md_B3.tmr_rpm,100);
  
  return(0);
}

int wd6md_setrpm(WD6MD *wd6md, int rpm)
{
  if(tmr_do(&wd6md->tmr_rpm) != 1) return(0);
  if(wd6md->re->rpm == rpm) return(0);
  if(wd6md->re->rpm < rpm) {
    wd6md->md->incSpeed(1);
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
    wd6md->md->incSpeed(-1);
  }

  return(1);
}
