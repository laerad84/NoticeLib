#include "NK_MiniFADC500.h"

NK_MiniFADC500::NK_MiniFADC500( std::string name )
:NKUSBMulti( name, 0x0547,0x1502)
{
    printf( "Notice USB : %04x:%04x %ld modules found.\n",vendorID, productID, map_sid_devh.size());
    for( int i = 0; i< devhList.size(); i++){
        printf( "SID,BUS,ADDR : %d %04x %04x\n", 
            std::get<1>(devhList.at(i)),
            std::get<2>(devhList.at(i)),
            std::get<3>(devhList.at(i)));
    }
}

NK_MiniFADC500::~NK_MiniFADC500(){
    ;
}
void NK_MiniFADC500::InitParameter(){
    std::cout << __PRETTY_FUNCTION__ << std::endl;  
    map_parname_intAddr = {
    {"SET"         , 0x20000000},
    {"CW"          , 0x20000001},
    {"RL"          , 0x20000002},
    {"DRAMON"      , 0x20000003},
    {"DACOFF"      , 0x20000004},
    {"MPED"        , 0x20000005},
    {"RPED"        , 0x20000006},
    {"DLY"         , 0x20000007},
    {"THR"         , 0x20000008},
    {"POL"         , 0x20000009},
    {"PSW"         , 0x2000000A},
    {"AMODE"       , 0x2000000B},
    {"PCT"         , 0x2000000C},
    {"PCI"         , 0x2000000D},
    {"PWT"         , 0x2000000E},
    {"DT"          , 0x2000000F},
    {"BCOUNT"      , 0x20000010},
    {"PTRIG"       , 0x20000011},
    {"TRIG"        , 0x20000012},
    {"TRIGENABLE"  , 0x20000013},
    {"TM"          , 0x20000014},
    {"TLT"         , 0x20000015},
    {"ZEROSUP"     , 0x20000016},
    {"ADCRST"      , 0x20000017},
    {"ADCCAL"      , 0x20000018},
    {"ADCDLY"      , 0x20000019},
    {"ADCALIGN"    , 0x2000001A},
    {"ADCSTAT"     , 0x2000001A},
    {"DRAMDLY"     , 0x2000001B},
    {"BITSLIP"     , 0x2000001C},
    {"DRAMTEST"    , 0x2000001D},
    {"PSCALE"      , 0x2000001E},
    {"DSR"         , 0x2000001F},
    {"RDATA"       , 0x40000000}};

    map_parname_val = {/// parameter name with default value
        {"DSR"    ,{1}},//sampling rate 1/2/4/8
        {"PTRIG"  ,{0}},//pedestal trigger interval, in ms
        {"CW"     ,{1000,1000,1000,1000}},//coincidence width
        {"RL"     ,{8}},// recording length 1,2,4,8,16,32,64,128,256 * 128ns
        {"DACOFF" ,{3500,3500,3500,3500}},// DAC Offset 0-4095
        {"MPED"   ,{0,0,0,0}},// measure ped 
        {"DLY"    ,{100,100,100,100}},// Trigger delay
        {"THR"    ,{50,50,50,50}},// Threashold
        {"POL"    ,{0,0,0,0}},// polarity 0 = negative, 1 = positive
        {"PSW"    ,{2,2,2,2}},/// peak sum width in ns 2-16382
        {"AMODE"  ,{1,1,1,1}},/// adc mode 0:raw, 1: filtered
        {"PCT"    ,{1,1,1,1}},/// pulse count threshold(1~15)
        {"PCI"    ,{1000,1000,1000,1000}},// pulse count interval( 32 ~ 8160 ns)
        {"PWT"    ,{100,100,100,100}},// pulse width threshold
        {"DT"     ,{2000,2000,2000,2000}},// trigger dead time
        {"TM"     ,{1,1,1,1}},//trigger mode, 4bit, peak_sum_or | peak_sum | pulse_width | pulse_count
        {"TLT"    ,{0xFFFE}},
        {"PSCALE" ,{1}},//prescale 1~65535
        {"TRIGENABLE",{9}}// 4bit, External | rtrig | ptrig | self trig
    };
    map_parname_rval = {/// parameter name with default value
        {"DSR"    ,{1}},//sampling rate 1/2/4/8
        {"PTRIG"  ,{0}},//pedestal trigger interval, in ms
        {"CW"     ,{1000,1000,1000,1000}},//coincidence width
        {"RL"     ,{8}},// recording length 1,2,4,8,16,32,64,128,256 * 128ns
        {"DRAMON" ,{1}},// Dram status 0/1 Off/On
        {"DACOFF" ,{3500,3500,3500,3500}},// DAC Offset 0-4095
        {"MPED"   ,{0,0,0,0}},// measure ped 
        {"DLY"    ,{100,100,100,100}},// Trigger delay
        {"THR"    ,{50,50,50,50}},// Threashold
        {"POL"    ,{0,0,0,0}},// polarity 0 = negative, 1 = positive
        {"PSW"    ,{2,2,2,2}},/// peak sum width in ns 2-16382
        {"AMODE"  ,{1,1,1,1}},/// adc mode 0:raw, 1: filtered
        {"PCT"    ,{1,1,1,1}},/// pulse count threshold(1~15)
        {"PCI"    ,{1000,1000,1000,1000}},// pulse count interval( 32 ~ 8160 ns)
        {"PWT"    ,{100,100,100,100}},// pulse width threshold
        {"DT"     ,{2000,2000,2000,2000}},// trigger dead time
        {"TM"     ,{1,1,1,1}},//trigger mode, 4bit, peak_sum_or | peak_sum | pulse_width | pulse_count
        {"TLT"    ,{0xFFFE}},
        {"PSCALE" ,{1}},//prescale 1~65535
        {"TRIGENABLE",{9}}// 4bit, External | rtrig | ptrig | self trig
    };
}
void NK_MiniFADC500::NKStart( int sid ){
    USBWrite( sid, 0x20000000, 1<<3);
}
void NK_MiniFADC500::NKStop( int sid ){
    USBWrite( sid, 0x20000000, 0 << 3 );
}
void NK_MiniFADC500::NKReset( int sid ){
    USBWrite( sid, 0x20000000, 1<<2 );
}
void NK_MiniFADC500::NKResetTimer( int sid ){
    USBWrite( sid, 0x20000000, 1);
}
void NK_MiniFADC500::DRAMON( int sid, unsigned long data ){
    //std::cout<< __PRETTY_FUNCTION__ << ":"  <<  __LINE__  << std::endl;
    unsigned long status;
    if( data){
        //std::cout<< "READ0" << std::endl; 
        status = USBReadReg( sid, 0x20000003);
        if( status ){
            USBWrite( sid, 0x20000003, 0);
        }
        //std::cout<< "READ1 " << status << std::endl; 
        USBWrite(sid, 0x20000003, 1);
        status = 0;
        while ( !status ){
            //std::cout<< "READ2" << std::endl; 
            status = USBReadReg( sid, 0x20000003);
        }                
    }else{
        USBWrite( sid, 0x20000003, 0);
    }
}
void NK_MiniFADC500::WriteDLY( int sid, unsigned long ch, unsigned long data){
    //std::cout<< __PRETTY_FUNCTION__ << ":"  <<  __LINE__  << std::endl;
    unsigned long value;
    value = ((data/1000) << 10)| (data %1000);
    SetParameter( sid, "DLY", ch, value);
}
unsigned int NK_MiniFADC500::ReadDLY( int sid, unsigned long ch ){
    //std::cout<< __PRETTY_FUNCTION__ << ":"  <<  __LINE__  << std::endl;
    unsigned int value;
    unsigned int data;
    value=GetParameter( sid, "DLY", ch);
    data = ( value >> 10 )*1000 | (value & 0x3FF );
    return data;
}
unsigned int NK_MiniFADC500::ReadBCount( int sid ){
    return USBReadReg( sid, 0x20000010);
}
void NK_MiniFADC500::ReadData(int sid,int  bcount, char* data){
    unsigned int count = bcount * 256;
    USBRead( sid, count, map_parname_intAddr["RDATA"], data);
}
void NK_MiniFADC500::ADCALIGN( int sid ){
    std::cout<< __PRETTY_FUNCTION__ << ":"  <<  __LINE__  << std::endl;
    unsigned long ch, dly, value;
    int count, sum, center;
    unsigned long gdly;
    int flag;
    SetParameter( sid, "ADCRST",0,0);
    usleep(500000);
    SetParameter( sid, "ADCCAL",0,0);
    SetParameter( sid, "ADCALIGN",0,1);
    for( ch=0; ch<4;ch++){
        count = 0;
        sum   = 0;
        flag  = 0;
        for(  dly = 0; dly < 32; dly++ ){
            SetParameter( sid, "ADCDLY", ch, dly);
            value = (SetParameter(sid, "ADCSTAT",0,0) >>  ch) & 0x1;
            if(!value){
                flag=1;
                count=count+1;
                sum=sum+dly;
            }else{
                if(flag){
                    dly=32;
                }
            }
        }
        //std::cout << ch << " " << sum << " " << count << std::endl; 
        center  = sum/count;
        if(center<11){
            gdly=center+11;
        }else{
            gdly=center-11;
        }
        SetParameter(sid,"ADCDLY",ch,gdly);
        printf("ch%ld calibration delay = %ld\n",ch,gdly);
    }
    SetParameter(sid,"ADCALIGN",0,0);
    SetParameter(sid,"ADCCAL",0,0);
    
}
void NK_MiniFADC500::ADCALIGN_DRAM( int sid ){
    std::cout<< __PRETTY_FUNCTION__ << ":"  <<  __LINE__  << std::endl;
    unsigned long ch;
    unsigned long dly;
    unsigned long value;
    int flag;
    int count;
    int sum;
    int aflag;
    unsigned long gdly;
    int bitslip;
    int gbitslip;
    //SetParameter(sid,"DRAMON",0,1);
    DRAMON(sid, 1);
    SetParameter(sid,"DRAMTEST",0,1);
    SetParameter(sid,"ADCCAL",0,0);
    SetParameter(sid,"DRAMTEST",0,2);
    usleep(100);
    for( ch = 0; ch < 8; ch++){
        count = 0;
        sum = 0;
        flag = 0;
        for( dly = 0; dly < 32; dly++){
            SetParameter(sid, "DRAMDLY", ch, dly );
            SetParameter(sid, "DRAMTEST", 0,3 );
            value = GetParameter(sid,"DRAMTEST",ch);
            aflag = 0;
            if(      (value & 0xFFFFFFFF) == 0xFFAA5500){ aflag =1;}
            else if( (value & 0xFFFFFFFF) == 0xAA5500FF){ aflag = 1;}
            else if( (value & 0xFFFFFFFF) == 0x5500FFAA){ aflag = 1;}
            else if( (value & 0xFFFFFFFF) == 0x00FFAA55){ aflag = 1;}
            else{ printf("Wrong\n");}
            //printf("%ld %ld %08X %d %d\n", ch, dly, value, aflag, count);

            if( aflag ){
                count = count +1;
                sum = sum + dly;
                if( count > 4 ){ flag = 1; }            
            }else{
                if( flag ){ 
                    dly =32; 
                }else{
                    count = 0; 
                    sum = 0; 
                }
            }
        }
        //std::cout << flag << " " << count << " " << sum << " " << dly << std::endl; 
        if(count){ 
            gdly = sum/count;
        }else{ 
            gdly = 9;
        }
        SetParameter( sid, "DRAMDLY", ch,gdly);
        for( bitslip = 0; bitslip < 4; bitslip++){
            SetParameter( sid, "DRAMTEST",0,3);
            value = GetParameter( sid,"DRAMTEST",ch);
            if( (value&0xFFFFFFFF) == 0xFFAA5500){
                aflag = 1;
                gbitslip = bitslip;
                bitslip = 4;
            }else{
                aflag = 0;
                SetParameter(sid, "BITSLIP",ch,0);
            }  
        }

        if( aflag ){
            printf("DRAM(%ld) is aligned, delay = %ld, bitslip = %d\n",ch,gdly,gbitslip);
        }else{
            printf("Fail to align DRAM(%ld)! delay = %ld, bitslip = %d\n",ch,gdly,gbitslip);
        }
    }
    SetParameter( sid, "DRAMTEST", 0, 0);
}
void NK_MiniFADC500::Write_ACQUISITION_TIME( int sid, unsigned long long data){
    unsigned long msb;
    unsigned long lsb;
    unsigned long long tmp;
    lsb = data& 0xFFFFFFFF;
    tmp = (data >> 32 )& 0xFFFFFFFF;
    msb = tmp & 0xFFFFFFFF; 
    USBWrite( sid, 0x2000001E, lsb);
    USBWrite( sid, 0x2000001F, msb);

}
unsigned long long NK_MiniFADC500::Read_LIVETIME( int sid){
    unsigned long long data;
    unsigned long lsb;
    unsigned long msb;
    lsb = USBReadReg( sid, 0x2000001E);
    msb = USBReadReg( sid, 0x2000001F);
    data = msb;
    data = data << 32;
    data = data + lsb;
    return data;
}

