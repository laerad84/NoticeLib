#include "NKUSB.h"
ClassImp(  NKUSB )

NKUSB::NKUSB(std::string name, unsigned short vid, unsigned short pid, unsigned short sid)
: TNamed( name.c_str(),Form("%d:%d:%d",vid,pid,sid) )
{
  vendorID   = vid;
  productID  = pid;
  serialID   = sid;
  isClaimed  = 0;
  isOpen     = false;
  interface  = 0;
  buffer =  (unsigned char*)malloc(16384);
  timeout   = 1000;
  USBInit();
  ctx =  0;
}
NKUSB::~NKUSB(){
  USBExit();
  free(buffer);
}
int  NKUSB::USBInit(){
  int ret;
  //if(( ret = libusb_init(ctx)) < 0 ){
  if(( ret = libusb_init(0)) < 0 ){
    fprintf( stderr,"Failed to Initialize libusb \n");
    exit(1);
  }
  return ret;
}
int NKUSB::USBExit(){
  //libusb_exit(*ctx);
  libusb_exit(0);
  return 0;
}
int NKUSB::USBOpen(){
  struct libusb_device  **devs;
  struct libusb_device  *dev;
  struct libusb_device_handle *devh;
  int r = 0;
  int i = 0;
  // Get Device list
  //if( libusb_get_device_list( *ctx,  &devs )<  0){
  if( libusb_get_device_list( 0,  &devs )<  0){
    fprintf( stderr, "Cannot get device list.\n");
    exit(1);
  }
  int nOpen  = 0;
  
  while((dev = devs[i++])){// 
    struct libusb_device_descriptor desc;
    r = libusb_get_device_descriptor( dev,  &desc );
    if( r < 0){
      fprintf( stderr,"Cannot get device descriptor\n");
      continue;
    }
    if( desc.idVendor == vendorID  && desc.idProduct== productID ){
      r = libusb_open(dev, &devh);
      if( r < 0){
	      fprintf( stderr,"Cannot open device.\n");
	      continue;
      }
      if( libusb_claim_interface(devh, interface )  <  0 ){
	      fprintf( stderr,"Cannot claim interface 0 on device.\n");
	      libusb_close( devh );
	      continue;
      }
      int tsid  = GetSID( devh );
      if( tsid == serialID ){
	      speed = libusb_get_device_speed(dev);
	      fprintf( stderr,"USB %d:%d:%d with speed %d.\n",vendorID, productID,serialID,speed);
	      cdevh      = devh;
	      bus        = libusb_get_bus_number(dev);
	      deviceAddr = libusb_get_device_address(dev);
	      //libusb_release_interface( devh, interface );
	      nOpen++;
	      isOpen = true;
	      break;
      }
      libusb_release_interface(devh, interface );
      libusb_close(devh);
    }
  }
  libusb_free_device_list(devs, 1);
  if( nOpen ){ return 0;}
  else{
    return -1;
  }
}
int NKUSB::USBClose(){
  
  libusb_device *dev;
  struct libusb_device_descriptor  desc;
  libusb_release_interface( cdevh, interface );
  /*
  dev = libusb_get_device( cdevh );
  if( libusb_get_device_descriptor(dev, &desc ) < 0){
    fprintf( stderr, "Cannot get device  descriptor.\n");
    return -1;
  }
  */
  libusb_close(cdevh);
  return  0;
}

