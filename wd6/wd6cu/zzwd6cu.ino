void loop_counter(void)
{
  g_loop_cnt++;
  if((g_millis-g_loop_ct) > 100) {
    g_loop_cps=g_loop_cnt;
    g_loop_cnt=0;
    g_loop_ct=g_millis;
  }
}

void setup()
{
  delay(1000);
  smar_setup();
  batt_setup();
//  md_setup();
  eval_setup();
  comm_setup();
//  servo_setup();
  light_setup();
  temp_setup();
  
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
  comm_send();
  comm_recv();
  eval_doit();

}
