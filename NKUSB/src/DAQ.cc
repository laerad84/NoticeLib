#include "DAQ.h"

DAQ::DAQ()
  : TNamed( "DAQ","test")
{
  info.runID = 0;
  info.daq_name = "DAQ";
  info.mod_name = "test";
  info.status = 0;
}

DAQ::DAQ( const char* name, const char* title, int RunID)
  :  TNamed( name, title )
{
  info.runID = RunID;
  info.daq_name = name;
  info.mod_name = title;
  info.status = 0;
}
DAQ::~DAQ(){
  ;
}

void DAQ::Init(){
  printf("Init\n");
  if( info.status != DWait ){ std::cout<< "Wrong command" << std::endl; return;}
  m.lock();
  info.status = DSet;
  m.unlock();
  cv.notify_all();
}
void DAQ::Start(){
  printf("Start\n");
  if( info.status != DSet ){ std::cout<< "Wrong command" << std::endl; return;}
  m.lock();
  info.status = DRun;
  m.unlock();
  cv.notify_all();
}
void DAQ::Stop(){
  printf("Stop\n");
  if( info.status != DRun ){ std::cout<< "Wrong command" << std::endl; return;}
  m.lock();
  info.status = DStop;
  m.unlock();
  // Check all status are stop.
  std::unique_lock<std::mutex> lk( m );
  cv.wait( lk, [this]{return rinfo.status == DWait && winfo.status == DWait;  });
  lk.unlock();
  info.status = DWait;
  cv.notify_all();
  info.runID++;
}
void DAQ::End(){
  printf("End\n");
  if( info.status != DWait ){ std::cout<< "Wrong command" << std::endl; return;}
  m.lock();
  info.status = DTerminate;
  m.unlock();
  cv.notify_all();
}
void DAQ::Read(){
  while( info.status != DTerminate){
    std::cout<< __FUNCTION__ << __LINE__ << ":" << info.status << std::endl;
    rinfo.status = DWait;
    if( rinfo.status == DWait ){//wait
      std::unique_lock<std::mutex> lk( m );      
      cv.wait( lk, [this]{ return info.status == DSet || info.status == DTerminate;}); 
      lk.unlock();
      if( info.status == DTerminate ){ break; }
      DAQRInit();
    }
    std::cout<< __FUNCTION__ <<__LINE__ << ":" << info.status << std::endl;    
    rinfo.status = DSet; 
    if( info.status == DSet ){//set
      //// Write down Setting
      DAQRSet();
      std::unique_lock<std::mutex> lk( m );
      cv.wait( lk, [this]{ return info.status ==  DRun ;});
      lk.unlock();
    }
    std::cout<< __FUNCTION__ <<__LINE__ <<":" << info.status << std::endl;
    //do{
    rinfo.status = DRun;
    while( info.status == DRun ){
      //write down data read and copy to Que;
      DAQRRun();
    }
    std::cout<< __FUNCTION__ <<__LINE__<<  ":" << info.status << std::endl;
    rinfo.status = DStop;
    cv.notify_all();
    if( info.status == DStop ){ // stop 
      // if remain data == 0 signale to`
      if( rinfo.status == DStop){
	DAQRStop();
	m.lock();
	rinfo.status = DWait;
	m.unlock();
	cv.notify_all();      
	
	//ReadStop();
      }
      std::unique_lock<std::mutex> lk(m);
      cv.wait( lk, [this]{ return  info.status == 0; });
      lk.unlock();
    }
  }
  printf("Terminate\n");
}

void DAQ::Write(){
  while( info.status != DTerminate ){
    std::cout<< __FUNCTION__ << __LINE__ << ":" << info.status << std::endl;
    winfo.status = DWait;
    if( info.status == DWait ){
      std::unique_lock<std::mutex> lk( m );      
      cv.wait( lk, [this]{ return info.status == DSet || info.status == DTerminate;}); 
      lk.unlock();
      if( info.status == DTerminate ){ break; }
      DAQWInit();
    }
    std::cout<< __FUNCTION__ << __LINE__ << ":" << info.status << std::endl;
    winfo.status = DSet;
    if( info.status == DSet ){
      /// Setting 
      std::unique_lock<std::mutex> lk( m );
      cv.wait( lk, [this]{ return info.status ==  DRun ;});
      lk.unlock();
      DAQWSet();
    }    
    std::cout<< __FUNCTION__ << __LINE__ << ":" << info.status << std::endl;
    winfo.status = DRun;
    while( info.status == DRun || !DataArr.empty() ){
      std::unique_lock<std::mutex> lk( m );
      cv.wait( lk, [this]{ return !DataArr.empty() || info.status == DStop; } );
      lk.unlock();
      DAQWRun();
      if( info.status == DStop && DataArr.empty() ){ break;}
      
    }
    std::cout<< __FUNCTION__ << __LINE__ << ":" << info.status << std::endl;
    winfo.status =DStop;
    if( info.status == DStop ){ // stop
      if( winfo.status ==DStop ){
	std::unique_lock<std::mutex> lk(m);
	cv.wait( lk, [this]{ return rinfo.status ==DWait ;});
	lk.unlock();
	/// Stop Process;
	DAQWStop();
	winfo.status = DWait;
	cv.notify_all();
      }
      
      std::unique_lock<std::mutex> lk(m);
      cv.wait(lk,[this]{ return info.status == DWait; });
      lk.unlock();
    }
  }
  printf("Terminate\n"); 
}

void DAQ::Bind( rpc::server* srv ){
  srv->bind(Form("%s/%s/Init"  ,this->GetName(),this->GetTitle()) , [this]{ this->Init(); });
  srv->bind(Form("%s/%s/Start" ,this->GetName(),this->GetTitle()) , [this]{ this->Start();});
  srv->bind(Form("%s/%s/Stop"  ,this->GetName(),this->GetTitle()) , [this]{ this->Stop();});
  srv->bind(Form("%s/%s/End"   ,this->GetName(),this->GetTitle()) , [this]{ this->End();});
  srv->bind(Form("%s/%s/Status",this->GetName(),this->GetTitle()) , [this]{ return this->GetStatus();});
  srv->bind(Form("%s/%s/SetRunNumber",this->GetName(), this->GetTitle()), [this](unsigned int rnum){ this->SetRunNumber(rnum);});
}

void DAQ::Register( THttpServer* serv ){
  ;
}
void DAQ::DAQRInit(){
  printf("DAQRInit\n");
}
void DAQ::DAQRSet(){
  printf("DAQRSet\n");
}
void DAQ::DAQRRun(){
  printf("DAQRRun\n");
}
void DAQ::DAQRStop(){
  printf("DAQRStop\n");
}
void DAQ::DAQWInit(){
  printf("DAQWInt\n");
}
void DAQ::DAQWSet(){
  printf("DAQWSet\n");
}
void DAQ::DAQWRun(){
  printf("DAQWRun\n");
}
void DAQ::DAQWStop(){
  printf("DAQWStop\n");
}
void DAQ::SetRunNumber( unsigned long rnum ){
  info.runID = rnum;
}
unsigned long DAQ::GetRunNumber( ){
  return info.runID;
}
