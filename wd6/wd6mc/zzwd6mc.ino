void loop_counter(void)
{
  g_millis=millis();
  g_loop_cnt++;
  if((g_millis-g_loop_ct) > 1000) {
    
    Serial.print("loopcps ");
    Serial.println(g_loop_cps);
//    Serial.print("fsY ");
//    Serial.println(g_cb_fsY);

/*    
    Serial.print("J1 curr: ");
    Serial.print(g_wd6md_J1.curr->getRawValue());
    Serial.print("\t");
    Serial.println(g_wd6md_J1.curr->getValue());    
    Serial.print("J2 curr: ");
    Serial.print(g_wd6md_J2.curr->getRawValue());
    Serial.print("\t");
    Serial.println(g_wd6md_J2.curr->getValue());    
*/    

    Serial.print(g_wd6md_J1.md->getSpeed());
    Serial.print(" ");
    Serial.print(g_wd6md_J2.md->getSpeed());
    Serial.print(" ");
    Serial.print(g_wd6md_J3.md->getSpeed());
    Serial.print(" ");
    Serial.print(g_wd6md_B1.md->getSpeed());
    Serial.print(" ");
    Serial.print(g_wd6md_B2.md->getSpeed());
    Serial.print(" ");
    Serial.println(g_wd6md_B3.md->getSpeed());
    
    
    Serial.print(g_wd6md_J1.re->rpm);
    Serial.print(" ");
    Serial.print(g_wd6md_J2.re->rpm);
    Serial.print(" ");
    Serial.print(g_wd6md_J3.re->rpm);
    Serial.print(" ");
    Serial.print(g_wd6md_B1.re->rpm);
    Serial.print(" ");
    Serial.print(g_wd6md_B2.re->rpm);
    Serial.print(" ");
    Serial.println(g_wd6md_B3.re->rpm);

    Serial.print(g_wd6md_J1.curr->getValue());
    Serial.print(" ");
    Serial.print(g_wd6md_J2.curr->getValue());
    Serial.print(" ");
    Serial.print(g_wd6md_J3.curr->getValue());
    Serial.print(" ");
    Serial.print(g_wd6md_B1.curr->getValue());
    Serial.print(" ");
    Serial.print(g_wd6md_B2.curr->getValue());
    Serial.print(" ");
    Serial.println(g_wd6md_B3.curr->getValue());

    
    Serial.print("m1s: ");
    Serial.print(g_cb_m1s);
    Serial.print(" m2s:");
    Serial.println(g_cb_m2s);    
   

    g_force_send=1;
    comm_send();
    
    g_loop_cps=g_loop_cnt;
    g_loop_cnt=0;
    g_loop_ct=g_millis;
  }
}

void setup()
{

//  delay(500);
  
digitalWrite(LED_BUILTIN,HIGH);
delay(200);
pinMode(LED_BUILTIN,OUTPUT);

  smar_setup();
//digitalWrite(LED_BUILTIN, HIGH);
//  batt_setup();
//  md_setup();
  comm_setup();
  
  Serial.begin(115200);

//digitalWrite(LED_BUILTIN, HIGH);
  
  g_md_J1.init();
  g_md_J2.init();
  g_md_J3.init();
  g_md_B1.init();
  g_md_B2.init();
  g_md_B3.init();
  vnh5019_initTimers();
  
  wd6re_setup();
  
  wd6md_setup();
  
  delay(1000);
  g_loop_ct=millis();
  Serial.println("Motor controller ready!");
}

void loop()
{
  int rpm,dir,speed;
  
  loop_counter();
  
  wd6re_loop();
  wd6md_readcurrent();
  
//  temp_read();

#if 0
  rpm=0;
  
  if((g_cb_fsY > 530) && (g_cb_fsY < 1024)) {
    rpm=map(g_cb_fsY,530,1024,4,65);
    dir=-1;
  } else if((g_cb_fsY < 500) && (g_cb_fsY >= 0)) {
    rpm=map(g_cb_fsY,0,500,65,4);
    dir=1;
  }

//  Serial.print("rpm: ");
//  Serial.println(rpm);

//  pwr=7;
//  dir=-1;
  

  if(rpm == 0) {
    g_wd6md_J1.md->setSpeed(0);
    g_wd6md_J2.md->setSpeed(0);
    g_wd6md_J3.md->setSpeed(0);
    g_wd6md_B1.md->setSpeed(0);
    g_wd6md_B2.md->setSpeed(0);
    g_wd6md_B3.md->setSpeed(0);
  } else {
    speed=400;
    g_wd6md_J1.md->setSpeed(speed);
    g_wd6md_J2.md->setSpeed(speed);
    g_wd6md_J3.md->setSpeed(speed);
    g_wd6md_B1.md->setSpeed(speed);
    g_wd6md_B2.md->setSpeed(speed);
    g_wd6md_B3.md->setSpeed(speed);
    
/*    
    wd6md_setrpm(&g_wd6md_J1,rpm,dir);
    wd6md_setrpm(&g_wd6md_J2,rpm,dir);
    wd6md_setrpm(&g_wd6md_J3,rpm,dir);
    wd6md_setrpm(&g_wd6md_B1,rpm,dir);
    wd6md_setrpm(&g_wd6md_B2,rpm,dir);
    wd6md_setrpm(&g_wd6md_B3,rpm,dir);
*/  
  }
#endif

//  wd6md_setspeedx();

  wd6md_setspeed();
  wd6cumd_comm();
}
