#ifndef DAQ__H__
#define DAQ__H__

#include <condition_variable>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <string>
#include <cstring>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include "THttpServer.h"
#include "rpc/server.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TNamed.h"
#include "TTimer.h"

enum DAQStatus{
  DWait,
  DSet,
  DRun,
  DStop,
  DTerminate
};

class RUNInfo : public TNamed{
 public:
  int runID;
  std::string daq_name;
  std::string mod_name;
  unsigned long TotalDataSize; 
  int status;
  MSGPACK_DEFINE_ARRAY( runID, daq_name, mod_name, status );
  ClassDef( RUNInfo, 1 )
};


class RWInfo{
public:
  int status;
  int EventID;
  ULong64_t rwSize;
  ClassDef( RWInfo, 1 )
};


class DAQ : public TNamed {
 public:
  DAQ();
  DAQ( const char* name, const char* title, int RunID );
  virtual ~DAQ();
  void Init();
  void Start();
  void Stop();
  void End();
  RUNInfo GetStatus(){ return info; }
  virtual void Read();
  virtual void Write();
  void Bind(rpc::server* srv);
  virtual void Register(THttpServer* serv);

  virtual void DAQRInit();
  virtual void DAQRSet();
  virtual void DAQRRun();
  virtual void DAQRStop();
  virtual void DAQWInit();
  virtual void DAQWSet();
  virtual void DAQWRun();
  virtual void DAQWStop();

  virtual void SetRunNumber(unsigned long rnum);
  virtual unsigned long GetRunNumber();
  std::queue<std::vector<unsigned short > > DataArr;
  std::mutex m;
  std::condition_variable cv;
  RUNInfo info;
  RWInfo  rinfo;
  RWInfo  winfo;

};

#endif
