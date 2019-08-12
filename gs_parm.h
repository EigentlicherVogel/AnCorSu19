//======================================================================
//==
//==	Class gs_Parm
//==
//==	List of common parameters of code gs_matrix
//==
//==	Andrei Daniel
//==	e-Mail:		daniel@jinr.ru
//==
//==	FILE:		gs_parm.h
//==	Version:	
//==	19.10.2006
//======================================================================

#ifndef GS_Parm_h
#define GS_Parm_h

#include <TROOT.h>
#include <Riostream.h>
#include "NlistA.h"

//======================================================================
class gs_parm
{
private:
	int ndata;

	NlistA a;

public:

    int
		 kswap
		,knew
		,k1file
		,k2file
		,k1tape
		,k2tape
		,nmatr
		,imatr
		,nchan
		,negrp
		,ntgrp;

	double tmax; 
	double ex0;
	double ey0;
	unsigned short xmin,xmax,ymin,ymax;

	char flistname[80];
	char frootname[80];
	char frootdt[80];
	char fsavename[80];
	void Alloc();
	int  Read_List(ifstream &fread, ofstream &eout);
	void SaveParms(ofstream& fout);
	void ReadParms(char* fname);
};
#endif
