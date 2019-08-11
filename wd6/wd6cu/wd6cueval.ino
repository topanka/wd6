void eval_setup(void)
{
  tmr_init(&g_tmr_lightpos,100);
}

int eval_doit(void)
{
  static uint8_t sw=0;
  static int l_lastlightpos=UCCB_PL_OFF;
  
//  if(g_recv_ready != 1) return(0);
  
  if((g_cb_fsBE == 11) && (g_recv_ready == 1)) {
    sw++;
    sw%=2;
    if(sw == 1) {
      light_pos_on();
    } else {
      light_pos_off();
    }
//    md_go();
  }      
  
  if((g_cb_b6pBE == 21) && (g_recv_ready == 1)) {
    g_piro_scan=PIRO_SCAN_START;
  }      
  if((g_cb_b6pBE == 61) && (g_recv_ready == 1)) {
    g_piro_scan=PIRO_SCAN_STOP;
  }      
  
  //md_setspeed();
  //servo_rudder();
  servo_holder();
  piro_doit();
  vl53l1x_read();
  pmw3901_read();
  
/*  
  if(g_cb_b6pBE == 11) {
    light_pos_on();
  }      
  if(g_cb_b6pBE == 41) {
    light_pos_off();
  }      
*/  

  if(g_cb_lightpos != l_lastlightpos) {
    if(g_cb_lightpos == UCCB_PL_OFF) {
      light_pos_off();
    } else if(g_cb_lightpos == UCCB_PL_ON) {
      light_pos_on();
    }
    l_lastlightpos=g_cb_lightpos;
  }
  if(g_cb_lightpos == UCCB_PL_BLINK) {
    if(tmr_do(&g_tmr_lightpos) == 1) {
      if(g_tmr_lightpos.cnt%2 == 0) {
        light_pos_on();
      } else {
        light_pos_off();
      }
    }
  }
  
  return(1);
}