int  NKUSB::DeviceList(){
  int nDevice = 0;
  struct libusb_device  **devs;
  struct libusb_device  *dev;
  struct libusb_device_handle *devh;
  int r = 0;
  int i = 0;
  // Get Device list
  //if( libusb_get_device_list( *ctx,  &devs )<  0){
  if( libusb_get_device_list( 0,  &devs )<  0){
    fprintf( stderr, "Cannot get device list.\n");
    exit(1);
  }
  int nOpen  = 0;
  while((dev = devs[i++])){// 
    struct libusb_device_descriptor desc;
    r = libusb_get_device_descriptor( dev,  &desc );
    if( r < 0){
      fprintf( stderr,"Cannot get device descriptor\n");
      continue;
    }
    
    int tbus        = libusb_get_bus_number(dev);
    int tdeviceAddr = libusb_get_device_address(dev);
    int tspeed      = libusb_get_device_speed(dev);
    if( desc.idVendor == vendorID  && desc.idProduct== productID ){
      nDevice++;
      r = libusb_open(dev, &devh);
      if( r < 0){
      	fprintf(stderr, "Cannot open  : VID:%04X PID:%04X BUS:%d ADDR:%d  SPD:%d\n",vendorID,  productID,   tbus, tdeviceAddr,  tspeed);
	      continue;
      }
      if( libusb_claim_interface(devh, interface )  <  0 ){
	      fprintf(stderr, "Cannot claim : VID:%04X PID:%04X BUS:%d ADDR:%d  SPD:%d\n",vendorID,  productID,   tbus, tdeviceAddr,  tspeed);
	      libusb_close( devh );
	      continue;
      }
      int tsid  = GetSID( devh );
      fprintf(stderr, "Claimed      : VID:%04X PID:%04X SID:%04X  BUS:%d ADDR:%d  SPD:%d\n",vendorID,  productID, tsid, tbus, tdeviceAddr,  tspeed);
      libusb_release_interface(  devh,interface  );
      nOpen++;
      libusb_close(devh);
    }
  }
  fprintf(stderr,"%d/%d  devices found.\n", nOpen,nDevice);
  return nDevice;
}

