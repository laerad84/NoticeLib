#ifndef NK_MiniFADC500__H__
#define NK_MiniFADC500__H__

#include "NKUSB.h"
#include "DAQ.h"

class NK_MiniFADC500 : public NKUSBMulti {
    public:
    NK_MiniFADC500( std::string name);
    ~NK_MiniFADC500();  
    void InitParameter();
    void NKStart(int sid );
    void NKStop( int sid );
    void NKReset( int sid );
    void NKResetTimer( int sid );

    void DRAMON( int sid, unsigned long data);
    void WriteDLY( int sid, unsigned long ch, unsigned long data);
    unsigned int ReadDLY( int sid, unsigned long ch);

    unsigned int ReadBCount( int sid);
    void ReadData( int sid, int bcount, char* data);
    
    void ADCALIGN( int sid);
    void ADCALIGN_DRAM( int sid );
    void Write_ACQUISITION_TIME( int sid, unsigned long long data);
    unsigned long long Read_LIVETIME( int sid );

    virtual bool Init(int sid);    
    virtual bool Check(int sid);
    virtual void Print(int sid );
    virtual bool Start(int sid);
    virtual bool Stop(int sid);
    virtual bool Terminate(int sid);
    virtual unsigned int Run(int sid, char* data);

    ClassDef( NK_MiniFADC500, 1 );
};

class MiniFADCDAQ : public DAQ {
public:
    MiniFADCDAQ(const char* name, const char* title, int RunID);
    virtual ~MiniFADCDAQ();
    virtual void DAQRInit();
    virtual void DAQRSet();
    virtual void DAQRRun();
    virtual void DAQRStop();
    virtual void DAQWInit();
    virtual void DAQWSet();
    virtual void DAQWRun();
    virtual void DAQWStop();

    NK_MiniFADC500* miniFADC;
    TFile* rootFile;
    TTree* rootTree;
    int Length;
    unsigned short Data[1024*1024];// 2MB
    std::vector<unsigned short> rDataVec;
    std::vector<unsigned short> wDataVec;
    
    char dataIO[10*1024*1024];
    ClassDef( MiniFADCDAQ, 1);
};

#endif
