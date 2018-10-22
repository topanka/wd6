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
  loop_counter();
  
  wd6re_loop();
  
//  temp_read();

  wd6md_setrpm(&g_wd6md_J2,4);
  wd6md_setrpm(&g_wd6md_J3,5);
  wd6md_setrpm(&g_wd6md_B2,6);
  wd6md_setrpm(&g_wd6md_B3,7);

  wd6cumd_comm();
}
