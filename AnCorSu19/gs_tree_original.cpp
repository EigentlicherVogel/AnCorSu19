//=====================================================================
//== Read Gammasphere data of the 2000 year experiment
//== Create a TTree
//==
//== Version: 08.12.2006
//== Version: 01.02.2006 - create small matrices for one transition
//== Version: 05.26.2010 - create TTree
//==
//== Andrey Daniel
//== e-mail: Andrey.V.Daniel@gmail.com
//=====================================================================
#include <TROOT.h>
#include <TObject.h>
#include <TSystem.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TTree.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include <unistd.h>

#include "GSHeaders.h"
#include "gs_parm.h"
#include "gs_file.h"

static tape_header		pTH;
static file_header		pFH;
static buffer_header	pBH;
static event_header		pEH;
static int              iSwap = 1;
u_short					*pData = pBH.EventData;

static u_short bitcode[16] =
  { 0x0001, 0x0002, 0x0004, 0x0008,
    0x0010, 0x0020, 0x0040, 0x0080,
    0x0100, 0x0200, 0x0400, 0x0800,
    0x1000, 0x2000, 0x4000, 0x8000
  };

static u_short bitset[16];

static int records;
static int events;
static int badrecords;
static int badevents;

static u_short DataLen;
static u_short RealDataLength;


static u_short ge_i[110];
static u_short ge_t[110];
static u_short ge_e[110];
static float dt[110][110];
static u_short da[110][110];

static Int_t nz;
static unsigned short ex;
static unsigned short ey;
static unsigned short isym;
static unsigned short ez[110];

gs_parm P;
gs_file F[205];
int nfiles;

int imsym[100];

ofstream eout("gs.err");


int  ReadParms();
int  SearchEndOfEvent(int ipos);
int  NextEvent(int ipos);
int  ReadEventHeader(int ipos);
int  ReadGamma(int ipos);
int  CheckGammaFired(int ipos);
int  CheckEH(size_t size);
int  ReadFH(FILE *ifile);
void Print1FH();
int  ReadTH(FILE *ifile);
void Print1TH();
void Swap(char *str, size_t n);
void SearchTape();


