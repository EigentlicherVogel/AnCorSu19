#include <math.h>
#include <stdio.h>
#include <stdlib.h> 
#include <cstring>
#include <string>
#include <iostream>
#include <bitset>
#include <dirent.h>

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

u_short GetBitOnes(u_short i);

char* ToCstr(std::string str);

int checkTHead(FILE *F);
int checkFHead(FILE *F);

int readData(FILE* cur_dat, TFile* FIL);

static int MIN_END_BEGIN_COUNT = 16000;
static u_short VSN_CHANNEL_COUNT = 16;
//static int ACTUAL_VSN_MAX = 13;

int BIN_CT = 1000;
double MIN_HIST = 0.0;
double MAX_HIST = 4000.0;

bool iSwap = true;
bool THead = true;
bool FHead = true;

size_t total_ev_read = 0;
size_t total_buf_read = 0;

u_short ga_inv[300];



int main(){

	FILE *cur_data;
	size_t tapeN = 0;
	size_t fileN = 0;
    
	DIR *LDire1; 
	struct dirent *DirOfDir;
	LDire1 = opendir("./Ternary");

	DIR *LDire2;
	struct dirent *DirOfFil;


	while((DirOfDir = readdir(LDire1)) != NULL){
		if(strcmp(DirOfDir->d_name, ".") != 0 
			&& strcmp(DirOfDir->d_name, "..") != 0){
			//Skip the default files , ./ and ../
			tapeN++;
			if ( DirOfDir->d_type == DT_DIR ){
				//If is a directory

				char buf1[50];
				strcpy(buf1, "./Ternary/");
				strcat(buf1, DirOfDir->d_name);
				strcat(buf1, "/");
				std::cout << buf1 << std::endl;	
				LDire2 = opendir(buf1);	
				//Manually construct the directory
				

				while((DirOfFil = readdir(LDire2)) != NULL){
					//Second Loop inside sub-dir

					fileN++;
					if ( DirOfFil->d_type != DT_DIR ){
						//Don't go deeper

						char buf2[50];
						strcpy(buf2, "./Ternary/");
						strcat(buf2, DirOfDir->d_name);
						strcat(buf2, "/");
						strcat(buf2, DirOfFil->d_name);
						std::cout << "Reading file: " << buf2 << std::endl;	
						//Ditto
						

						cur_data = fopen(buf2,"rb");
						//Open the file and inspect

						int c = fgetc(cur_data);
						if(c == EOF){
							//Check if the file is empty
							ungetc(c, cur_data);
							std::cout << buf2 << " is an empty file." << std::endl;
							continue;
						}else{
							ungetc(c, cur_data);


							if(strcmp(DirOfFil->d_name,"file1.dat") == 0){
								THead = true;
								FHead = true;
							}else{
								THead = false;
							}

							std::string filN = "EHTape" + std::to_string(tapeN) + "-" + DirOfFil->d_name + ".root";
							TFile* FI = TFile::Open(ToCstr(filN),"RECREATE");
							//Creates TFile for histograms 

							int a = readData(cur_data, FI);
							if(a != 0){
								return -1;
							}
							//Read each file
						}
						std::cout << "" << std::endl;
					}
				}
				std::cout << "" << std::endl;
				fileN = 0;	
			}
		}
	}
	//}
	std::cout << "Finished! " << tapeN << " tapes read in total." << std::endl;
}



