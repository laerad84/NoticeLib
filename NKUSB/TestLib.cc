#include "NKUSB.h"

int main( int  argc,char** argv  ){
  NKUSB* nk  = new NKUSB("MiniFADC",0x0547,0x1502,5);
  //nk->USBInit();
  nk->USBOpen();
  
  nk->USBClose();
  //nk->USBExit();
  return 0;
}
