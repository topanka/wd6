#define SMAR_TOT_IDX           10
#define SMAR_TOT_NUM           31

typedef struct tagSMAR {
  int tbl[SMAR_TOT_NUM];
  int idx;
  int sum;
  int portmap;
  int lvv;
  unsigned int lvc;
  unsigned int avn;
  unsigned int eqn;
  unsigned long lvt;
} SMAR;

SMAR smar[SMAR_TOT_IDX]={0};

int smar_setportidx(int port)
{
  int rval=-1,i;
  
  for(i=0;i < SMAR_TOT_IDX;i++) {
    if(smar[i].portmap == -1) {
      smar[i].portmap=port;
      rval=i;
      break;
    }
  }

  return(rval);
}

int smar_getportidx(int port)
{
  int rval=-1,i;
  
  for(i=0;i < SMAR_TOT_IDX;i++) {
    if(smar[i].portmap == port) {
      rval=i;
      break;
    }
  }

  return(rval);
}

int smar_setup(void)
{
  int i;
  
  for(i=0;i < SMAR_TOT_IDX;i++) {
    smar[i].portmap=-1;
    smar[i].lvv=-1;
    smar[i].avn=1;
    smar[i].eqn=1;
    smar[i].lvt=0;
  }
  
//  Serial.begin(9600);
  return(0);
}

int smar_init(int port, unsigned int avn, unsigned int eqn)
{
  int rval=-1,i,iport;
  
  iport=smar_setportidx(port);
  if((iport < 0) || (iport >= SMAR_TOT_IDX)) goto end;
  if(avn > SMAR_TOT_NUM) goto end;
  if(eqn > 20) goto end;
  for(i=0;i < SMAR_TOT_NUM;i++) {
    smar[iport].tbl[i]=0;
  }
  smar[iport].avn=avn;
  smar[iport].eqn=eqn;
  
  rval=0;
  
end:

  return(rval);
}

int smar_analogRead(int port)
{
  int rval=-1,iport,v,lvch=0;
  
  iport=smar_getportidx(port);
  if((iport < 0) || (iport >= SMAR_TOT_IDX)) goto end;
  smar[iport].sum-=smar[iport].tbl[smar[iport].idx];
  smar[iport].tbl[smar[iport].idx]=analogRead(port);
  
//  Serial.println(smar_tbl[iport][smar_idx[iport]]);
  
  smar[iport].sum+=smar[iport].tbl[smar[iport].idx];
  smar[iport].idx=(smar[iport].idx+1)%smar[iport].avn;

  v=smar[iport].sum/smar[iport].avn;

/*  
if(iport == 0) {  
  Serial.print(smar[iport].tbl[smar[iport].idx]);
  Serial.print(" ");
  Serial.print(smar[iport].lvv);
  Serial.print(" ");
  Serial.print(smar[iport].lvc);
  Serial.print(" ");
  Serial.println(v);
}
*/

  if(smar[iport].lvv < 0) {
    smar[iport].lvv=v;
    smar[iport].lvc=0;
    rval=v;
  } else {
    if(smar[iport].lvv == v) {
      smar[iport].lvc=0;
      smar[iport].lvt=g_millis;
      rval=v;
    } else {
      if(smar[iport].lvc++ > smar[iport].eqn) {
        if(abs(smar[iport].lvv-v) <= 3) {
          if((g_millis-smar[iport].lvt) > 300) lvch=1;
        } else {
          lvch=1;
        }
        if(lvch == 1) {
          smar[iport].lvv=v;
          smar[iport].lvc=0;
          rval=v;
        } else {
          rval=smar[iport].lvv;
        }
      } else {
        rval=smar[iport].lvv;
      }
    }
  }

end:

  return(rval);
}