bool NK_MiniFADC500::Init(int sid){
    std::cout<< __PRETTY_FUNCTION__ << ":" << __LINE__ << std::endl;
    SetParameter(sid,"DSR", 0 ,map_parname_val["DSR"][0]);
    NKResetTimer(sid);
    NKReset(sid);
    ADCALIGN(sid);
    ADCALIGN_DRAM(sid);
    SetParameter(sid,"PTRIG", 0, map_parname_val["PTRIG"][0]);
    for( int i = 0; i < 4; i++){SetParameter(sid,"CW", i,map_parname_val["CW"][i]);}
    SetParameter(sid,"RL", 0,map_parname_val["RL"][0]);
    DRAMON(sid, 1);
    for( int i = 0; i < 4; i++){SetParameter(sid,"DACOFF",i,map_parname_val["DACOFF"][i]);sleep(1);}
    for( int i = 0; i < 4; i++){GetParameter(sid,"MPED"   ,i);}//,map_parname_val["MPED"][i]);}
    for( int i = 0; i < 4; i++){//SetParameter(sid,"DLY"   ,i+1,map_parname_val["DLY"][i]+map_parname_val["CW"][i]);
        WriteDLY( sid, i, map_parname_val["DLY"][i]+map_parname_val["CW"][i]);
    }
    for( int i = 0; i < 4; i++){SetParameter(sid,"THR"   ,i,map_parname_val["THR"][i]);}
    for( int i = 0; i < 4; i++){SetParameter(sid,"POL"   ,i,map_parname_val["POL"][i]);}
    for( int i = 0; i < 4; i++){SetParameter(sid,"PSW"   ,i,map_parname_val["PSW"][i]);}
    for( int i = 0; i < 4; i++){SetParameter(sid,"AMODE" ,i,map_parname_val["AMODE"][i]);}
    for( int i = 0; i < 4; i++){SetParameter(sid,"PCT"   ,i,map_parname_val["PCT"][i]);}
    for( int i = 0; i < 4; i++){SetParameter(sid,"PCI"   ,i,map_parname_val["PCI"][i]);}
    for( int i = 0; i < 4; i++){SetParameter(sid,"PWT"   ,i,map_parname_val["PWT"][i]);}
    for( int i = 0; i < 4; i++){SetParameter(sid,"DT"    ,i,map_parname_val["DT"][i]);}
    for( int i = 0; i < 4; i++){SetParameter(sid,"TM"    ,i,map_parname_val["TM"][i]);}
    SetParameter( sid, "TLT"       , 0, map_parname_val["TLT"][0]);
    SetParameter( sid, "PSCALE"    , 0, map_parname_val["PSCALE"][0]);
    SetParameter( sid, "TRIGENABLE", 0, map_parname_val["TRIGENABLE"][0]);
    Check( sid );
    return true;
}
bool NK_MiniFADC500::Check( int sid ){
    for( auto a: map_parname_rval ){
        if( a.second.size() == 1 ){
            (a.second)[0] = GetParameter( sid, a.first, 0 );
        }else{
            for( int i = 0; i< 4; i++){
                a.second[i] = GetParameter( sid, a.first, i );
            }
        }
    }
    /*
    map_parname_rval["DSR"][0]    = GetParameter( sid, "DSR",0);
    map_parname_rval["RL" ][0]    = GetParameter( sid, "RL",0);
    map_parname_rval["DRAMON"][0] = GetParameter( sid, "DRAMON",0);
    map_parname_rval["PTRIG"][0]  = GetParameter( sid, "PTRIG",0);
    for( int i = 0; i< 4; i++){ map_parname_rval["CW"][i]     = GetParameter( sid, "CW"    ,i+1);}
    for( int i = 0; i< 4; i++){ map_parname_rval["DACOFF"][i] = GetParameter( sid, "DACOFF",i+1);}
    for( int i = 0; i< 4; i++){ map_parname_rval["DLY"][i]    = GetParameter( sid, "DLY"   ,i+1);}
    for( int i = 0; i< 4; i++){ map_parname_rval["THR"][i]    = GetParameter( sid, "THR"   ,i+1);}
    for( int i = 0; i< 4; i++){ map_parname_rval["POL"][i]    = GetParameter( sid, "POL"   ,i+1);}
    for( int i = 0; i< 4; i++){ map_parname_rval["PSW"][i]    = GetParameter( sid, "PSW"   ,i+1);}
    for( int i = 0; i< 4; i++){ map_parname_rval["AMODE"][i]  = GetParameter( sid, "AMODE" ,i+1);}
    for( int i = 0; i< 4; i++){ map_parname_rval["PCT"][i]    = GetParameter( sid, "PCT"   ,i+1);}
    for( int i = 0; i< 4; i++){ map_parname_rval["PCI"][i]    = GetParameter( sid, "PCI"   ,i+1);}
    for( int i = 0; i< 4; i++){ map_parname_rval["PWT"][i]    = GetParameter( sid, "PWT"   ,i+1);}
    for( int i = 0; i< 4; i++){ map_parname_rval["DT"][i]     = GetParameter( sid, "DT"    ,i+1);}
    for( int i = 0; i< 4; i++){ map_parname_rval["TM"][i]     = GetParameter( sid, "TM"    ,i+1);}            
    map_parname_rval["TLT"][0]        = GetParameter( sid, "TLT",0);
    map_parname_rval["PSCALE"][0]     = GetParameter( sid, "PSCALE",0);
    map_parname_rval["TRIGENABLE"][0] = GetParameter( sid, "TRIGENABLE",0);
    */
    return true;
}
void NK_MiniFADC500::Print( int sid){
    for( auto a : map_parname_val){
        PrintParameter(a.first);
    }
}
bool NK_MiniFADC500::Start(int sid){
    NKReset(sid);
    NKStart(sid);
    return true;
}
bool NK_MiniFADC500::Stop(int sid){
    NKStop(sid);
    return true;
}
bool NK_MiniFADC500::Terminate(int sid){
    USBClose();
    return true;
}
unsigned int NK_MiniFADC500::Run(int sid, char* data){
    unsigned long bcount = ReadBCount( sid );
    ReadData( sid, bcount, data);
    return bcount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MiniFADCDAQ::MiniFADCDAQ(const char* name, const char* title, int RunID)
:DAQ(name, title, RunID)
{
    miniFADC = new NK_MiniFADC500("miniFADC");
    miniFADC->USBOpen();
    miniFADC->PrintList();
}
MiniFADCDAQ::~MiniFADCDAQ(){
    ;
}
void MiniFADCDAQ::DAQRInit(){
    miniFADC->InitParameter();
}
void MiniFADCDAQ::DAQRSet(){
    miniFADC->Init(5);
    miniFADC->Print(5);
}
void MiniFADCDAQ::DAQRRun(){
    //char* data = (char*)malloc( 10*1024*1024);
    unsigned int bcount = miniFADC->ReadBCount( 5 );
    if( bcount > 10*1024){
        bcount = 10*1024;
    }
    miniFADC->ReadData( 5, bcount,dataIO);
    rDataVec.resize(bcount*512);
    std::memcpy(&rDataVec[0],&dataIO[0],bcount*1024);
    m.lock();
    DataArr.push(rDataVec);  
    m.unlock();
}
void MiniFADCDAQ::DAQRStop(){
    miniFADC->NKStop(5);
}
void MiniFADCDAQ::DAQWInit(){
}
void MiniFADCDAQ::DAQWSet(){
    rootFile = new TFile(Form("%s_%s_%d.root",GetName(),GetTitle(),info.runID),"recreate");
    rootTree = new TTree("DataTree","");
    rootTree->Branch("DataVec",&wDataVec);
}
void MiniFADCDAQ::DAQWRun(){
    wDataVec = DataArr.front();
    m.lock();
    DataArr.pop();
    m.unlock();
    rootTree->Fill();
}
void MiniFADCDAQ::DAQWStop(){
    rootTree->Write();
    rootFile->Close();
}