int NKUSB::GetSID(libusb_device_handle *devh){
  if( !devh){ exit(1);}

  int ret;
  unsigned char data[1];
  ret = libusb_control_transfer( devh, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN, 0xD2,0,0, data, 1 , 1000);
  if( ret < 0 ){
    fprintf( stderr,  "Cannot get serial ID.\n");
    return 0;
  }
  return (int)( data[0] &0xFF);
}
int NKUSB::USBFind(){
  return 0;
}
int NKUSB::USBClaim(){
  return 0;
}
int NKUSB::USBRelease(){
  return 0;
}
int NKUSB::USBRead(uint32_t count, uint32_t addr, char* data, uint32_t mid){
  std::cout<< __PRETTY_FUNCTION__ << std::endl; 
  int  transferred = 0;
  int length = 12;
  uint32_t taddr = addr |0x10000000;
  std::memcpy( &buffer[0], (void*)&count, 4);
  std::memcpy( &buffer[4],  (void*)&taddr, 4);
  std::memcpy( &buffer[8],   (void*)&mid, 4);
  int nbulk  = count / 4096;// 4096*int32= 16384 bytes
  int remain = count % 4096;
  int  size = 16384;
  int stat;
  if((stat = libusb_bulk_transfer(cdevh, 0x06, buffer, length, &transferred, timeout)) < 0){
    fprintf(stderr,"Cannot write(Read).\n");
    return  stat;
  }
  for( int loop  = 0;  loop  < nbulk; loop++){
    if(( stat = libusb_bulk_transfer( cdevh, 0x82, buffer,size,&transferred,  timeout))< 0){
      fprintf(stderr,"Cannot Read.\n");
      return stat;
    }
    std::memcpy(&data[0]+loop*size, buffer,size);
  }
  if(  remain ){
    if(( stat = libusb_bulk_transfer( cdevh, 0x82, buffer,size,&transferred,  timeout))< 0){
      fprintf(stderr,"Cannot Read.\n");
      return stat;
    }
    std::memcpy(&data[0]+nbulk*size,buffer, remain*4);
  }
  return stat;
}
int   NKUSB::USBWrite(uint32_t  addr, uint32_t data, uint32_t mid){
  std::cout<< __PRETTY_FUNCTION__ << std::endl; 
  int length = 12;
  int transferred=0;
  uint32_t taddr =  addr & 0x7FFFFFFF;
  std::memcpy( &buffer[0], (void*)&data, 4);
  std::memcpy( &buffer[4], (void*)&taddr, 4);
  std::memcpy( &buffer[8], (void*)& mid, 4);
  int stat;
  if((stat = libusb_bulk_transfer(cdevh, 0x06, buffer, length, &transferred,  timeout))<0){
    fprintf(stderr,"Cannot write.\n");
    USBReset();
    return stat;
  }
  usleep(1000);
  return stat;
}
int  NKUSB::USBReadReg(uint32_t addr){
  unsigned char data[4];
  unsigned int value;
  USBRead(1,addr,data);
  std::memcpy(&value,&data[0],4);
  return value;
}
int NKUSB::USBReadControl(uint8_t  bRequest, uint16_t wValue, uint16_t wIndex, char *data, uint16_t wLength){
  int stat =0;
  if((stat  = libusb_control_transfer( cdevh, LIBUSB_REQUEST_TYPE_VENDOR |  LIBUSB_ENDPOINT_IN, bRequest, wValue, wIndex, data, wLength, timeout))< 0){
    fprintf(stderr,"USBReadControl\n");
  }
  return  stat;
}
int  NKUSB::USBWriteControl( uint8_t bRequest, uint16_t wValue, uint16_t wIndex, char *data, uint16_t wLength){
  int stat = 0;
  if((stat = libusb_control_transfer( cdevh,  LIBUSB_REQUEST_TYPE_VENDOR |  LIBUSB_ENDPOINT_OUT, bRequest, wValue, wIndex, data, wLength, timeout))< 0){
    fprintf(stderr,"USBWriteControl\n");
  }
  return stat;
}
int  NKUSB::USBReset(){
  unsigned char data;
  USBWriteControl( 0xD6, 0, 0, &data, 0);
  return  0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ClassImp(  NKUSBMulti )

NKUSBMulti::NKUSBMulti(std::string name, unsigned short vid, unsigned short pid)
: TNamed( name.c_str(),Form("%d:%d",vid,pid) )
{
  std::cout<< __PRETTY_FUNCTION__  << std::endl;
  vendorID   = vid;
  productID  = pid;
  interface  = 0;
  Status  = 0;
  cmdbuffer =  (unsigned char*)malloc(12);
  /*
  for( int i = 0 ; i<  40; i++ ){
    buffer[i] =  (unsigned char*)malloc(16384);
  }
  */
  timeout   = 1000;
  USBInit();
}
NKUSBMulti::~NKUSBMulti(){
  std::cout<< __PRETTY_FUNCTION__  << std::endl;
  USBExit();
  //free(buffer);
}
int NKUSBMulti::USBInit(){
  std::cout<< __PRETTY_FUNCTION__  << std::endl;
  Status = 1;
  int ret;
  //if(( ret = libusb_init(ctx)) < 0 ){
  if(( ret = libusb_init(0)) < 0 ){
    fprintf( stderr,"Failed to Initializze libusb \n");
    exit(1);
  }
  return ret;
}
int NKUSBMulti::USBExit(){
  std::cout<< __PRETTY_FUNCTION__  << std::endl;
  Status = 0;
  //libusb_exit(*ctx);
  libusb_exit(0);
  return 0;
}
int NKUSBMulti::USBOpen(){
  std::cout<< __PRETTY_FUNCTION__  << std::endl;
  Status = 2;
  //MakeList();
  return MakeList();
}
int NKUSBMulti::USBClose(){
  std::cout<< __PRETTY_FUNCTION__  << std::endl;
  Status = 1;
  //CloseList();
  return CloseList();
}

unsigned int NKUSBMulti::GetSID(libusb_device_handle *devh){
  //std::cout<< __PRETTY_FUNCTION__  << std::endl;
  if( devh == nullptr ){ std::cout<< "DEVH is null.\n" << std::endl; }
  if( !devh ){ exit(1);}
  unsigned int ret;
  unsigned char data[1];
  ret = libusb_control_transfer( devh, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN, 0xD2,0,0, data, 1 , 1000);
  if( ret < 0 ){
    fprintf( stderr,  "Cannot get serial ID.\n");
    return 0;
  }
  ret = (unsigned int)(data[0] & 0xFF );
  return ret;
}
unsigned int NKUSBMulti::USBRead(int sid,uint32_t count, uint32_t addr, char* data, uint32_t mid){
  //std::cout<< __PRETTY_FUNCTION__ << std::endl; 
  struct libusb_device_handle* cdevh=GetDevh( sid );
  if(   cdevh  ==  nullptr ){  return  -1;}
  int  transferred = 0;
  int length = 12;
  std::memset( cmdbuffer, 0,  12);
  uint32_t taddr = addr |0x10000000;
    int nbulk  = count / 4096;// 4096*int32= 16384 bytes
  int remain = count % 4096;
  int size = 16384;
  int stat;
  char* buffer = (char *)malloc(size);
  //std::memcpy( &buffer[0], (void*)&count, 4);
  //std::memcpy( &buffer[4], (void*)&taddr, 4);
  //std::memcpy( &buffer[8], (void*)&mid, 4);
   
  buffer[0] = count &0xFF;
  buffer[1] = (count >> 8 )& 0xFF; 
  buffer[2] = (count >> 16 )& 0xFF; 
  buffer[3] = (count >> 24 )& 0xFF; 
  buffer[4] = (addr >> 0 ) & 0xFF;
  buffer[5] = (addr >> 8 ) & 0xFF;
  buffer[6] = (addr >> 16 ) & 0xFF;
  buffer[7] = ((addr >> 24 ) & 0xFF) | 0x80;
  buffer[8] = (mid >> 0 ) & 0xFF;
  buffer[9] = (mid >> 8 ) & 0xFF;
  buffer[10] = (mid >> 16 ) & 0xFF;
  buffer[11] = (mid >> 24 ) & 0xFF;

  //  if((stat = libusb_bulk_transfer(cdevh, 0x06, buffer, length, &transferred, timeout)) < 0){
    if((stat = libusb_bulk_transfer(cdevh, 0x06, buffer, length, &transferred, timeout)) < 0){
    fprintf(stderr,"Cannot write(Read).\n");
    return  stat;
  }
  for( int loop  = 0;  loop  < nbulk; loop++){
    if(( stat = libusb_bulk_transfer( cdevh, 0x82, buffer,size,&transferred,  timeout))< 0){
      //if(( stat = libusb_bulk_transfer( cdevh, 0x82, &data[0]+loop*size,size,&transferred,  timeout))< 0){
      fprintf(stderr,"Cannot Read.\n");
      return stat;
    }
    std::memcpy((void*)&data[0]+loop*size, buffer,size);
  }
  if(  remain ){
    if(( stat = libusb_bulk_transfer( cdevh, 0x82, buffer,size,&transferred,  timeout))< 0){
    //if(( stat = libusb_bulk_transfer( cdevh, 0x82, &data[0]+nbulk*size,remain*4,&transferred,  timeout))< 0){
      fprintf(stderr,"Cannot Read.\n");
      return stat;
    }
    std::memcpy((void *)&data[0]+nbulk*size,buffer, remain*4);
  }
  return stat;
}
unsigned int NKUSBMulti::USBRead( int  sid, unsigned int count, unsigned int addr, char* data){
  //std::cout<< __PRETTY_FUNCTION__ << std::endl; 
  libusb_device_handle* cdevh= GetDevh( sid );
  if( cdevh =  nullptr ){ return -1;}
  int  transferred = 0;
  int length = 8;
  std::memset( cmdbuffer, 0,  length);
  uint32_t taddr = addr | 0x10000000;
  //std::cout<< "SID : " << GetSID(GetDevh(sid)) << std::endl;
  int nbulk  = count / 4096;// 4096*int32= 16384 bytes
  int remain = count % 4096;
  int  size = 16384;
  char* buffer = (char *)malloc(size);
   
  buffer[0] = count &0xFF;
  buffer[1] = (count >> 8 )& 0xFF; 
  buffer[2] = (count >> 16 )& 0xFF; 
  buffer[3] = (count >> 24 )& 0xFF; 
  buffer[4] = (addr >> 0 ) & 0xFF;
  buffer[5] = (addr >> 8 ) & 0xFF;
  buffer[6] = (addr >> 16 ) & 0xFF;
  buffer[7] = ((addr >> 24 ) & 0xFF) | 0x80;

  // std::memcpy( &buffer[0], (void*)&count, 4);
  //std::memcpy( &buffer[4],  (void*)&taddr, 4);

  int stat;
  //std::cout << "Write" << std::endl; 
    //if((stat = libusb_bulk_transfer(cdevh, 0x06, buffer, length, &transferred, timeout)) < 0){ << not worked
    if((stat = libusb_bulk_transfer(GetDevh(sid), 0x06, buffer, length, &transferred, timeout)) < 0){
    fprintf(stderr,"Cannot write(Read).\n");
    return  stat;
  }
  //std::cout<< "Loop" << std::endl; 
  for( int loop  = 0;  loop  < nbulk; loop++){
//    if(( stat = libusb_bulk_transfer( cdevh, 0x82, &data[0]+loop*size,size,&transferred,  timeout))< 0){
    if(( stat = libusb_bulk_transfer( GetDevh(sid), 0x82, buffer,size,&transferred,  timeout))< 0){  
      fprintf(stderr,"Cannot Read.\n");
      return stat;
    }
    std::memcpy((void*)&data[0]+loop*size, buffer,size);
  }
  if(  remain ){
    if(( stat = libusb_bulk_transfer( GetDevh(sid), 0x82, buffer,remain*4,&transferred,  timeout))< 0){
    //if(( stat = libusb_bulk_transfer( cdevh, 0x82, &data[0]+nbulk*size,remain*4,&transferred,  timeout))< 0){
      fprintf(stderr,"Cannot Read.\n");
      return stat;
    }
    std::memcpy((void *)&data[0]+nbulk*size,buffer, remain*4);
  }
  return stat;  
}
unsigned int NKUSBMulti::USBWrite(int sid, uint32_t addr, uint32_t data, uint32_t mid){
  //std::cout<< __PRETTY_FUNCTION__ << std::endl; 
  struct libusb_device_handle* cdevh=GetDevh( sid );
  if(   cdevh  ==  nullptr ){  return  -1;}
  int length = 12;
  int transferred=0;
  std::memset(cmdbuffer,0, 12);
  uint32_t taddr =  addr & 0x7FFFFFFF;
  std::memcpy( &cmdbuffer[0], (void*)&data, 4);
  std::memcpy( &cmdbuffer[4], (void*)&taddr, 4);
  std::memcpy( &cmdbuffer[8], (void*)& mid, 4);
  int stat;
  if((stat = libusb_bulk_transfer(cdevh, 0x06, cmdbuffer, length, &transferred,  timeout))<0){
    fprintf(stderr,"Cannot write.\n");
    USBReset(sid);
    return stat;
  }
  usleep(1000);
  return stat;
}
unsigned int NKUSBMulti::USBWrite(int sid,unsigned int addr, unsigned int data){
  //std::cout<< __PRETTY_FUNCTION__ << std::endl; 
  struct libusb_device_handle* cdevh=GetDevh( sid );
  if(   cdevh  ==  nullptr ){  return  -1;}
  int length = 8;
  int transferred=0;
  std::memset(cmdbuffer,0, 12);
  uint32_t taddr =  addr & 0x7FFFFFFF;
  std::memcpy( &cmdbuffer[0], (void*)&data, 4);
  std::memcpy( &cmdbuffer[4], (void*)&taddr, 4);
  int stat;
  if((stat = libusb_bulk_transfer(cdevh, 0x06, cmdbuffer, length, &transferred,  timeout))<0){
    fprintf(stderr,"Cannot write.\n");
    USBReset(sid);
    return stat;
  }
  usleep(1000);
  return stat;
}
unsigned int NKUSBMulti::USBReadReg(int  sid,uint32_t addr){
  //std::cout<< __PRETTY_FUNCTION__ << std::endl;   
  unsigned char data[4];
  unsigned int value;
  USBRead(sid,1,addr,data);
  //std::memcpy( &value,&data[0],4);
  value = (unsigned int)(data[0] & 0xFF) 
        + (unsigned int)((data[1] & 0xFF ) << 8 )
        + (unsigned int)((data[2] & 0xFF ) << 16 )
        + (unsigned int)((data[3] & 0xFF ) << 24 );
  return value;
}
unsigned int NKUSBMulti::USBReadReg( int  sid, unsigned int addr,  unsigned int mid){
  unsigned char data[4];
  unsigned  int value;
  USBRead(sid, 1,addr,data,mid);
  //std::memcpy(&value,&data[0],4);
  value = (unsigned int)(data[0] & 0xFF) 
        + (unsigned int)((data[1] & 0xFF ) << 8 )
        + (unsigned int)((data[2] & 0xFF ) << 16 )
        + (unsigned int)((data[3] & 0xFF ) << 24 );
  return value;
}
unsigned int NKUSBMulti::USBReadControl(int sid,uint8_t  bRequest, uint16_t wValue, uint16_t wIndex, char *data, uint16_t wLength){
  struct libusb_device_handle* cdevh=GetDevh( sid );
  if(   cdevh  ==  nullptr ){  return  -1;}
  int stat =0;
  if((stat  = libusb_control_transfer( cdevh, LIBUSB_REQUEST_TYPE_VENDOR |  LIBUSB_ENDPOINT_IN, bRequest, wValue, wIndex, data, wLength, timeout))< 0){
    fprintf(stderr,"USBReadControl\n");
  }
  return  stat;
}
unsigned int NKUSBMulti::USBWriteControl(int sid, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, char *data, uint16_t wLength){
  struct libusb_device_handle* cdevh=GetDevh( sid );
  if(   cdevh  ==  nullptr ){  return  -1;}
  int stat = 0;
  if((stat = libusb_control_transfer( cdevh,  LIBUSB_REQUEST_TYPE_VENDOR |  LIBUSB_ENDPOINT_OUT, bRequest, wValue, wIndex, data, wLength, timeout))< 0){
    fprintf(stderr,"USBWriteControl\n");
  }
  return stat;
}
unsigned int NKUSBMulti::USBReset(int sid){
  char data;
  USBWriteControl(sid, 0xD6, 0, 0, &data, 0);
  return  0;
}


bool NKUSBMulti::SortSID(std::tuple< libusb_device_handle*, int, int, int, int, int> a,
			 std::tuple< libusb_device_handle*, int, int, int, int, int> b){
  return std::get<1>( a ) < std::get<1>( b );
}
struct libusb_device_handle*  NKUSBMulti::GetDevh( int  sid  ){
/*
  if( map_sid_devh.find(sid) == map_sid_devh.end() ){
    std::cout<< "SID "<< sid << "not found.\n " << std::endl; 
    return nullptr;
  } 
  return  map_sid_devh[sid];
  */
  for( int  index  =  0;  index <  (int)devhList.size();  index++  ){
    if( std::get<1>(devhList.at(index)) == sid  ){
      return std::get<0>(devhList.at(index));
    }    
  }
  return  nullptr;
  
}
int NKUSBMulti::MakeList(){
  std::cout<< __PRETTY_FUNCTION__  << std::endl;
  struct libusb_device **devs;
  struct libusb_device *dev;
  struct libusb_device_handle *devh;
  int r = 0;
  int i = 0;
  int sid = 0;
  int bus        = 0;
  int deviceAddr = 0;
  int speed      = 0;

  //if( libusb_get_device_list( *ctx, &devs ) < 0 ){
  if( libusb_get_device_list( 0, &devs ) < 0 ){
    exit(1);
  }
  int nOpen = 0;
  while((dev = devs[i++])){
    struct  libusb_device_descriptor desc;
    libusb_get_device_descriptor( dev, &desc );
    
    if((r=libusb_get_device_descriptor( dev, &desc)) < 0 ){
      fprintf( stderr, "Cannot get device descriptor.\n");
      continue;
    }

    if( desc.idVendor == vendorID &&
	    desc.idProduct == productID ){
      if(( r= libusb_open(dev, &devh ) )< 0 ){
	      fprintf( stderr,"Cannot open device.\n");
	      continue;
      }
      if( libusb_claim_interface( devh, interface ) < 0 ){
	      fprintf( stderr, "Cannot claim interface 0 on device.\n" );
	      libusb_close( devh );
	      continue;
      }
      sid = GetSID(devh);
      speed = libusb_get_device_speed(dev);
      bus   = libusb_get_bus_number( dev );
      deviceAddr = libusb_get_device_address( dev );
      devhList.push_back(std::make_tuple(devh, sid, bus, deviceAddr, speed,1 ));
					 //libusb_release_interface( devh,  interface );
      map_sid_devh.insert({ sid, devh} );
      nOpen++;
    }
  }
  std::cout<< __PRETTY_FUNCTION__ << ":"  <<  __LINE__  << std::endl;
  std::sort( devhList.begin(), devhList.end(),NKUSBMulti::SortSID);/// Sort by serial id
  std::cout<< __PRETTY_FUNCTION__ << ":"  <<  __LINE__  << std::endl;
  std::cout<< nOpen << " devices found. " << std::endl;
  return  nOpen;
}
int NKUSBMulti::CloseList(){
  std::cout<< __PRETTY_FUNCTION__  << std::endl;
  int  nClose =  0;
  for(  int index  = 0;  index <  (int)devhList.size();  index++ ){    
    if(  std::get<5>( devhList.at(index))  ==  0  ){ continue;}
    libusb_release_interface( std::get<0>(devhList.at(index)), interface );
    libusb_close( std::get<0>( devhList.at(index) ) );
    std::get<5>( devhList.at(index))  = 0;//Set Off
  }
  std::cout<< __PRETTY_FUNCTION__ << ":"  <<  __LINE__  << std::endl;
  return nClose;
}
void NKUSBMulti::PrintList(){
  std::cout<< __PRETTY_FUNCTION__  << std::endl;
  for( int index=0;  index  < (int)devhList.size(); index++ ){
    //std::tuple< libusb_device_handle*, int, int, int, int, int>
    fprintf(stdout,"%d %d %d %d %d\n",
	    std::get<1>(devhList.at(index)),
	    std::get<2>(devhList.at(index)),
	    std::get<3>(devhList.at(index)),
	    std::get<4>(devhList.at(index)),
	    std::get<5>(devhList.at(index)));    
  }
}
unsigned long NKUSBMulti::AddrCHMapping( unsigned long addr, unsigned long ch ){
  if( ch == 0 ){
    return addr;
  }else{
    //return addr + ((( ch -1 ) & 0xFF ) << 16 );
    return addr + ((ch & 0xFF) << 16);
  }
}
int NKUSBMulti::SetParameter(int sid, std::string ParName, unsigned long ch, unsigned long val ){
  //unsigned long addr = AddrCHMapping( std::stoul(map_parname_addr[ParName]), ch );
  /*
  std::cout<< __PRETTY_FUNCTION__ << ":" 
           << __LINE__ << ":" 
           << ParName << " " 
           << ch << " " 
           << val << std::endl;
  */ 
  if( map_parname_intAddr.find(ParName) == map_parname_intAddr.end()){
      printf( "No such Parameter: %s\n", ParName.c_str());
      return 0; 
  }
  unsigned long addr = AddrCHMapping(map_parname_intAddr[ParName],ch);
  return USBWrite( sid, addr, val);
}
int NKUSBMulti::SetParameter( int sid, std::string ParName, unsigned long ch, unsigned long val, unsigned int mid){
  //unsigned long addr = AddrCHMapping(  std::stoul( map_parname_addr[ParName]),  ch);
  /*std::cout<< __PRETTY_FUNCTION__ << ":" 
           << __LINE__ << ":" 
           << ParName << " " 
           << ch << " " 
           << val << std::endl;
  */
  if( map_parname_intAddr.find(ParName) == map_parname_intAddr.end()){
      printf( "No such Parameter: %s\n", ParName.c_str());
      return 0; 
  }
  unsigned long addr = AddrCHMapping(map_parname_intAddr[ParName],ch);
  return USBWrite( sid, addr, val, mid);
}
 unsigned long NKUSBMulti::GetParameter(int  sid, std::string ParName, unsigned long ch ){
  //unsigned long addr = AddrCHMapping(  std::stoul( map_parname_addr[ParName]),  ch);
  if( map_parname_intAddr.find(ParName) == map_parname_intAddr.end()){
      printf( "No such Parameter: %s\n", ParName.c_str());
      return 0; 
  }
  unsigned long addr = AddrCHMapping(map_parname_intAddr[ParName],ch);
  return USBReadReg( sid, addr);
}
 unsigned long NKUSBMulti::GetParameter(int  sid, std::string ParName, unsigned long ch, unsigned int mid){
  //unsigned long addr = AddrCHMapping(  std::stoul( map_parname_addr[ParName]),  ch);
  if( map_parname_intAddr.find(ParName) == map_parname_intAddr.end()){
      printf( "No such Parameter: %s\n", ParName.c_str());
      return 0; 
  }
  unsigned long addr = AddrCHMapping(map_parname_intAddr[ParName],ch);
  return USBReadReg( sid,addr,mid);
}
void NKUSBMulti::PrintParameter( std::string ParName){
  if( map_parname_val.find(ParName) == map_parname_val.end() || 
      map_parname_rval.find(ParName) == map_parname_rval.end() ){
        printf( "No such Parameter: %s\n ", ParName.c_str());
        return;
  }
  if( map_parname_val[ParName].size() == 1 ){
    printf("%s : %ld || %ld \n", ParName.c_str(), map_parname_val[ParName][0], map_parname_rval[ParName][0]);
  }else if( map_parname_val[ParName].size() == 4 ){
    printf("%s : %ld %ld %ld %ld || %ld %ld %ld %ld \n", 
          ParName.c_str(), 
          map_parname_val[ParName][0], 
          map_parname_val[ParName][1], 
          map_parname_val[ParName][2], 
          map_parname_val[ParName][3], 
          map_parname_rval[ParName][0],
          map_parname_rval[ParName][1],
          map_parname_rval[ParName][2],
          map_parname_rval[ParName][3]);
  }
  return;
}

bool NKUSBMulti::Init(int sid){;}
bool NKUSBMulti::Start(int sid){;}
bool NKUSBMulti::Stop(int sid){;}
bool NKUSBMulti::Terminate(int sid){;}
unsigned int NKUSBMulti::Run(int sid, char* data){;}
    