int readData(FILE* cur_dat, TFile* FIL){

	TH1S *ext_histd[VSN_CHANNEL_COUNT][16];
	//Create 2D array of pointers to histograms
	
	
	for(int i = 0;i < VSN_CHANNEL_COUNT;i++){
		for(int j = 0;j < 16;j++){		
		
			std::string chanL = "ext_d[" + std::to_string(i+1) + "][" + std::to_string(j+1)+ "]";
			std::string nameL = "ext_d[" + std::to_string(i+1) + "][" + std::to_string(j+1)+ "]";
  			//Convert both strings to cstrs, understandable by Branch
			
			ext_histd[i][j] = new TH1S(ToCstr(chanL), ToCstr(nameL), BIN_CT, MIN_HIST, MAX_HIST);
			//Instantiate the array
		}
	}



	if(THead){
		checkTHead(cur_dat);
	}
	
	if(FHead){
		checkFHead(cur_dat);
	}
	
	int rot_count = 0;
	int ex_count = 0;
	int act_len = 0;
	int buf_size = 0;

	while(!feof(cur_dat)){

		buf_size = fread(&pBH,1,22,cur_dat);
		if(buf_size != 22){
			return -3;
		}
		rot_count += 22;

		if(iSwap){
				SwapBits((char*)&pBH.RecordType,2);
				SwapBits((char*)&pBH.RecordLength,2);
				SwapBits((char*)&pBH.ModeFlags,2);
				SwapBits((char*)&pBH.DataLength,2);
		}
		if(pBH.RecordType != 3){

			std::cout << "Erronous Record Type Is "<< pBH.RecordType << std::endl;
			std::cout << "Erronous File Pointer At " << ftell(cur_dat) << std::endl;
			return 0;

		}else{
			
			total_buf_read++;
			
			int rot_max = (pBH.DataLength * 2);
			
			int next_char;
			
			while(rot_count < rot_max){

				buf_size = fread(&pEH,1,20,cur_dat);
				rot_count += 20;
		
				if(iSwap){
					SwapBits((char*)&pEH.len,2);
					SwapBits((char*)&pEH.len_clean,2);
				}

				if(rot_count < pBH.RecordLength){
					act_len = pEH.len & 0x00ff;
				}else{
					act_len = 0;
				}	
				//Read total data length

				int gamma_ct = pEH.len_clean & 0x00ff;
				//Read number of gammas

				ex_count = (act_len) - 10 - (gamma_ct*4);
				//Calculate external data length
				
				fseek(cur_dat, gamma_ct*8 ,SEEK_CUR);
				rot_count += gamma_ct * 8;
			

				fread(ga_inv, 2 , ex_count , cur_dat);
			
				for(int i = 0;i < ex_count;i++){
					SwapBits((char*)&ga_inv[i],2);
					//Swap all bits of the data
					rot_count += 2;
				}

				//Load to histogram
							
				u_short cur_vsn = 0;
				int chan_loc = 0;
				bool in_chan = false;
				
				for(int j = 3;j < ex_count - 1;j++){
					
					if (ga_inv[j] == 0x8b0a){
						if(cur_vsn == 8) {
						in_chan = false;
						continue; 
						}
					}
					//Skips 8b0a when channel count is at their location
					
					if(!in_chan && ga_inv[j] == 0x0000){
						cur_vsn++; //If not reading a channel and reads a 0000, it's because there is an empty channel
						in_chan = false;
						continue;
					}
					
					if(!in_chan){
						chan_loc = GetBitOnes(ga_inv[j]);
						in_chan = true;
						continue;
						//At a channel start, read how many data words are in channel
					}
					
					if(in_chan){
						u_short chan_flag = (ga_inv[j] & 0xf000) >> 12;
						u_short chan_data = (ga_inv[j] & 0x0fff);
						total_ev_read++;//Read two parts of every single data word

						if(cur_vsn < 8){
							ext_histd[cur_vsn][chan_flag]->Fill(chan_data);
							chan_loc -= 1;
						}else{
							ext_histd[cur_vsn][chan_flag]->Fill(chan_data);
							chan_loc -= 1;
						}
					}
					
					if(chan_loc == 0){
						cur_vsn++;
						in_chan = false; //Start new data word
					}							
				}
				

				//After each record, check if the next record starts with the char 80 (80xx)
				next_char = getc(cur_dat);
				if(next_char != 128){
					//If something does not start from 80, then it signifies the end of event buffers at its tail
					if(rot_count < MIN_END_BEGIN_COUNT){
						std::cout << "File Pointer Of Aberration At " << ftell(cur_dat) << std::endl;
						std::cout << "Rotation Count at Aberration is " << rot_count << std::endl;
					}//But if it's not at the end of record, then somthing is wrong
					//If it detects such an error, then forward it to 16384 directly as the following data would be useless
					while(rot_count < 16384){
						next_char = getc(cur_dat);
						rot_count++;
					}
					ungetc(next_char, cur_dat);
				}else{
					ungetc(next_char, cur_dat);
				}	

				//Also, if the rot_count ever exceeds 16384, roll back to 16384
				if(rot_count > 16384){
					std::cout << "Error: Rotation Count Out of bounds at:" << rot_count << std::endl;
					std::cout << "File Pointer Of Error At " << ftell(cur_dat) << std::endl;
					int ubergross = 16384 - rot_count; //Es mussen negativ ist
					if(fseek(cur_dat, ubergross, SEEK_CUR) == 0){
						std::cout << "Breaking; rolling back pointers by " << ubergross << std::endl;
					}else{
						std::cout << "Error in rolling back pointer!!" << std::endl;
					}
					break;
				}	
				
			}

			//If the rot_count does not reach 16384 at the end of each buffer, then forward the pointer to 16384
			if(rot_count < 16384){
				while(rot_count < 16384){
					next_char = getc(cur_dat);
					rot_count++;
				}
			}

			rot_count = 0;

		}

		
		fflush(stdout);
	}

	std::cout << "Read " << total_buf_read  << " Buffers and " << total_ev_read << " Events in this file." << std::endl;
	
	
	for(int i = 0;i < VSN_CHANNEL_COUNT;i++){
		for(int j = 0;j < 16;j++){
			ext_histd[i][j]->Write();
		}
	}

	FIL->Close();
	fclose(cur_dat);

	return 0;
}

u_short GetBitOnes(u_short i){
	// Returns the number of set bits in a u_short or any 16-bit data type

     std::bitset<16> bitst (i);
     return bitst.count();
}

void SwapBits(char *str, size_t n){
	//=================================================================
	//== Swaps each two bytes
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

char* ToCstr(std::string str){
	char * tocstr = new char [str.length()+1];
	std::strcpy (tocstr, str.c_str());
	return tocstr;
}

int checkTHead(FILE *F){
	size_t size = fread(&pTH,1,112,F);
	if(size != 112)
	{
		std::cout << "Tape Header Error!" << std::endl;
		return -1;
	}
	if(iSwap)
	{
		SwapBits((char*)&pTH.RecordType,2);
		SwapBits((char*)&pTH.RecordLength,2);
		SwapBits((char*)&pTH.RecordVer,2);
		SwapBits((char*)&pTH.TapeNum,2);
		SwapBits((char*)&pTH.TapeUnit,2);
	}
	std::cout << pTH.exp_title1 << " " << pTH.exp_title2 << std::endl; 
	std::cout << "Time at " << pTH.time << std::endl;
	std::cout << "Date at " << pTH.date << std::endl;
	return 0;
}

int checkFHead(FILE *F){
	size_t size = fread(&pFH,1,90,F);
	if(size != 90)
	{
		std::cout << "File Header Error!" << std::endl;
		return -2;
	}
	if(iSwap)
	{
		SwapBits((char*)&pFH.RecordType,2);
		SwapBits((char*)&pFH.RunNumber,2);
		SwapBits((char*)&pFH.FileNumber,2);
	}
	//std::cout << "File No." << pFH.FileNumber << std::endl; 
	return 0;
}
