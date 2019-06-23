unsigned long g_piro_doit_tmo=60;
unsigned long g_piro_counter=0;
int g_piro_max_val=-1;
int g_piro_max_pos=0;

PIRO g_psl;
PIRO g_psr;

int piro_setup(void)
{
  tmr_init(&g_tmr_piro,g_piro_doit_tmo);
  
  memset((void*)&g_psl,0,sizeof(PIRO));
  g_psl.port=WD6CU_PIRO_LEFT;
  memset((void*)&g_psr,0,sizeof(PIRO));
  g_psr.port=WD6CU_PIRO_RIGHT;
  return(0);
}

int piro_doit(void)
{
  int sum,diff,ret;

  piro_read(&g_psl);
  piro_read(&g_psr);
  
  if(tmr_do(&g_tmr_piro) != 1) return(0);

  sum=g_psl.s_val0+g_psr.s_val0;
  if(sum > 0) {
    diff=(32*(g_psl.s_val0-g_psr.s_val0))/sum;
  } else {
    diff=0;
  }

  if(g_piro_scan == PIRO_SCAN_START) {
    servo_scan_start();
    g_piro_scan=PIRO_SCAN_ONPROGRESS;
    g_piro_max_val=-1;
  } else if(g_piro_scan == PIRO_SCAN_ONPROGRESS) {
    ret=servo_scan_onprogress(sum,&g_piro_max_val,&g_piro_max_pos);
    if(ret == 0) {
      servo_scan_stop(g_piro_max_pos);
      g_piro_scan=PIRO_SCAN_STOP;
      g_piro_scan=PIRO_SCAN_FOLLOW;
//      g_piro_scan=PIRO_SCAN_COURSE;
    }
  } else if(g_piro_scan == PIRO_SCAN_COURSE) {
    ret=servo_course(diff);
    if(ret == 0) {
//      g_piro_scan=PIRO_SCAN_STOP;
      g_piro_scan=PIRO_MOTOR_FOLLOW;
    }
  } else if(g_piro_scan == PIRO_SCAN_FOLLOW) {
    servo_follow(diff);
  } else if(g_piro_scan == PIRO_MOTOR_FOLLOW) {
    cumc_follow(diff);
  }

/*
  Serial.print(g_piro_counter);

  Serial.print(" ");
//  Serial.print(g_psl.s_val0);
  Serial.print(lsv);
  Serial.print(" ");
//  Serial.print(g_psr.s_val0);
  Serial.print(rsv);
  Serial.print(" ");
  Serial.print(sum);
  Serial.print(" ");
  Serial.print(diff);
*/  
  

/*  
  Serial.print(" ");
  Serial.print(g_psl.s_val0);
  Serial.print(" ");
  Serial.print((3.3*g_psl.s_val0)/1024.0);
  
  Serial.print(" ");
  Serial.print(g_psr.s_val0);
  Serial.print(" ");
  Serial.print((3.3*g_psr.s_val0)/1024.0);
*/  

/*  
  vall=analogRead(WD6CU_PIRO_LEFT);
  valr=analogRead(WD6CU_PIRO_RIGHT);

  Serial.print(g_piro_counter);
  
  Serial.print(" ");
  Serial.print(vall);
  Serial.print(" ");
  Serial.print((3.3*vall)/1024.0);
  
  Serial.print(" ");
  Serial.print(valr);
  Serial.print(" ");
  Serial.print((3.3*valr)/1024.0);
*/  
  
//  Serial.println();

  g_piro_counter+=g_piro_doit_tmo;

  return(0);
}
