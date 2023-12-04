#ifndef NKUSB__H__
#define NKUSB__H__

#include <cstdlib>
#include <cstdio>
#include <string>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <libusb-1.0/libusb.h>
#include <list>
#include <vector>
#include <tuple>
#include <map>
#include <unordered_map>

#include <algorithm>
#include "TNamed.h"
// Only open single usb module 

/*
bool cmpSID( std::tuple< libusb_device_handle* , int, int, int, int> a,
	     std::tuple< libusb_device_handle* , int, int, int, int> b){
  return std::get<1>(a) < std::get<1>(b);
}
*/
class NKUSB : public TNamed {
 public:
  NKUSB(std::string name, unsigned short vid, unsigned short pid, unsigned short sid);
  ~NKUSB();
  int USBInit();
  int USBOpen();
  int USBClose();
  int USBExit();
  int USBRead(uint32_t count, uint32_t  addr, char* data,uint32_t mid=0);
  int USBWrite(uint32_t addr,  uint32_t data, uint32_t  mid = 0);
  int USBReadReg(uint32_t addr);
  int USBReadControl( uint8_t  bRequest, uint16_t wValue, uint16_t wIndex, char *data, uint16_t wLength);
  int USBWriteControl( uint8_t bRequest, uint16_t wValue, uint16_t wIndex, char *data, uint16_t wLength);
  
  int DeviceList();
  int GetSID(libusb_device_handle *devh);
  int USBFind();
  int USBClaim();
  int USBRelease();
  int USBReset();
  unsigned short vendorID;
  unsigned short productID;
  unsigned short serialID;
  bool  isOpen;
  int isClaimed;
  int interface;
  int speed;
  int bus;
  int deviceAddr;
  libusb_context** ctx;
  struct libusb_device_handle *cdevh;
  unsigned  char* buffer;
  unsigned  int   timeout;
  ClassDef( NKUSB, 1);
};

class NKUSBMulti : public TNamed {
public:
  NKUSBMulti( std::string name, unsigned short vid, unsigned short pid );
  ~NKUSBMulti();
  int USBInit();
  int USBExit();
  int USBOpen();
  int USBClose();

  unsigned int GetSID( libusb_device_handle *devh);
  static  bool SortSID(std::tuple<libusb_device_handle*,  int, int, int, int, int> a,
	       std::tuple<libusb_device_handle*,  int, int, int, int, int> b);
  struct libusb_device_handle* GetDevh( int sid );
  int MakeList();
  //int OpenList();
  int CloseList();
  void PrintList();

  unsigned int USBWrite(int sid, unsigned int addr,unsigned int data, unsigned int mid );//mid is FADC's sid
  unsigned int USBWrite(int sid, unsigned int addr,unsigned int data);
  unsigned int USBRead( int sid,unsigned int count,unsigned int addr, char* data, unsigned int mid );// mid is FADC's sid
  unsigned int USBRead( int sid,unsigned int count,unsigned int addr, char* data);

  unsigned int USBReadReg( int sid, unsigned int addr );
  unsigned int USBReadReg( int sid, unsigned int addr, unsigned int mid  );
  unsigned int USBReadControl( int sid, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, char *data, uint16_t wLength);
  unsigned int USBWriteControl( int sid, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, char *data, uint16_t wLength);
  unsigned int USBReset( int sid );
  
  std::map< int, struct libusb_device_handle* > map_sid_devh;
  std::vector< std::tuple<struct libusb_device_handle*, int, int, int, int, int >  > devhList;// dev, sid, bus, deviceAddr, speed, open (0/1)
  int  Status;
  unsigned short vendorID;
  unsigned short productID;
  int interface;
  unsigned char* cmdbuffer;
  //unsigned char* buffer[40];/// 40 : max FADC / pc
  unsigned int timeout;
  libusb_context **ctx;

  /// Handle Parameter
  bool ReadParameterMap(  std::string parName );
  bool WriteParameterMap( std::string parName );
  int SetParameter( int sid, std::string ParName, unsigned long ch, unsigned long val);
  int SetParameter( int sid, std::string ParName, unsigned long ch, unsigned long val,unsigned int mid);
  unsigned long GetParameter(int sid, std::string ParName,unsigned long ch);
  unsigned long GetParameter(int sid, std::string ParName,unsigned long ch, unsigned int mid);
  unsigned long AddrCHMapping( unsigned long addr, unsigned long ch );
  void PrintParameter(std::string ParName);

  virtual bool Init(int sid);
  virtual bool Start(int sid);
  virtual bool Stop(int sid);
  virtual bool Terminate(int sid);
  virtual unsigned int Run(int sid, char* data);
  std::unordered_map< std::string, std::string >      map_parname_addr;//ParName Address(string)
  std::unordered_map< std::string, unsigned long >    map_parname_intAddr;
  std::unordered_map< std::string, std::vector<unsigned long> > map_parname_val;
  std::unordered_map< std::string, std::vector<unsigned long> > map_parname_rval;
  ClassDef( NKUSBMulti, 1 );
};

#endif
