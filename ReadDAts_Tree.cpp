#include <math.h>
#include <stdio.h>
#include <stdlib.h> 
#include <cstring>
#include <string>
#include <iostream>
#include <bitset>

#include <TROOT.h>
#include <TObject.h>
#include <TSystem.h>
#include <TFile.h>
#include <TH1.h>
#include <TTree.h>

#include "GSHeaders.h"


static tape_header		pTH;
static file_header		pFH;
static buffer_header	pBH;
static event_header		pEH;

void SwapBits(char *str, size_t n);

u_short SetBits(u_short i);

static int MIN_END_BEGIN_COUNT = 16000;
static u_short VSN_CHANNEL_COUNT = 16;

int ex_count = 0;
int act_len = 0;
int buf_size = 0;
bool iSwap = true;

bool THead = true;
bool FHead = true;

static Long64_t maxtreesize = 1024 * 1024 * 1024;

static u_short BIT_DIGITs[16] =
  { 0x0001, 0x0002, 0x0004, 0x0008,
    0x0010, 0x0020, 0x0040, 0x0080,
    0x0100, 0x0200, 0x0400, 0x0800,
    0x1000, 0x2000, 0x4000, 0x8000
  };
    
u_short dump[100];
		
u_short ga_inv[100];

u_short bigarr[16384];

