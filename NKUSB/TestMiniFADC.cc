#include <unistd.h>
#include <cstdio>

#include "TH1.h"
#include "TH2.h"
#define PC_DRAM_SIZE 10
#define DATA_ARRAY_SIZE (PC_DRAM_SIZE*1024*1024)
#define CHUNK_SIZE      (PC_DRAM_SIZE*1024)
#include "NK_MiniFADC500.h"
#include <iostream>

int main( int argc, char** argv ){
//void TestMiniFADC(){
//R__LOAD_LIBRARY(lib/libNKUSB.so);
  NK_MiniFADC500* mini = new NK_MiniFADC500("MiniFADC");
  char* dataArr;
  dataArr = (char *)malloc(sizeof(char)*4*DATA_ARRAY_SIZE);
  //std::cout<< mini->DeviceList() << std::endl; 
  mini->USBOpen();
  mini->PrintList();
  mini->InitParameter();
  mini->Init(5);
  mini->Print(5);
  mini->Terminate(5);
  
}
