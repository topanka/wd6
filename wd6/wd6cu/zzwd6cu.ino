void loop_counter(void)
{
  g_loop_cnt++;
  if((g_millis-g_loop_ct) > 1000) {
    g_loop_cps=g_loop_cnt;
    g_loop_cnt=0;
    g_loop_ct=g_millis;
  }

  if((g_millis-g_loop_ct) > 1000) {
    
    Serial.print("loopcps ");
    Serial.println(g_loop_cps);
    Serial.print("battV ");
    Serial.println(g_battV);
  }
  
}

void setup()
{
  delay(1000);
  analogReadResolution(10); 
  smar_setup();
  batt_setup();
//  md_setup();
  eval_setup();
  comm_setup();
//  servo_setup();
  light_setup();
  temp_setup();
  cumc_comm_setup();
  
  Serial.begin(115200);
 
  delay(2000);
  g_loop_ct=millis();
}

void loop()
{
  g_millis=millis();
  loop_counter();
  
  batt_read(&g_battV,&g_battA);
  temp_read();
  bxcu_comm();
  eval_doit();
  cumc_comm();

}
