void loop_counter(void)
{
  g_loop_cnt++;
  if((g_millis-g_loop_ct) > 1000) {
    g_loop_cps=g_loop_cnt;
    g_loop_cnt=0;
    g_loop_ct=g_millis;

//    Serial.print("loopcps ");
//    Serial.println(g_loop_cps);
//    Serial.print("battV ");
//    Serial.println(g_battV);

    
  }
}

void setup()
{
  pinMode(RESET_MD_PIN,INPUT_PULLUP);
  delay(500);
  analogReadResolution(10); 

  Serial.begin(115200);
  
  // Configure SPI Flash chip select
  pinMode(WD6CU_SPI_CS_PIN,OUTPUT);
  digitalWrite(WD6CU_SPI_CS_PIN,HIGH);
  SPI.begin(); // initiate SPI 

  smar_setup();
  batt_setup();
  eval_setup();
  comm_setup();
//  servo_setup();
  light_setup();
  temp_setup();
//  piro_setup();
  vl53l1x_setup();
//  pmw3901_setup();
  cumc_comm_setup();
  
  pinMode(RESET_MD_PIN,OUTPUT);
  digitalWrite(RESET_MD_PIN,LOW);
  delay(10);
  pinMode(RESET_MD_PIN,INPUT_PULLUP);

  delay(1000);
  g_loop_ct=millis();
}

void loop()
{
  g_millis=millis();
  loop_counter();

  vl53lx1_restart();

  batt_read(&g_battV,&g_battA);
  temp_read();
  bxcu_comm();
  eval_doit();
  cumc_comm();

}
