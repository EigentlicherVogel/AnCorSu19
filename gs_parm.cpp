//======================================================================
//==
//==	Class gs_parm
//==
//==	List of common parameters of code gs_dt
//==
//==
//==	FILE:		gs_parm.cpp
//==	Versions: 23.11.2006
//======================================================================

#include "gs_parm.h"
#include "math.h"


void gs_parm::Alloc()
{
	
	kswap   = 1;
	knew    = 1;
	k1file	= 0;
	k2file	= -1;
	k1tape  = 0;
	k2tape  = 0;
	nmatr   = 11;
	imatr   = 0;
	nchan   = 64;
	negrp   = 2;
	ntgrp	= 1;
	tmax    = 100.;

	
	ex0	= 0.;
	ey0	= 0.;


	ndata = 40;
} //--------------------------------------------------------------------

int gs_parm::Read_List(ifstream &fread, ofstream &eout)
{

    int m;
    Alloc();
	a.Set(ndata,"PARM");

	a.Nadd("kswap",		&kswap);
	a.Nadd("knew",		&knew);
	a.Nadd("k1file",	&k1file);
	a.Nadd("k2file",	&k2file);
	a.Nadd("k1tape",	&k1tape);
	a.Nadd("k2tape",	&k2tape);
	a.Nadd("nmatr",	    &nmatr);
	a.Nadd("imatr",	    &imatr);
	a.Nadd("nchan",		&nchan);
	a.Nadd("negrp",		&negrp);
	a.Nadd("ntgrp",		&ntgrp);
	a.Nadd("tmax",		&tmax);

	a.Nadd("ex0",		&ex0);
	a.Nadd("ey0",		&ey0);


	if(fread.is_open())
		m = a.Nget(fread);
	else
		m = -1;


	xmin = floor(3.*ex0)/negrp - nchan/2;
	ymin = floor(3.*ey0)/negrp - nchan/2;
	xmax = xmin + nchan;
	ymax = ymin + nchan;



   return m;
} //--------------------------------------------------------------------

void gs_parm::ReadParms(char* fname)
{
	//==================================================================
	//== Read parameters
	//==================================================================

	ifstream fread;
	fread.open(fname);
	if(fread.is_open())
		a.Nget(fread);
	fread.close();
} //-------------------------------------------------------------------


void gs_parm::SaveParms(ofstream& fout)
{
	a.Nput(fout);
} //--------------------------------------------------------------------

