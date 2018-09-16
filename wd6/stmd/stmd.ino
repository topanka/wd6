//wd6 arduino

#include "vnh5019.h"
#include "wd6re.h"
#include "wd6mtr.h"
#include "sh1tmr.h"

VNH5019MD g_md_J3(28,29,8,26,27,A3,&OCR4C,5,wd6re_isrJ3);
VNH5019MD g_md_B3(40,41,46,38,39,A6,&OCR5A,2,wd6re_isrB3);

WD6RE g_wd6re_J1={0};
WD6RE g_wd6re_J2={0};
WD6RE g_wd6re_J3={0};
WD6RE g_wd6re_B1={0};
WD6RE g_wd6re_B2={0};
WD6RE g_wd6re_B3={0};

WD6MTR g_mtrJ3={0};
WD6MTR g_mtrB3={0};

unsigned long g_millis=0;
unsigned char g_rb=0;
unsigned long g_chmst=0;    //change motor speed time
int g_msn=0,g_mso=0;
