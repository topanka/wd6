byte g_w_commbuf[100]={0};
unsigned long g_w_sendtime=0;
unsigned long g_w_commpkt_counter=0;        //total counter of sent packets

byte g_r_commbuf[100]={0};
int g_r_state=UCCB_PST_INIT;
unsigned int g_r_len=0;
byte g_commmode=1;
unsigned long g_commmode_t=0;

int comm_setup(void)
{
  Serial3.begin(19200);
  buildCRCTable();

  return(0);
}

int comm_pack1(byte *d, uint16_t l, byte *buf, uint16_t *len)
{
  if((*len+l) > sizeof(g_w_commbuf)) return(-1);
  memcpy((void*)(buf+*len),(void*)d,l);
  (*len)+=l;
  return(0);
}

int comm_packsh1(uint16_t *len)
{
  byte lead=UCCB_WD6CU_LEAD;
  byte crc8;
  int16_t rpm1,rpm2;
  
  *len=0;
  
//  md_getmc(&m1c,&m2c);
  if(g_dir_m1 >= 0) rpm1=(int16_t)g_rpm_m1;
  else rpm1=-(int16_t)g_rpm_m1;
  if(g_dir_m2 >= 0) rpm2=(int16_t)g_rpm_m2;
  else rpm2=-(int16_t)g_rpm_m2;

/*
  Serial.print(g_loop_cnt);
  Serial.print(" ");
  Serial.println(g_loop_cps);
*/  
  
  comm_pack1((byte*)&lead,sizeof(lead),g_w_commbuf,len);      //1:1
  comm_pack1((byte*)&g_loop_cps,sizeof(g_loop_cps),g_w_commbuf,len);    //4:5
  comm_pack1((byte*)&g_battV,sizeof(g_battV),g_w_commbuf,len);    //2:7
  comm_pack1((byte*)&g_battA,sizeof(g_battA),g_w_commbuf,len);    //2:9
  comm_pack1((byte*)&g_cur_m1,sizeof(g_cur_m1),g_w_commbuf,len);    //2:11
  comm_pack1((byte*)&g_cur_m2,sizeof(g_cur_m2),g_w_commbuf,len);    //2:13
  comm_pack1((byte*)&rpm1,sizeof(rpm1),g_w_commbuf,len);    //2:15
  comm_pack1((byte*)&rpm2,sizeof(rpm2),g_w_commbuf,len);    //2:17
  comm_pack1((byte*)&g_temperature,sizeof(g_temperature),g_w_commbuf,len);    //2:19
  comm_pack1((byte*)&g_wd6_piro_val,sizeof(g_wd6_piro_val),g_w_commbuf,len);    //2:21
  comm_pack1((byte*)&g_wd6_vl53l1x_dist,sizeof(g_wd6_vl53l1x_dist),g_w_commbuf,len);    //2:23
  crc8=getCRC(g_w_commbuf,*len);

/*
  Serial.print(" ");
  Serial.print("crc ");
  Serial.println(crc8);
*/
  
  comm_pack1((byte*)&crc8,sizeof(crc8),g_w_commbuf,len);    //1:24
  
//24 byte long  
  
  return(0);
}

int comm_send(void)
{
  uint16_t len;

//  g_w_commpkt_counter++;
  
//csakhogy ne kuggyon soha  
//g_commmode=1;

  if(g_commmode != 0) return(0);
  
/*  
  Serial.print("commmode ");
  Serial.println(g_commmode);
*/

  g_w_commpkt_counter++;
  
  comm_packsh1(&len);
  
//  delay(50);
  
  Serial3.write((byte*)&g_w_commbuf[0],len);
  
//  g_w_commpkt_counter=0;
  g_commmode=1;

  return(1);
}

int comm_read(int *state, unsigned char *buf, unsigned int *len)
{
  int rval=-1,nr=0;
  unsigned char c1;
  unsigned char crc8;
//  static unsigned long xx=0;

  while(Serial3.available()) {
    c1=(unsigned char)Serial3.read();
    
    if(++nr >= 100) break;
    switch(*state) {
      case UCCB_PST_INIT:
      case UCCB_PST_READY:
/*      
  Serial.print(*len);
  Serial.print(" ");
  Serial.print(c1);
  Serial.println(" init/ready");
*/  
        if(c1 == UCCB_WD6BX_LEAD) {
          *len=0;
          buf[*len]=c1;
          *state=UCCB_PST_DATA;
    	  } else {
          *state=UCCB_PST_INIT;
        }
	      break;
      case UCCB_PST_DATA:
/*      
  Serial.print(*len);
  Serial.print(" ");
  Serial.print(c1);
  Serial.println(" data");
*/  
        (*len)++;
        buf[*len]=c1;
        if(*len == UCCB_WD6BX_PKTLAST) {
          *state=UCCB_PST_CRC;
        }
        break;
      case UCCB_PST_CRC:
/*      
  Serial.print(*len);
  Serial.print(" ");
  Serial.print(c1);
  Serial.println(" crc1");
*/  
        if(*len != UCCB_WD6BX_PKTLAST) {
          *state=UCCB_PST_INIT;
          break;
        }
        (*len)++;
/*        
  Serial.print(*len);
  Serial.print(" ");
  Serial.print(c1);
  Serial.println("crc2");
*/  
        crc8=getCRC(buf,*len);
        
/*        
        xx++;
  Serial.print(xx);
  Serial.print(" ");
  Serial.print(c1);
  Serial.print(" ");
  Serial.println(crc8);
*/  
        if(crc8 != c1) {
          *state=UCCB_PST_INIT;
          break;
        }
//  Serial.println("crc3");
        *state=UCCB_PST_READY;
        return(UCCB_PST_READY);
      default:
        *state=UCCB_PST_INIT;
        break;  
    }
  }
  
  return(rval);
}

