#define WD6_HOLDERPAN_CENTER              1500
#define WD6_HOLDERPAN_MIN                 580
#define WD6_HOLDERPAN_MAX                 2420

#define WD6_HOLDERTILT_CENTER             1500
#define WD6_HOLDERTILT_MIN                1130
#define WD6_HOLDERTILT_MAX                1870
#define WD6_HOLDERTILT_LEVEL              1370

Servo holderpan; 
Servo holdertilt; 

int g_wd6_holderpan_pos=WD6_HOLDERPAN_CENTER;
int g_wd6_holdertilt_pos=WD6_HOLDERTILT_LEVEL;
unsigned long g_wd6_holder_t=0;

int servo_setup(void)
{
  holderpan.attach(5,WD6_HOLDERPAN_MIN,WD6_HOLDERPAN_MAX);
  holdertilt.attach(6,WD6_HOLDERTILT_MIN,WD6_HOLDERTILT_MAX);
  
  holderpan.writeMicroseconds(g_wd6_holderpan_pos);
  holdertilt.writeMicroseconds(g_wd6_holdertilt_pos);
  
  return(0);
}

int servo_holder(void)
{
  int vx,vy;
  
  vx=constrain(g_cb_tsxp,-100,100);
  vy=constrain(g_cb_tsyp,-100,100);
  if((vx == 0) && (vy == 0)) return(0);

  if((g_millis-g_wd6_holder_t) < 40) return(0);
  
  vx=vx/3;
  g_wd6_holderpan_pos+=vx;
  if(g_wd6_holderpan_pos < WD6_HOLDERPAN_MIN) g_wd6_holderpan_pos=WD6_HOLDERPAN_MIN;
  else if(g_wd6_holderpan_pos > WD6_HOLDERPAN_MAX) g_wd6_holderpan_pos=WD6_HOLDERPAN_MAX;
  holderpan.writeMicroseconds(g_wd6_holderpan_pos);
  
  vy=-vy/3;
  g_wd6_holdertilt_pos+=vy;
  if(g_wd6_holdertilt_pos < WD6_HOLDERTILT_MIN) g_wd6_holdertilt_pos=WD6_HOLDERTILT_MIN;
  else if(g_wd6_holdertilt_pos > WD6_HOLDERTILT_MAX) g_wd6_holdertilt_pos=WD6_HOLDERTILT_MAX;
  holdertilt.writeMicroseconds(g_wd6_holdertilt_pos);
  
  g_wd6_holder_t=g_millis;
  
  return(0);
}

int servo_scan_start(void)
{
  g_piro_scan_start_t0=g_millis;
  g_wd6_holderpan_pos=WD6_HOLDERPAN_MIN;
  holderpan.writeMicroseconds(g_wd6_holderpan_pos);
  g_wd6_holdertilt_pos=WD6_HOLDERTILT_LEVEL;
  holdertilt.writeMicroseconds(g_wd6_holdertilt_pos);

  return(0);
}

int servo_scan_onprogress(int val, int *max_val, int *max_pos)
{
  int rval=-1;

  if(g_millis < g_piro_scan_start_t0+500) return(1);

/*  
  if(g_piro_scan_start_t0 > 0) {
    Serial.println("start measurement");
    g_piro_scan_start_t0=0;
  }
*/  
  
  if(g_wd6_holderpan_pos <= WD6_HOLDERPAN_MIN) {
    g_wd6_holderpan_pos=WD6_HOLDERPAN_MIN;
    *max_val=val;
    *max_pos=g_wd6_holderpan_pos;
  } else if(val > *max_val) {
    *max_val=val;
    *max_pos=g_wd6_holderpan_pos;
  }
  if(g_wd6_holderpan_pos >= WD6_HOLDERPAN_MAX) {
    g_wd6_holderpan_pos=WD6_HOLDERPAN_MAX;
    return(0);
  }
  if(g_wd6_holderpan_pos < WD6_HOLDERPAN_MAX) {
    g_wd6_holderpan_pos+=50;
    rval=1;
  }
  if(g_wd6_holderpan_pos >= WD6_HOLDERPAN_MAX) {
    g_wd6_holderpan_pos=WD6_HOLDERPAN_MAX;
    rval=1;
  }
  holderpan.writeMicroseconds(g_wd6_holderpan_pos);

  return(rval);
}

int servo_scan_stop(int pos)
{
  g_piro_scan_stop_t0=g_millis;
  g_wd6_holderpan_pos=pos;
  if(g_wd6_holderpan_pos >= WD6_HOLDERPAN_MAX) {
    g_wd6_holderpan_pos=WD6_HOLDERPAN_MAX;
  }
  holderpan.writeMicroseconds(g_wd6_holderpan_pos);

  return(0);
}

int servo_course(int diff)
{
  int d;
  
  if(g_millis < g_piro_scan_stop_t0+500) return(1);
  
  if(diff > 0) {
    if(diff > 6) diff=6;
    d=map(diff,1,6,100,200);
    g_cb_m1s=-d;
    g_cb_m2s=d;
  } else if(diff < 0) {
    if(diff < -6) diff=-6;
    d=map(-diff,1,6,100,200);
    g_cb_m1s=d;
    g_cb_m2s=-d;
  } else {
    g_cb_m1s=0;
    g_cb_m2s=0;
  }
  if(g_wd6_holderpan_pos < WD6_HOLDERPAN_CENTER) {
    g_wd6_holderpan_pos+=20;
    if(g_wd6_holderpan_pos > WD6_HOLDERPAN_CENTER) {
      g_wd6_holderpan_pos=WD6_HOLDERPAN_CENTER;
    }  
  } else if(g_wd6_holderpan_pos > WD6_HOLDERPAN_CENTER) {
    g_wd6_holderpan_pos-=20;
    if(g_wd6_holderpan_pos < WD6_HOLDERPAN_CENTER) {
      g_wd6_holderpan_pos=WD6_HOLDERPAN_CENTER;
    }
  }
  holderpan.writeMicroseconds(g_wd6_holderpan_pos);

  if(g_wd6_holderpan_pos == WD6_HOLDERPAN_CENTER) {
    return(0);
  }

  return(1);
}


int servo_follow(int diff)
{
  int d;

  if(g_millis < g_piro_scan_stop_t0+500) return(0);

 /*
  if(diff != 0) {
    Serial.println(diff);
  }
*/  
  
  if(diff > 0) {
    if(diff > 7) diff=7;
    d=map(diff,1,7,5,150);
    g_wd6_holderpan_pos+=d;
  } else if(diff < 0) {
    if(diff < -7) diff=-7;
    d=map(-diff,1,7,5,150);
    g_wd6_holderpan_pos-=d;
  }
  if(g_wd6_holderpan_pos < WD6_HOLDERPAN_MIN) {
    g_wd6_holderpan_pos=WD6_HOLDERPAN_MIN;
  } else if(g_wd6_holderpan_pos > WD6_HOLDERPAN_MAX) {
    g_wd6_holderpan_pos=WD6_HOLDERPAN_MAX;
  }
  holderpan.writeMicroseconds(g_wd6_holderpan_pos);

  return(0);
}