int main(int argc, char **argv)
{
#ifdef WIN32
	if(gSystem->Load("libhist.dll") < 0) return -2;
#endif

	size_t size;
	size_t FileRead;
	char title[80];

	int i1,i2,i3,ifile;
	int iret;
	int pos;
	int iprint;
	double delta;
	unsigned short id1,id2;
        Long64_t maxtreesize = 1024*1024;
	maxtreesize *= 1024;
	maxtreesize *= 4;


	FILE *ifile_data;
	FILE *ifile_current;


	if(ReadParms() < 0)
	{
		printf("Error of input parms \n");
		return -1;
	}

	sprintf(title,"%5hu%5hu%5hu%5hu",P.xmin,P.xmax,P.ymin,P.ymax); 
	TFile *FF = TFile::Open(P.fsavename,"RECREATE");
	TTree *T = new TTree("T", title);
	T->SetMaxTreeSize(maxtreesize);
	T->Branch("nz",    &nz,    "nz/I");
	T->Branch("ex",    &ex,    "ex/s");
	T->Branch("ey",    &ey,    "ey/s");
	T->Branch("isym",  &isym,  "isym/s");
	T->Branch("ez",    ez,     "ez[nz]/s");


	//== Loop by all files
	for(ifile = P.k1file; ifile < P.k2file; ++ifile)
	{
		events = 0;
		ifile_data = fopen(F[ifile].fname,"rb");
		if(!ifile_data)
		{
			printf("FILE: %s WAS NOT OPEN\n",F[ifile].fname);
			break;
		}
	
		printf("START %5d FILE: %s\n",ifile,F[ifile].fname);

		//== Read the same file for the new block of matrices

		FileRead = 0;
		if(F[ifile].kthead)
		{
			iret = ReadTH(ifile_data);
			if(iret < 0)
			{
				printf("%5d FILE: %s\n",ifile,F[ifile].fname);
				printf("ERROR OF TAPE HEADER IRET = %d \n",iret);
				return -1;
			}
			Print1TH();
			FileRead += 112;
		}
		if(F[ifile].kfhead)
		{
			iret = ReadFH(ifile_data);
			if(iret < 0)
			{
				printf("%5d FILE: %s\n",ifile,F[ifile].fname);
				printf("ERROR OF FILE HEADER IRET = %d \n",iret);
				return -1;
			}
			Print1FH();
			FileRead += 90;
		}

		iprint = 0;
		//== Loop by blocks of one file
		while ( (size=fread(&pBH, 1, 16384, ifile_data)) > 0 )
		{
			FileRead += size;
			if(P.kswap) Swap((char*)&pBH, size);
			records++;
//			if(events>2325540&&events<2325560)
//				printf("events are %10d \n",events);
			iret = CheckEH(size);
//			if(events>2325540&&events<2325560)
//				printf("events are %10d \n",events);
			if(iret < 0) 
			{
				badrecords++;
				break;
			}
 
			//== Loop by events in this record
			pos  = 0;
			while(pos < RealDataLength)
			{
				pos = NextEvent(pos);
				if(pos < 0) break;
		
				events++;
				if(CheckGammaFired(pos) < 0) break;
				pos = ReadGamma(pos);
				//== !!! Here we have gamma data
				//== Have we enough number of gamma rays 
				
				for(i1 = 0; i1 < pEH.len_clean; ++i1)
				{
					id1 = ge_i[i1] - 1;
					if(id1 >= 110) continue;

					for(i2 = 0; i2 < pEH.len_clean; ++i2)
					{
						id2 = ge_i[i2] - 1;
						if(id2 >= 110 || id2 == id1) continue;
						delta = ge_t[i1] - ge_t[i2] - dt[id1][id2];
						if(fabs(delta) > P.tmax) continue;
						ex = ge_e[i1] / P.negrp;
						ey = ge_e[i2] / P.negrp;

						if(ex < P.xmin || ex > P.xmax 
							|| ey < P.ymin || ey > P.ymax) continue;
		
						isym = da[id1][id2];
						nz   = 0;
						for(i3 = 0; i3 < pEH.len_clean; ++i3)
						{
							if(ge_i[i3] == ge_i[i1] 
								|| ge_i[i3] == ge_i[i2]) continue;

							ez[nz++] = ge_e[i3] / 3;
						}
						if(nz != 0) T->Fill();
					} //== for(i2 = 0;
				} //== for(i1 = 0;

				//== Check end of event
				while(!(pData[pos] == 0xffff)
					&& pos < RealDataLength) pos++;
				if(pos >= RealDataLength) break;

				//== Control Print 
			
				if(++iprint == 1000000)
				{
					iprint = 0;
					printf("\rSIZE = %12ld   EVENTS = %10d   BAD EVENTS %10d"
					//,FileRead,events,badevents);
						,F[ifile].ksize - FileRead,events,badevents);
					fflush(stdout);
					//sleep(1);
				} //== if(++iprint
			} //== while(pos
		} //== while(fread

		printf("\nSIZE = %12ld   EVENTS = %10d   BAD EVENTS %10d\n"
			,F[ifile].ksize - FileRead,events,badevents);
		fflush(stdout);
		fclose(ifile_data);

		//== end of file reading
		//== check is it necessary to continue

		ifile_current = fopen("gs_current.dat","wt");
		fprintf(ifile_current,"%d",ifile);
		fclose(ifile_current);

	} //== for(ifile=
	T->AutoSave();
	FF->Close();
	printf("End of reding\n");
} //-------------------------------------------------------------------

