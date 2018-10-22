#include "wd6cumc.h"

byte g_wmc_commbuf[100]={0};
unsigned long g_wmc_sendtime=0;
unsigned long g_wmc_commpkt_counter=0;        //total counter of sent packets

byte g_rmc_commbuf[100]={0};
int g_rmc_state=WD6CUMC_PST_INIT;
unsigned int g_rmc_len=0;

int cumc_comm_setup(void)
{
  Serial1.begin(500000);    //wd6 motor controller
  return(0);
}

int cumc_comm_pack1(byte *d, uint16_t l, byte *buf, uint16_t *len)
{
  if((*len+l) > sizeof(g_wmc_commbuf)) return(-1);
  memcpy((void*)(buf+*len),(void*)d,l);
  (*len)+=l;
  return(0);
}

int cumc_comm_packuccb(int16_t fsBE, int16_t b6pBE, uint16_t *len)
{
  byte lead=WD6CUMC_CU_LEAD;
  byte crc8;
  int16_t stb=0;
  
  *len=0;

//  stb=g_sw10p;
//  if(g_sh1_m1on == 1) stb|=UCCB_ST_M1;
//  if(g_sh1_m2on == 1) stb|=UCCB_ST_M2;
//  if(g_sh1_poslight == UCCB_PL_ON) stb|=(UCCB_PL_ON<<UCCB_PL_STPOS);
//  if(g_sh1_poslight == UCCB_PL_BLINK) stb|=(UCCB_PL_BLINK<<UCCB_PL_STPOS);

  cumc_comm_pack1((byte*)&lead,sizeof(lead),g_wmc_commbuf,len);      //1:1
  cumc_comm_pack1((byte*)&g_wmc_commpkt_counter,sizeof(g_wmc_commpkt_counter),g_wmc_commbuf,len);    //4:5-32
  cumc_comm_pack1((byte*)&g_battV,sizeof(g_battV),g_wmc_commbuf,len);    //2:7-12
  cumc_comm_pack1((byte*)&g_cb_tsX,sizeof(g_cb_tsX),g_wmc_commbuf,len);    //2:9-12  
  cumc_comm_pack1((byte*)&g_cb_tsY,sizeof(g_cb_tsY),g_wmc_commbuf,len);    //2:11-12
  cumc_comm_pack1((byte*)&g_cb_fsX,sizeof(g_cb_fsX),g_wmc_commbuf,len);    //2:13-12
  cumc_comm_pack1((byte*)&g_cb_fsY,sizeof(g_cb_fsY),g_wmc_commbuf,len);    //2:15-12
  cumc_comm_pack1((byte*)&g_cb_fsZ,sizeof(g_cb_fsZ),g_wmc_commbuf,len);    //2:17-12
  cumc_comm_pack1((byte*)&g_cb_fsBS,sizeof(g_cb_fsBS),g_wmc_commbuf,len);    //2:19-4
  cumc_comm_pack1((byte*)&fsBE,sizeof(fsBE),g_wmc_commbuf,len);    //2:21-8
  cumc_comm_pack1((byte*)&stb,sizeof(stb),g_wmc_commbuf,len);    //2:23-4
  cumc_comm_pack1((byte*)&g_cb_b6pBS,sizeof(g_cb_b6pBS),g_wmc_commbuf,len);    //2:25-4
  cumc_comm_pack1((byte*)&b6pBE,sizeof(b6pBE),g_wmc_commbuf,len);    //2:27-8
  
  crc8=getCRC(g_wmc_commbuf,*len);
  
//Serial.print("crc8 ");  
//Serial.println(crc8);  
  
  cumc_comm_pack1((byte*)&crc8,sizeof(crc8),g_wmc_commbuf,len);    //1:28
  
//28 byte long  
  
  return(0);
}

int cumc_comm_send(void)
{
  uint16_t len;
  static int16_t l_b6pBE=0;
  static int16_t l_fsBE=0;

  g_wmc_commpkt_counter++;
  
  if(g_cb_b6pBE != 0) l_b6pBE=g_cb_b6pBE;
  if(g_cb_fsBE != 0) l_fsBE=g_cb_fsBE;
  
  if((g_millis < g_wmc_sendtime+50) &&
     (l_b6pBE == 0) &&
     (l_fsBE == 0)) return(0);
/*
Serial.print("write to motor");
Serial.println(g_cb_fsY);
*/
  g_wmc_sendtime=g_millis;
  cumc_comm_packuccb(l_fsBE,l_b6pBE,&len);
  Serial1.write((byte*)&g_wmc_commbuf[0],len);
  
  l_b6pBE=0;
  l_fsBE=0;

  g_wmc_commpkt_counter=0;

  return(1);
}

