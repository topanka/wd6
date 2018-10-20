#include "wd6cumc.h"

byte g_wmd_commbuf[100]={0};
unsigned long g_wmd_sendtime=0;
unsigned long g_wmd_commpkt_counter=0;        //total counter of sent packets

byte g_rmd_commbuf[100]={0};
int g_rmd_state=WD6CUMC_PST_INIT;
unsigned int g_rmd_len=0;
 
int comm_setup(void)
{
  Serial2.begin(500000);
  buildCRCTable();
//  tmr_init(&g_tmr_comm,250);
  tmr_init(&g_tmr_comm,200);
}

int wd6md_comm_pack1(byte *d, uint16_t l, byte *buf, uint16_t *len)
{
  if((*len+l) > sizeof(g_wmd_commbuf)) return(-1);
  memcpy((void*)(buf+*len),(void*)d,l);
  (*len)+=l;
  return(0);
}

int tscr_comm_packtscr(uint16_t *len)
{
  byte lead=WD6CUMC_MC_LEAD;
  byte crc8;
  unsigned int m1c=0,m2c=0;
  int16_t rpm1,rpm2;
  
  *len=0;
  
/*
  Serial.print(rpm1);
  Serial.print(" ");
  Serial.println(rpm2);
*/  

  wd6md_comm_pack1((byte*)&lead,sizeof(lead),g_wmd_commbuf,len);      //1:1
  wd6md_comm_pack1((byte*)&g_loop_cps,sizeof(g_loop_cps),g_wmd_commbuf,len);    //4:5
  wd6md_comm_pack1((byte*)&g_wd6md_J1.re->rpm,sizeof(uint16_t),g_wmd_commbuf,len);    //2:7
  wd6md_comm_pack1((byte*)&g_wd6md_J2.re->rpm,sizeof(uint16_t),g_wmd_commbuf,len);    //2:9
  wd6md_comm_pack1((byte*)&g_wd6md_J3.re->rpm,sizeof(uint16_t),g_wmd_commbuf,len);    //2:11
  wd6md_comm_pack1((byte*)&g_wd6md_B1.re->rpm,sizeof(uint16_t),g_wmd_commbuf,len);    //2:13
  wd6md_comm_pack1((byte*)&g_wd6md_B2.re->rpm,sizeof(uint16_t),g_wmd_commbuf,len);    //2:15
  wd6md_comm_pack1((byte*)&g_wd6md_B3.re->rpm,sizeof(uint16_t),g_wmd_commbuf,len);    //2:17
  crc8=getCRC(g_wmd_commbuf,*len);
  wd6md_comm_pack1((byte*)&crc8,sizeof(crc8),g_wmd_commbuf,len);    //1:18
  
//18 byte long  
  
  return(0);
}

int comm_send(void)
{
  unsigned int len;

  if(g_force_send == 0) {
    if(tmr_do(&g_tmr_comm) != 1) return(0);
  }
  g_force_send=0;
  
  tscr_comm_packtscr(&len);
  
  Serial2.write((byte*)&g_wmd_commbuf[0],len);
  
  g_wmd_commpkt_counter=0;

  return(1);
}

int comm_read(int *state, unsigned char *buf, unsigned int *len)
{
  int rval=-1,ret,nr=0;
  unsigned char c1;
  unsigned char crc8;
//  static unsigned long xx=0;

  while(Serial2.available()) {
    c1=(unsigned char)Serial2.read();
    
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
        if(c1 == WD6CUMC_CU_LEAD) {
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
        if(*len == WD6CUMC_CU_PKTLAST) {
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
        if(*len != WD6CUMC_CU_PKTLAST) {
          *state=WD6CUMC_PST_INIT;
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
          *state=WD6CUMC_PST_INIT;
          break;
        }
//  Serial.println("crc3");
        *state=WD6CUMC_PST_READY;
        return(WD6CUMC_PST_READY);
      default:
        *state=WD6CUMC_PST_INIT;
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

int comm_unpackcu(unsigned char *buf, unsigned long len, 
                  unsigned long *commpkt_counter,
                  int *battV,
                  int *tsX,
                  int *tsY,
                  int *fsX,
                  int *fsY,
                  int *fsZ,
                  int *fsBS,
                  int *fsBE,
                  int *stb,
                  int *b6pBS,
                  int *b6pBE)
{
  unsigned int l;
  
  l=1;

  comm_unpack1((unsigned char *)commpkt_counter,sizeof(unsigned long),buf,&l);
  comm_unpack1((unsigned char *)battV,sizeof(uint16_t),buf,&l);
  comm_unpack1((unsigned char *)tsX,sizeof(uint16_t),buf,&l);
  comm_unpack1((unsigned char *)tsY,sizeof(uint16_t),buf,&l);
  comm_unpack1((unsigned char *)fsX,sizeof(uint16_t),buf,&l);
  comm_unpack1((unsigned char *)fsY,sizeof(uint16_t),buf,&l);
  comm_unpack1((unsigned char *)fsZ,sizeof(uint16_t),buf,&l);
  comm_unpack1((unsigned char *)fsBS,sizeof(uint16_t),buf,&l);
  comm_unpack1((unsigned char *)fsBE,sizeof(uint16_t),buf,&l);
  comm_unpack1((unsigned char *)stb,sizeof(uint16_t),buf,&l);
  comm_unpack1((unsigned char *)b6pBS,sizeof(uint16_t),buf,&l);
  comm_unpack1((unsigned char *)b6pBE,sizeof(uint16_t),buf,&l);

  return(0);
}

int comm_recv(void)
{
  int ret,stb,pwr0,poslight;

  ret=comm_read(&g_rmd_state,g_rmd_commbuf,&g_rmd_len);
  
  if(ret == WD6CUMC_PST_READY) {
    comm_unpackcu(g_rmd_commbuf,g_rmd_len,
                  &g_wcu_commpkt_counter,
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
                  &g_cb_b6pBE);
  
    return(1);
  }

  return(0);
}

int wd6cumd_comm(void)
{
  comm_send();
  comm_recv();
}