int ReadParms()
{
	//=================================================================
	//== Read all parameters
	//=================================================================

	int k,i1,i2;
	ifstream fin;
	fin.open("gs.inp");
	fin >> P.flistname;
	fin >> P.frootname;
	fin >> P.frootdt;
	fin >> P.fsavename;

	k = P.Read_List(fin, eout);
	fin.close();
	if(k < 0) return k;

	if(P.ntgrp == 0)
	{
		if(strstr(P.flistname,"aug")) P.ntgrp = 1;
		else						  P.ntgrp = 2;
	}


	FILE *finp = fopen(P.flistname,"rt");
	if(!finp)
	{
		printf("FILE: %s WAS NOT OPEN\n",P.flistname);
		return -1;
	}
	nfiles = 0;
	while(F[nfiles].Read(finp) != EOF) ++nfiles;
	SearchTape();

	if(P.k2file == -1 || P.k2file > nfiles) P.k2file = nfiles;
	fclose(finp);

	if(P.knew == 0)
	{
		FILE *finp2 = fopen("gs_current.dat","rt");
		if(!finp2) return nfiles;

		fscanf(finp2,"%d",&P.k1file);
		fclose(finp2);
		P.k1file++;
	}

	TFile F(P.frootname);
	TH2F *d1 = (TH2F*)F.Get(P.frootdt);
	TH2S *d2 = (TH2S*)F.Get("da64");
	if(!d1) return -2;
	if(!d2) return -3;
	for(i1 = 1; i1 < 111; ++i1)
	{
		for(i2 = 1; i2 < 111; ++i2)
		{
			dt[i1-1][i2-1] = (float)d1->GetBinContent(i1,i2);
			dt[i2-1][i1-1] = (float)d1->GetBinContent(i2,i1);
			da[i1-1][i2-1] = (u_short)d2->GetBinContent(i1,i2);
			da[i2-1][i1-1] = (u_short)d2->GetBinContent(i1,i2);
		}
	}
	F.Close();
	return nfiles;
} //-------------------------------------------------------------------


void SearchTape()
{
	int i,j,k;

	if(P.k1tape > 0)
	{
		P.k1file = 1000;
		for(i = 0; i < nfiles; ++i)
		{
			if(F[i].ktape == P.k1tape)
			{
				P.k1file = i;
				break;
			}
		}
		P.k2file = P.k1file + 1;
		if(P.k2tape <= 0)
		{
			for(j = i; j < nfiles; ++j)
			{
				if(F[j].ktape != P.k1tape)
				{
					P.k2file = j;
					break;
				} //== end if
			} //== end for
		} //== end if
		else
		{
			for(j = i; j < nfiles; ++j)
			{
				if(F[j].ktape == P.k2tape)
				{
					P.k2file = j+1;
					for(k = j+1; k < nfiles; ++k)
					{
						if(F[k].ktape != P.k2tape)
						{
							P.k2file = k;
							break;
						} //== end if
					} //== end for
					break;
				} //== end if
			} //== end for
		} //== end else
	}
} //-------------------------------------------------------------------


int SearchEndOfEvent(int ipos)
{
	int pos = ipos;
	while(!(pData[pos] == 0xffff)
		&& pos < RealDataLength) pos++;
	if(pos >= RealDataLength) return -1;
	return pos;
} //-------------------------------------------------------------------

int NextEvent(int ipos)
{
	//===============================================================
	//== Search first word of event
	//===============================================================

	int pos = ipos;
    int len_words;
    int cpos;

	for(;;)
    {
        while((pData[pos] & 0xc000) != 0x8000
            && pos < RealDataLength) pos++;

		//== Check we have enough words for event header
		if(pos + 7 > RealDataLength) return -1;

		//== Check event length > 0
        len_words = pData[pos] & 0x0fff;
		if(len_words == 0) 
		{
			pos++;
			continue;
		}
		
		//== Check: last event word = 0xffff and last word < buffer length 
        cpos = pos + len_words - 1;
        if(cpos < RealDataLength)
        {
            if(pData[cpos] == 0xffff)	break;
            else						pos++;
        }
        else
        {
            return -1;
        }

    }

	pos = ReadEventHeader(pos);

	if(pos + DataLen*pEH.len_clean > RealDataLength) return -1;
	return pos;
} //-------------------------------------------------------------------


int ReadEventHeader(int ipos)
{
	//===============================================================
	//== Read one event
	//===============================================================

	int pos = ipos;

    pEH.len			= pData[pos] & 0x0fff;
    pEH.len_clean	= pData[++pos] & 0x00ff;
    pEH.len_dirty	= (pData[++pos] & 0xff00) >> 8;
    pEH.len_bgo		= pData[pos] & 0x00ff;
    pEH.ttH			= pData[++pos];
    pEH.ttM			= pData[++pos];
    pEH.ttL			= pData[++pos];
    pEH.tac1		= pData[++pos] & 0x0fff;
    pEH.tac2		= pData[++pos];
    pEH.sumge		= pData[++pos];
    pEH.sumbgo		= pData[++pos];
	
	return pos;
} //-------------------------------------------------------------------

