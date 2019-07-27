#include <math.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <iostream>

#include "GSHeaders.h"

static tape_header		pTH;
static file_header		pFH;
static buffer_header	pBH;
static event_header		pEH;

void Swap(char *str, size_t n);


int ex_count = 0;
bool iSwap = true;

bool THead = true;
bool FHead = true;


void findCounts(u_short *arr, int n);

u_short dump[100];
		
u_short ga_inv[100];

u_short bigarr[16384];


int main(){

	FILE *cur_dat;
	FILE *output;
	cur_dat = fopen("file1.dat","rb");
	size_t size = fread(&pTH,1,112,cur_dat);
	output = fopen("Data","w");
	if(THead){ //If it is head of a tape
		if(size != 112)
		{
			return -1;
		}
		if(iSwap)
		{
			Swap((char*)&pTH.RecordType,2);
			Swap((char*)&pTH.RecordLength,2);
			Swap((char*)&pTH.RecordVer,2);
			//Swap((char*)&pTH.TapeNum,2);
			Swap((char*)&pTH.TapeUnit,2);
		}
		
		std::cout << pTH.exp_title1 << std::endl;

		fprintf(output,"%s %d %d %d"
		,"\nTape Info:" ,pTH.RecordType, pTH.RecordLength, pTH.TapeNum);
	}
	
	if(FHead){ //If it is head of a file
	size_t size = fread(&pFH,1,90,cur_dat);
		if(size != 90)
		{
			return -1;
		}
		if(iSwap)
		{
			Swap((char*)&pFH.RecordType,2);
			Swap((char*)&pFH.RunNumber,2);
			Swap((char*)&pFH.FileNumber,2);
		}
		
		fprintf(output,"%s %d %d %d"
		, "\nFile Info:" ,pFH.RecordType, pFH.RunNumber, pFH.FileNumber);
	}
	
	int rot_count = 0;
	
   //Read buffer header
	size_t size4 = fread(&pBH,1,22,cur_dat);
	std::cout << size4 << std::endl;
	
			if(iSwap)
		{
			Swap((char*)&pBH.RecordType,2);
			Swap((char*)&pBH.RecordLength,2);
			Swap((char*)&pBH.ModeFlags,2);
		}
			fprintf(output,"%s %d %d %x"
		, "\nEvent Info:" ,pBH.RecordType, pBH.RecordLength, pBH.ModeFlags);
		rot_count += 22;

	//Read files a byte a time
	while(rot_count <= 16384)
{
		size4 = fread(&pEH,1,20,cur_dat);
		
		//u_short *mover = &bigarr[rot_count];
	
		if(iSwap)
		{
			Swap((char*)&pEH.len,2);
			Swap((char*)&pEH.len_clean,2);
		}
			int act_len = pEH.len & 0x00ff;
			int gamma_ct = pEH.len_clean & 0x00ff;
			
		fprintf(output,"%s %d %d %s"
		, "\nEvent Info:" , act_len, gamma_ct, "\n");
		
		ex_count = (act_len) - 10 - (gamma_ct*4);
		rot_count += 20;
		
		fread(dump , 1, gamma_ct*8, cur_dat);
		rot_count += gamma_ct * 8;
		
		//u_short *ga_inv;
		//ga_inv = (u_short*)malloc(ex_count);

		size4 = fread(ga_inv, 2 , ex_count , cur_dat);
		
		rot_count += (ex_count * 2);
		
		for(int i = 0;i < ex_count;i++)
		{
		
		Swap((char*)&ga_inv[i],2);
		fprintf(output, "%04x ", ga_inv[i]);
		
		rot_count++;	
		//*ga_inv++;

		}
		//Seems to segfault because of this loop
		
		
		
	}
	//findCounts(bigarr, 16384);


}

void Swap(char *str, size_t n)
{
	//=================================================================
	//== Swap each two bytes
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

void findCounts(u_short *arr, int n) 
{ 
    // Traverse all array elements 
    int i = 0; 
    while (i<n) 
    { 
        // If this element is already processed, 
        // then nothing to do 
        if (arr[i] <= 0) 
        { 
            i++; 
            continue; 
        } 
  
        // Find index corresponding to this element 
        // For example, index for 5 is 4 
        int elementIndex = arr[i]-1; 
  
        // If the elementIndex has an element that is not 
        // processed yet, then first store that element 
        // to arr[i] so that we don't loose anything. 
        if (arr[elementIndex] > 0) 
        { 
            arr[i] = arr[elementIndex]; 
  
            // After storing arr[elementIndex], change it 
            // to store initial count of 'arr[i]' 
            arr[elementIndex] = -1; 
        } 
        else
        { 
            // If this is NOT first occurrence of arr[i], 
            // then decrement its count. 
            arr[elementIndex]--; 
  
            // And initialize arr[i] as 0 means the element 
            // 'i+1' is not seen so far 
            arr[i] = 0; 
            i++; 
        } 
    } 
  
    printf("\nBelow are counts of all elements\n"); 
    for (int i=0; i<n; i++) 
        printf("%d -> %d\n", i+1, abs(arr[i])); 
} 
