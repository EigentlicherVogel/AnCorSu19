//======================================================================
//==
//==	Class Nblock
//==
//==	To read data of free format
//==
//==	Nlist:  input: total number of data, name of block data
//==	Nadd:   input: name of data, address of data, number of data
//==	Nget:   input: istream
//==
//==	Andrei Daniel
//==	e-Mail:		daniel@jinr.ru
//==
//==	FILE:		NlistA.h
//==	Version:	29/04/1998
//==				27/09/2005	for Visual Studio NET
//==
//======================================================================


#ifndef NlistA_h
#define NlistA_h
#include <string.h>

#include <Riostream.h>
#include <fstream>
using namespace std;

class NblockA
{
    public:
    enum Itype {Iint=1, Idouble=2, Ifloat=3, Ilong=4} t;
    union Free
    {
        double d;
        float  r;
        int    i;
        long   l;
    };				//== Address of data
	double	*dval;
	float	*rval;
	int		*ival;
	long	*lval;
    Free  y_min;
    Free  y_max;
    char  *w;       //== Name of data
    int   z;        //== Number of input data
    int   klimit;   //== Limits of data
                    //== 0 - no; 1 - low; 2 - high; 3 - both
   public:

   int Add(const char *w_add, double *xval, int z_add,
           int klimit_add, Free x_min, Free x_max);
   int Add(const char *w_add, float  *xval, int z_add,
           int klimit_add, Free x_min, Free x_max);
   int Add(const char *w_add, int    *xval, int z_add,
           int klimit_add, Free x_min, Free x_max);
   int Add(const char *w_add, long   *xval, int z_add,
           int klimit_add, Free x_min, Free x_max);

//   friend Nlist::Nadd;
};

class NlistA : public NblockA
{
	int     last;          // Last record
	int     Maxname;       // Max. number of records
	NblockA *x,xx;
	char    *Bname;        // Name of block of data
	Free    x_min;
	Free    x_max;

public:
	NlistA  (int Max_inp, const char *Bname_inp);
	NlistA();
	~NlistA ();
	void Set(int Max_inp, const char *Bname_inp);
    int Nadd   (const char *w_add, double *y, int n=1);
    int Nadd   (const char *w_add, double *y,
                int  key,    double y_min, double y_max, int n=1);
    int Nadd   (const char *w_add, float  *y,    int    n=1);
    int Nadd   (const char *w_add, float  *y,
               int  key,    float  y_min, float  y_max, int n=1);
    int Nadd   (const char *w_add, int    *y,    int    n=1);
    int Nadd   (const char *w_add, int    *y,
               int  key,    int    y_min, int y_max,    int n=1);
    int Nadd   (const char *w_add, long   *y,    int n=1);
    int Nadd   (const char *w_add, long   *y,
               int  key,    long   y_min, long   y_max, int n=1);
    int Nget   (istream& fname);
    void Nput  ();
	void Nput  (ostream& fout);
    //int  GetNRecords();
    //int  GetRecord(NblockA *record);
	int GetName(char* name, int& nvalue, int rec);
	int GetValueI(int rec, int i=0);
	double GetValueD(int rec, int i=0);
};

#endif  /* NlistA_h  */

//inline NlistA::GetNRecords() { return last;}