int comm_unpack1(unsigned char *d, unsigned int l, unsigned char *buf, unsigned int *len)
{
  memcpy((void*)d,(void*)(buf+*len),l);
  (*len)+=l;
  return(0);
}

int comm_unpackuccb(unsigned char *buf, unsigned long len, 
                    unsigned long *commpkt_counter,
                    int16_t *battV,
                    int16_t *tsX,
                    int16_t *tsY,
                    int16_t *fsX,
                    int16_t *fsY,
                    int16_t *fsZ,
                    int16_t *fsBS,
                    int16_t *fsBE,
                    int16_t *stb,
                    int16_t *b6pBS,
                    int16_t *b6pBE,
                    int16_t *m1s,
                    int16_t *m2s,
                    int16_t *rdd,
                    int16_t *tsx,
                    int16_t *tsy,
                    byte *commmode)
{
  unsigned int l;
  
  l=1;

  comm_unpack1((unsigned char *)commpkt_counter,sizeof(unsigned long),buf,&l);
  comm_unpack1((unsigned char *)battV,sizeof(int16_t),buf,&l);
  comm_unpack1((unsigned char *)tsX,sizeof(int16_t),buf,&l);
  comm_unpack1((unsigned char *)tsY,sizeof(int16_t),buf,&l);
  comm_unpack1((unsigned char *)fsX,sizeof(int16_t),buf,&l);
  comm_unpack1((unsigned char *)fsY,sizeof(int16_t),buf,&l);
  comm_unpack1((unsigned char *)fsZ,sizeof(int16_t),buf,&l);
  comm_unpack1((unsigned char *)fsBS,sizeof(int16_t),buf,&l);
  comm_unpack1((unsigned char *)fsBE,sizeof(int16_t),buf,&l);
  comm_unpack1((unsigned char *)stb,sizeof(int16_t),buf,&l);
  comm_unpack1((unsigned char *)b6pBS,sizeof(int16_t),buf,&l);
  comm_unpack1((unsigned char *)b6pBE,sizeof(int16_t),buf,&l);
  comm_unpack1((unsigned char *)m1s,sizeof(int16_t),buf,&l);
  comm_unpack1((unsigned char *)m2s,sizeof(int16_t),buf,&l);
  comm_unpack1((unsigned char *)rdd,sizeof(int16_t),buf,&l);
  comm_unpack1((unsigned char *)tsx,sizeof(int16_t),buf,&l);
  comm_unpack1((unsigned char *)tsy,sizeof(int16_t),buf,&l);
  comm_unpack1((unsigned char *)commmode,sizeof(byte),buf,&l);

  return(0);
}

int comm_recv(void)
{
  int ret;
  int16_t stb,m1s,m2s;

  g_recv_ready=0;
  if(g_commmode == 0) return(0);
  
  ret=comm_read(&g_r_state,g_r_commbuf,&g_r_len);
  
  if(ret == UCCB_PST_READY) {
    comm_unpackuccb(g_r_commbuf,g_r_len,
                    &g_cb_w_commpkt_counter,
                    &g_cb_battV,
                    &g_cb_tsX,
                    &g_cb_tsY,
                    &g_cb_fsX,
                    &g_cb_fsY,
                    &g_cb_fsZ,
                    &g_cb_fsBS,
                    &g_cb_fsBE,
                    &stb,
                    &g_cb_b6pBS,
                    &g_cb_b6pBE,
                    &m1s,
                    &m2s,
                    &g_cb_rdd,
                    &g_cb_tsxp,
                    &g_cb_tsyp,
                    &g_commmode);

    g_cb_sw10p=stb&UCCB_ST_SW10P;
    if((int)(stb&UCCB_ST_M1) == 0) {
      m1s=0;
      g_cb_m1s=0;
    }
    if((int)(stb&UCCB_ST_M2) == 0) {
      m2s=0;
      g_cb_m2s=0;
    }
    g_cb_lightpos=(int)((stb&UCCB_ST_POSLIGHT)>>UCCB_PL_STPOS);
    
    if(g_piro_scan == PIRO_MOTOR_FOLLOW) {
      if((m1s != 0) || (m2s != 0)) {
        g_piro_scan=PIRO_SCAN_STOP;
        g_cb_m1s=m1s;
        g_cb_m2s=m2s;
      }
    } else {
      g_cb_m1s=m1s;
      g_cb_m2s=m2s;
    }
/*
Serial.print("fsY=");
Serial.println(g_cb_fsY);
*/    
    
    g_recv_ready=1;
    return(1);
  }

  return(0);
}
int bxcu_comm(void)
{
  comm_send();
  comm_recv();

  return(0);
}
