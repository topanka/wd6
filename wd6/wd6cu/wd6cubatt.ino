int batt_setup(void)
{
  int i,avn;

  avn=7;
  smar_init(UCCB_BATTV_PORT,avn,5);
  
//fill the buffer  
  for(i=0;i < 2*avn;i++) {
    smar_analogRead(UCCB_BATTV_PORT);
  }
  
  avn=11;
  smar_init(UCCB_BATTA_PORT,avn,8);
  
//fill the buffer  
  for(i=0;i < 2*avn;i++) {
    smar_analogRead(UCCB_BATTA_PORT);
  }
  
  return(0);
}

int batt_read(int16_t *battV, int16_t *battA)
{
  static unsigned long l_brtv=0,l_brta=0;
  static int l_bv=0;
  static int l_ba=0;
  
  if((g_millis-l_brtv) >= 500) {
    l_bv=smar_analogRead(UCCB_BATTV_PORT);
    l_brtv=g_millis;
  }
  if((g_millis-l_brta) >= 25) {
    l_ba=smar_analogRead(UCCB_BATTA_PORT);
    l_brta=g_millis;
  }
  
  *battV=l_bv;
  *battA=l_ba;
  
  return(0);
}
