void setup() {
  g_millis=millis();
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(500000);
  delay(1000);
  Serial.println("motor test");
  
  g_md_J3.init();
  g_md_B3.init();
  vnh5019_initTimers();
  
  wd6re_setup();
  
  stmdmtr_setup();
  
}

void loop() {
  g_millis=millis();

  wd6re_loop();
  
  // put your main code here, to run repeatedly:
  if(Serial2.available() > 0) {
    g_rb=Serial2.read();
    g_rb+=100;
    Serial2.write((byte*)&g_rb,1);
  }

  stmdmtr_setrpm(&g_mtrJ3,3);
  stmdmtr_setrpm(&g_mtrB3,7);

/*
  if((g_millis-g_chmst) > 4000) {
    g_chmst=g_millis;
    g_msn++;
    g_msn%=3;
  }
  if((g_msn == 0) && (g_msn != g_mso)) {
    Serial.println("60");
    g_mtrB3.md->setSpeed(60);
    g_mtrJ3.md->setSpeed(60);
    g_mso=g_msn;
  } else if((g_msn == 1) && (g_msn != g_mso)) {
    Serial.println("0");
    g_mtrB3.md->setSpeed(0);
    g_mtrJ3.md->setSpeed(0);
    g_mso=g_msn;
  } else if((g_msn == 2) && (g_msn != g_mso)) {
    Serial.println("100");
    g_mtrB3.md->setSpeed(100);
    g_mtrJ3.md->setSpeed(100);
    g_mso=g_msn;
  }
*/  
//  delay(2000);
  
}