int cumc_comm_read(int *state, unsigned char *buf, unsigned int *len)
{
  int rval=-1,nr=0;
  unsigned char c1;
  unsigned char crc8;

  while(Serial1.available()) {
    c1=(unsigned char)Serial1.read();
    if(++nr >= 100) break;
    switch(*state) {
      case WD6CUMC_PST_INIT:
      case WD6CUMC_PST_READY:
/*      
  Serial.print(*len);
  Serial.print(" ");
  Serial.print(c1);
  Serial.println(" init/ready");
*/  
        if(c1 == WD6CUMC_MC_LEAD) {
          *len=0;
          buf[*len]=c1;
          *state=WD6CUMC_PST_DATA;
        } else {
          *state=WD6CUMC_PST_INIT;
        }
        break;
      case WD6CUMC_PST_DATA:
/*      
  Serial.print(*len);
  Serial.print(" ");
  Serial.print(c1);
  Serial.println(" data");
*/  
        (*len)++;
        buf[*len]=c1;
        if(*len == WD6CUMC_MC_PKTLAST) {
          *state=WD6CUMC_PST_CRC;
        }
        break;
      case WD6CUMC_PST_CRC:
/*      
  Serial.print(*len);
  Serial.print(" ");
  Serial.print(c1);
  Serial.println(" crc1");
*/  
        if(*len != WD6CUMC_MC_PKTLAST) {
          *state=WD6CUMC_PST_INIT;
          break;
        }
        (*len)++;
        crc8=getCRC(buf,*len);
/*        
  Serial.print(c1);
  Serial.print(" ");
  Serial.println(crc8);
*/  
        if(crc8 != c1) {
          *state=WD6CUMC_PST_INIT;
          break;
        }
        *state=WD6CUMC_PST_READY;
        return(WD6CUMC_PST_READY);
      default:
        *state=WD6CUMC_PST_INIT;
        break;  
    }
  }
  
  return(rval);
}

int cumc_comm_unpack1(unsigned char *d, unsigned int l, unsigned char *buf, unsigned int *len)
{
  memcpy((void*)d,(void*)(buf+*len),l);
  (*len)+=l;
  return(0);
}

int cumc_comm_unpackuccb(unsigned char *buf, unsigned int len, 
                    unsigned long *loop_cps,
                    int16_t *J1rpm, int16_t *B1rpm,
                    int16_t *J2rpm, int16_t *B2rpm,
                    int16_t *J3rpm, int16_t *B3rpm
                    )
{
  unsigned int l;
  
  l=1;
  cumc_comm_unpack1((unsigned char *)loop_cps,sizeof(unsigned long),buf,&l);
  cumc_comm_unpack1((unsigned char *)J1rpm,sizeof(int16_t),buf,&l);
  cumc_comm_unpack1((unsigned char *)J2rpm,sizeof(int16_t),buf,&l);
  cumc_comm_unpack1((unsigned char *)J3rpm,sizeof(int16_t),buf,&l);
  cumc_comm_unpack1((unsigned char *)B1rpm,sizeof(int16_t),buf,&l);
  cumc_comm_unpack1((unsigned char *)B2rpm,sizeof(int16_t),buf,&l);
  cumc_comm_unpack1((unsigned char *)B3rpm,sizeof(int16_t),buf,&l);

  return(0);
}

int cumc_comm_recv(void)
{
  int ret;

  ret=cumc_comm_read(&g_rmc_state,g_rmc_commbuf,&g_rmc_len);
  
//                  Serial.println(ret);
  
  if(ret == WD6CUMC_PST_READY) {
    cumc_comm_unpackuccb(g_rmc_commbuf,g_rmc_len,
                    &g_mc_loopcps,
                    &g_mc_J1rpm,
                    &g_mc_J2rpm,
                    &g_mc_J3rpm,
                    &g_mc_B1rpm,
                    &g_mc_B2rpm,
                    &g_mc_B3rpm
                    );

Serial.print(g_mc_J3rpm);
Serial.print(" ");
Serial.println(g_mc_B3rpm);
                    

/*
                    Serial.print(g_tscr_power);
                    Serial.print(" ");
                    Serial.println(g_tscr_rudder);
*/  

  }
  
  return(0);
}

int cumc_comm(void)
{
  cumc_comm_send();
  cumc_comm_recv();

  return(0);
}