int main(){


	TFile FIL("Test Tree.root","RECREATE");
	TTree *T = new TTree("T", "Ex Data Test Tree");
	T->SetMaxTreeSize(maxtreesize);
	
	u_short ext_d[VSN_CHANNEL_COUNT][16];
	
	for(int i = 0;i < VSN_CHANNEL_COUNT;i++){
		for(int j = 0;j < 16;j++){
			std::string chanL = "ext_d[" + std::to_string(i+1) + "][" + std::to_string(j+1)+ "]";
			char * cstrC = new char [chanL.length()+1];
  			std::strcpy (cstrC, chanL.c_str());
  		/*
			std::string nameL = "ext_d[" + std::to_string(i+1) + "][" + std::to_string(j+1)+ "]/s";
			char * cstrN = new char [nameL.length()+1];
  			std::strcpy (cstrN, nameL.c_str());*/
  		
  			//Convert both strings to cstrs, understandable by Branch
  		
			T->Branch(cstrC, &ext_d[i][j] /*, cstrN */ );
			
			//delete[] cstrC;
			//delete[] cstrN;
		}
	}
	//Make the tree.
	//Both of these starts from 1 because the VSN serial numbers starts from 1 and there is no VSN channel 0.



	FILE *cur_dat;
	FILE *output;
	cur_dat = fopen("file1.dat","rb");
	size_t size = 0;
	output = fopen("Data-Cal","w");
	if(THead){
	size = fread(&pTH,1,112,cur_dat);
		if(size != 112)
		{
			return -1;
		}
		if(iSwap)
		{
			SwapBits((char*)&pTH.RecordType,2);
			SwapBits((char*)&pTH.RecordLength,2);
			SwapBits((char*)&pTH.RecordVer,2);
			//SwapBits((char*)&pTH.TapeNum,2);
			SwapBits((char*)&pTH.TapeUnit,2);
		}
		
		std::cout << pTH.exp_title1 << " " << pTH.RecordType << " " << pTH.RecordLength << std::endl;

		fprintf(output,"%s %d %d %d"
		,"\nTape Info:" ,pTH.RecordType, pTH.RecordLength, pTH.TapeNum);
	}
	
	if(FHead){
	size = fread(&pFH,1,90,cur_dat);
		if(size != 90)
		{
			return -1;
		}
		if(iSwap)
		{
			SwapBits((char*)&pFH.RecordType,2);
			SwapBits((char*)&pFH.RunNumber,2);
			SwapBits((char*)&pFH.FileNumber,2);
		}
		
		fprintf(output,"%s %d %d %d"
		, "\nFile Info:" ,pFH.RecordType, pFH.RunNumber, pFH.FileNumber);
	}
	
	int rot_count = 0;
	//size_t ind_file = fread(&pBH,1,22,cur_dat);
	

	while(!feof(cur_dat)){
	
		fread(&pBH,1,22,cur_dat);
		if(iSwap){
				SwapBits((char*)&pBH.RecordType,2);
				SwapBits((char*)&pBH.RecordLength,2);
				SwapBits((char*)&pBH.ModeFlags,2);
				SwapBits((char*)&pBH.DataLength,2);
			}
		 if(pBH.RecordType == 3){
				fprintf(output,"%s %d %d %d %x"
			, "\nBuffer Info:" ,pBH.RecordType, pBH.RecordLength, pBH.DataLength, pBH.ModeFlags);
				rot_count += 22;
				std::cout << "File Pointer At " << ftell(cur_dat) << std::endl;
			
			
			if(pBH.RecordLength != 16384){
			std::cout << "Record Byte Number Is "<< pBH.RecordLength << std::endl;
			std::cout << "File Pointer At " << ftell(cur_dat) << std::endl;
			//break;
			}
		
		
		int next_char;
		
		while(rot_count < (pBH.DataLength * 2)){

			buf_size = fread(&pEH,1,20,cur_dat);
			rot_count += 20;
		
			//u_short *mover = &bigarr[rot_count];
	
			if(iSwap){
					SwapBits((char*)&pEH.len,2);
					SwapBits((char*)&pEH.len_clean,2);
				}
			if(rot_count < pBH.RecordLength){
				act_len = pEH.len & 0x00ff;
			}else{
				act_len = 0;
			}
			int gamma_ct = pEH.len_clean & 0x00ff;
			
			fprintf(output,"%s %d %d %s"
			, "\nEvent Info:" , act_len, gamma_ct, "\n");
		
			ex_count = (act_len) - 10 - (gamma_ct*4);
			
		
			fread(dump , 1, gamma_ct*8, cur_dat);
			rot_count += gamma_ct * 8;
		
			//u_short *ga_inv;
			//ga_inv = (u_short*)malloc(ex_count);

			buf_size = fread(ga_inv, 2 , ex_count , cur_dat);
		
			//rot_count += (ex_count);
		
			for(int i = 0;i < ex_count;i++){
				SwapBits((char*)&ga_inv[i],2);
				fprintf(output, "%04x ", ga_inv[i]);
				rot_count += 2;
				//*ga_inv++;
			}
			
			u_short cur_vsn = 0;
			int chan_loc = 0;
			bool in_chan = false;
			
			for(int j = 2;j < ex_count - 1;j++){
				//SwapBits((char*)&ga_inv[j],2);
				/*if (j <= 1 || j == ex_count - 1) {
					in_chan = false;
					continue; //Skip first two and last one counts b/c they are format datas
				}*/

				
				if (ga_inv[j] == 0x8b01){
					if(cur_vsn == 0) {
					cur_vsn++;
					in_chan = false;
					continue; 
					}
				}
				if (ga_inv[j] == 0x8b0a){
					if(cur_vsn == 8) {
					cur_vsn++;
					in_chan = false;
					continue; 
					}
				}
				//Skips 8b01 and 8b0a's when channel count is at their location
				
				if(!in_chan && ga_inv[j] == 0x0000){
					cur_vsn++; //If not reading a channel and reads a 0000, it's because there is an empty channel
					in_chan = false;
					continue;
				}
				
				if(!in_chan){
   					chan_loc = SetBits(ga_inv[j]);
   					in_chan = true;
   					continue;
   					//At a channel start, read how many data words are in channel
				}
				
				if(in_chan){
					u_short chan_flag = (ga_inv[j] & 0xf000) >> 12;
					u_short chan_data = (ga_inv[j] & 0x0fff);
					//Read two parts of every single data word
					if(cur_vsn < 8){
						std::cout << "VSN: " << cur_vsn << std::endl;
						std::cout << "Flag: " << chan_flag << std::endl;
						std::cout << "Energy: " << chan_data << std::endl;
						ext_d[cur_vsn-1][chan_flag] = chan_data;
						chan_loc -= 1;
						
					//std::cout << "Reading channel " << chan_loc << " of vsn " << cur_vsn - 1 << std::endl;
					}else{
						std::cout << "VSN: " << cur_vsn-1 << std::endl;
						std::cout << "Flag: " << chan_flag << std::endl;
						std::cout << "Energy: " << chan_data << std::endl;
						ext_d[cur_vsn-2][chan_flag] = chan_data;
						chan_loc -= 1;
					//std::cout << "Reading channel " << chan_loc << " of vsn " << cur_vsn - 2 << std::endl;
					}
				}
				
				if(chan_loc == 0){
   					cur_vsn++;
					in_chan = false; //Start new data word
				}
			//std::cout << "Filling Tree " << ftell(cur_dat) << std::endl;
			
			}
			
			T->Fill();
			return 0;
			
			next_char = getc(cur_dat);
			
			if(next_char != 128){
				//If something does not start from 80 inside a record, then it signifies the end of event buffers at its tail
				
				if(rot_count < MIN_END_BEGIN_COUNT){
					std::cout << "File Pointer Of Aberration At " << ftell(cur_dat) << std::endl;
					std::cout << "Rotation COunt at Aberration is " << rot_count << std::endl;
				
				}//But if it's not at the end of record, then somthing is wrong
				
				while(rot_count < 16384){
							
				next_char = getc(cur_dat);
				rot_count++;
				}
				ungetc(next_char, cur_dat);
				
			}else{
			
				ungetc(next_char, cur_dat);
			}
			
			
		}

		//std::cout << "Loop Count Is " << rot_count << std::endl;
		//std::cout << "Loop End File Pointer At " << ftell(cur_dat) << std::endl;
		rot_count = 0;
		}
		//ind_file = fread(&pBH,1,22,cur_dat);
		
	fflush(stdout);

	}
	//findCounts(bigarr, 16384);

	FIL.Close();
	
	fclose(cur_dat);
	fclose(output);
}

u_short SetBits(u_short i)
// Returns the number of set bits in a u_short or any 16-bit data type
{
     std::bitset<16> bitst (i);
     return bitst.count();
}

void SwapBits(char *str, size_t n)
{
	//=================================================================
	//== SwapBits each two bytes
	//=================================================================

	char x;
	size_t  i;

	for(i = 1; i < n; i+=2)
	{
		x = str[i];
		str[i] = str[i-1];
		str[i-1] = x;
	}
}
