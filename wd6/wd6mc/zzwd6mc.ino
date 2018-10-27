//MAX RPM 65

void loop_counter(void)
{
  g_millis=millis();
  g_loop_cnt++;
  if((g_millis-g_loop_ct) > 1000) {
    
    Serial.print("loopcps ");
    Serial.println(g_loop_cps);
    Serial.print("fsY ");
    Serial.println(g_cb_fsY);

    
    Serial.print("J1 curr: ");
    Serial.print(g_wd6md_J1.curr->getRawValue());
    Serial.print("\t");
    Serial.println(g_wd6md_J1.curr->getValue());    
    Serial.print("J2 curr: ");
    Serial.print(g_wd6md_J2.curr->getRawValue());
    Serial.print("\t");
    Serial.println(g_wd6md_J2.curr->getValue());    
    
    
    g_loop_cps=g_loop_cnt;
    g_loop_cnt=0;
    g_loop_ct=g_millis;
  }
}

void setup()
{
  delay(1000);
  smar_setup();
//  batt_setup();
//  md_setup();
  comm_setup();
  
  Serial.begin(115200);

  g_md_J1.init();
  g_md_J2.init();
  g_md_J3.init();
  g_md_B1.init();
  g_md_B2.init();
  g_md_B3.init();
  vnh5019_initTimers();
  
  wd6re_setup();
  
  wd6md_setup();
  
  delay(2000);
  g_loop_ct=millis();
}

void loop()
{
  int pwr,dir;
  
  loop_counter();
  
  wd6re_loop();
//  wd6md_readcurrent();
  
//  temp_read();

  pwr=0;
  
  if(g_cb_fsY > 530) {
    pwr=map(g_cb_fsY,530,4,1024,65);
    dir=1;
  } else if(g_cb_fsY < 500) {
    pwr=map(g_cb_fsY,0,65,500,4);
    dir=-1;
  }
  

  pwr=6;
  dir=1;
/*
  if(pwr == 0) {
    g_wd6md_J1.md->setSpeed(0);
    g_wd6md_J2.md->setSpeed(0);
    g_wd6md_J3.md->setSpeed(0);
    g_wd6md_B1.md->setSpeed(0);
    g_wd6md_B2.md->setSpeed(0);
    g_wd6md_B3.md->setSpeed(0);
  } else {
*/  
    wd6md_setrpm(&g_wd6md_J1,pwr,dir);
    wd6md_setrpm(&g_wd6md_J2,pwr,dir);
    wd6md_setrpm(&g_wd6md_J3,pwr,dir);
    wd6md_setrpm(&g_wd6md_B1,pwr,dir);
    wd6md_setrpm(&g_wd6md_B2,pwr,dir);
    wd6md_setrpm(&g_wd6md_B3,pwr,dir);
//  }

  wd6cumd_comm();
}