int ReadGamma(int ipos)
{
    //=======================================================================
    //== Read Gamma data
    //=======================================================================

	int i;
	int pos = ipos;
	u_short ge_id;

	//== loop to get all the good germanium energies
	for (i=0; i < pEH.len_clean; ++i)
	{
		
		//== check germanium id
		ge_id = pData[++pos] & 0x00ff;
		if (ge_id > 110) ge_id = 0; 

		ge_i[i] = ge_id;
           
		//== get germanium energy
		ge_e[i] = pData[++pos] & 0x3fff;

		//== skip one word
		pos++;
		//== get the germanium tac 
		if(bitset[4] || bitset[5])
			ge_t[i] = pData[++pos] & 0x1fff;
		else
			ge_t[i] = 0;

		if(bitset[5]) pos+=2;
		if(bitset[6]) pos+=2;
	}

	return pos;

} //-------------------------------------------------------------------

int CheckGammaFired(int ipos)
{
	//===============================================================
	//== Check number of fired detectors
	//===============================================================

	int iret = 0;
	if (pEH.len_clean > 110) 
	{
		/*
		printf("Gemult = %d RECORD = %d POS = %d\n"
			,pEH.len_clean, records, pos0);
		*/
		badevents++;
		iret = -1;
	}

	if (pEH.len_dirty > 110) 
	{
		/*
		printf("Gemult_dirty = %d RECORD = %d POS = %d\n"
			,pEH.len_dirty,records,pos0);
		*/
		badevents++;
		iret = -2;
	}

	if (pEH.len_bgo > 110) 
	{
		/*
		printf("bgomult = %d RECORD = %d POS = %d\n"
			,pEH.len_bgo,records,pos0);
		*/
		badevents++;
		iret = -3;
	}

#ifdef TEST1
	if(iret < 0)
	{
		int i;
		int n = pos+10;
		if(n >= RealDataLength) n = RealDataLength -1;
		printf("RECORD %d POS %d POS0 %d DataLength %d\n"
			,records,pos,pos0,RealDataLength);
		for(i = pos-10; i < n; ++i)
			printf("%10d %6.4x \n",i,pData[i]);
		//== scanf("%d",&n);
	}
#endif // TEST1
	return iret;
} //-------------------------------------------------------------------

int CheckEH(size_t size)
{
	//=================================================================
	//== Check buffer header
	//=================================================================
	if(events>2325540&&events<2325560) printf("enter here \n");
	if (size != 16384) 
	{
		printf("+++ ERROR OF BLOCK SIZE = %lu bytes, MUST BE 16384\n",size);
		return -1;
	}

	if (pBH.RecordType != 3) 
	{
		printf("+++ ERROR OF RECORD TYPE = %hu, MUST BE 3\n", pBH.RecordType);
		return -2;
	}
	if (pBH.RecordLength != 16384) 
	{
		printf("+++ ERROR OF RECORD LENGTH = %hu, MUST BE 16384\n"
			,pBH.RecordLength);
		return -3;
	}
	if (pBH.RecordVer != 0) 
	{
		printf("+++ ERROR OF RECORD VERSION = %hu, MUST BE 0\n",pBH.RecordVer);
		return -4;
	}
	if (pBH.HeaderBytes != 22) 
	{
		printf("+++ ERROR OF HEADER LENGTH = %hu, MUST BE 22\n",pBH.HeaderBytes);
		return -5;
	}
	if(pBH.DataLength>EB_SIZE){
		printf("+++ Error of data length \n");
		return -6;
	}

	//== extract data structure information from
	//== the buffer header
	for(int i = 0; i < 16; ++i)
		if((pBH.ModeFlags & bitcode[i]) == bitcode[i])	bitset[i] = 1;
		else											bitset[i] = 0;

    DataLen = 3;

    //==  ge_time
    if (bitset[4] || bitset[5]) DataLen += 1;

    //== ge_trap and ge_low
    if (bitset[5]) DataLen += 2;

    //== bgo_time and bgo_low
    if (bitset[6]) DataLen += 2;
 //               if(events>2325540&&events<2325560){
   //                     printf("event is %10d \n", events);
//	printf("rdlen %u \n",RealDataLength);}
//	if(events>2325540&&events<2325560)
//		printf("rddlen %u \n", pBH.DataLength);
	RealDataLength = pBH.DataLength+1;
  //              if(events>2325540&&events<2325560){
//			printf("after rd pbh \n");
  //                      printf("eventsss is %u \n", pData[RealDataLength]);}
        if(events>2325540&&events<2325560) printf("enter here1 \n");

	while(pData[RealDataLength] != 0xffff){
		RealDataLength--;
//		if(events>2325540&&events<2325560)
//			printf("test len is %u \n", RealDataLength);
	}
	return 0;
} //-------------------------------------------------------------------


