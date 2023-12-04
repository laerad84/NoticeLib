#include "NKUSB.h"

int main( int  argc,  char** argv ){
  NKUSBMulti* nk  =   new  NKUSBMulti("MiniFADC",0x0547,0x1502);
  int nusb  = nk->USBOpen();//open all device
  std::cout<<  nusb <<   std::endl;
  nk->PrintList();
  nusb = nk->USBClose();
  std::cout<<  nusb <<   std::endl;
  
  return  0;
}