int ReadFH(FILE *ifile)
{
	//=================================================================
	//== Read and check file header
	//=================================================================
	
	size_t size = fread(&pFH,1,90,ifile);
	if(size != 90)
	{
#ifdef TEST1
		printf("+++ ERROR OF FH: SIZE OF FILE HEADER = %d, MUST BE 90\n",size);
#endif // TEST1
		return -1;
	}

	if(iSwap)
	{
		Swap((char*)&pFH.RecordType,2);
		Swap((char*)&pFH.RecordLength,2);
		Swap((char*)&pFH.RecordVer,2);
		Swap((char*)&pFH.RunNumber,2);
		Swap((char*)&pFH.FileNumber,2);
	}


	if (pFH.RecordType != 2) 
	{
#ifdef TEST1
		printf("+++ ERROR OF FH: RECORD TYPE = %d, MUST BE 2\n"
			,pFH.RecordType);
#endif // TEST1
		return -2;
	}

	if (pFH.RecordLength != 90) 
	{
#ifdef TEST1
		printf("+++ ERROR OF FH: RECORD LENGTH = %d, MUST BE 90\n"
			,pFH.RecordLength);
#endif // TEST1
		return -3;
	}

	if (pFH.RecordVer != 1) 
	{
#ifdef TEST1
		printf("+++ ERROR OF FH: RECORD VERSION = %d, MUST BE 1\n"
			,pFH.RecordVer);
#endif // TEST1
		return -4;
	}

	return 0;
} //-------------------------------------------------------------------

void Print1FH()
{
	printf("Run number: %d, title: %s%s\n"
		,pFH.RunNumber,pFH.run_title1,pFH.run_title2);
} //-------------------------------------------------------------------



int ReadTH(FILE *ifile)
{
	//=================================================================
	//== Read and check tape header
	//=================================================================

	size_t size = fread(&pTH,1,112,ifile);
	if(size != 112)
	{
#ifdef TEST1
		printf("+++ ERROR OF TH: SIZE OF TAPE HEADER = %d, MUST BE 112\n",size);
#endif // TEST1
		return -1;
	}
	if(iSwap)
	{
		Swap((char*)&pTH.RecordType,2);
		Swap((char*)&pTH.RecordLength,2);
		Swap((char*)&pTH.RecordVer,2);
		Swap((char*)&pTH.TapeNum,2);
		Swap((char*)&pTH.TapeUnit,2);
	}

	if (pTH.RecordType != 1) 
	{
#ifdef TEST1
		printf("+++ ERROR OF TH: RECORD TYPE = %d, MUST BE 1\n",
			pTH.RecordType);
#endif // TEST1
		return -2;
	}

	if (pTH.RecordLength != 112) 
	{
#ifdef TEST1
		printf("+++ ERROR OF TH: RECORD LENGTH = %d, MUST BE 112\n",
			pTH.RecordLength);
#endif // TEST1
		return -3;
	}

	if (pTH.RecordVer != 1) 
	{
#ifdef TEST1
		printf("+++ ERROR OF TH: RECORD VERSION = %d, MUST BE 1\n",
			pTH.RecordVer);
#endif // TEST1
		return -4;
	}

	return 0;
} //-------------------------------------------------------------------

void Print1TH()
{
	printf("Tape Time and Date: %s %s\n"
		,pTH.time, pTH.date); 
} //-------------------------------------------------------------------


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
} //-------------------------------------------------------------------

