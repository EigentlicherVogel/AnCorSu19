//=====================================================================
//==  To read data of free format
//==
//==  Andrei Daniel
//==  daniel@suntimpx.jinr.ru
//==  FILE:    NlistA.C
//==  Version:	29/04/1998
//==			27/09/2005
//=====================================================================

#include "NlistA.h"
#define min(a,b) ((a) < (b) ? (a) : (b))

extern ofstream eout;

int NblockA::Add(const char *w_add, double *xval, int z_add,
                int klimit_add, Free x_min, Free x_max) 
{
	w   = new char[strlen(w_add) +1];
	if(w != 0) 
	{
		strcpy(w,w_add);
		t      = Idouble;
		z      = z_add;
		klimit = klimit_add;
		dval = xval;
		y_min.d = x_min.d; 
		y_max.d = x_max.d; 
		return 0;
	}
   
	size_t lll = strlen(w_add);
	eout << " NblockA error of memory allocation: N = " << lll << endl;
	return -1;
} //-------------------------------------------------------------------

int NblockA::Add(const char *w_add, float *xval, int z_add,
                int klimit_add, Free x_min, Free x_max) 
{
	w   = new char[strlen(w_add) +1];
	if(w != 0) 
	{
		strcpy(w,w_add);
		t      = Ifloat;
		z      = z_add;
		klimit = klimit_add;
		rval = xval;
		y_min.d = x_min.r; 
		y_max.d = x_max.r; 
		return 0;
	}
   
	size_t lll = strlen(w_add);
	eout << " NblockA error of memory allocation: N = " << lll << endl;
	return -1;
} //-------------------------------------------------------------------

int NblockA::Add(const char *w_add, int *xval, int z_add,
                int klimit_add, Free x_min, Free x_max) 
{
	w   = new char[strlen(w_add) +1];
	if(w != 0) 
	{
		strcpy(w,w_add);
		t      = Iint;
		z      = z_add;
		klimit = klimit_add;
		ival = xval;
		y_min.d = x_min.i; 
		y_max.d = x_max.i; 
		return 0;
	}
   
	size_t lll = strlen(w_add);
	eout << " NblockA error of memory allocation: N = " << lll << endl;
	return -1;
} //-------------------------------------------------------------------

int NblockA::Add(const char *w_add, long *xval, int z_add,
                int klimit_add, Free x_min, Free x_max) 
{
	w   = new char[strlen(w_add) +1];
	if(w != 0) 
	{
		strcpy(w,w_add);
		t      = Ilong;
		z      = z_add;
		klimit = klimit_add;
		lval = xval;
		y_min.d = x_min.l; 
		y_max.d = x_max.l; 
		return 0;
	}
   
	size_t lll = strlen(w_add);
	eout << " NblockA error of memory allocation: N = " << lll << endl;
	return -1;
} //-------------------------------------------------------------------


NlistA::NlistA(int Max_inp, const char *Bname_inp) 
{
	Set(Max_inp, Bname_inp);
} //-------------------------------------------------------------------

NlistA::NlistA()
{
	Maxname = 0;
	Bname   = 0;
	x       = 0;
} //-------------------------------------------------------------------

NlistA::~NlistA()
{
	if(Bname) delete Bname;
	if(x)	  delete x;
} //-------------------------------------------------------------------

void NlistA::Set(int max_inp, const char* Bname_inp)
{
	Maxname = max_inp;
	Bname   = new char[strlen(Bname_inp) + 1];
	strcpy(Bname,Bname_inp);
	last = 0;
	x    =   new NblockA [max_inp];
} //-------------------------------------------------------------------



int NlistA::GetName(char* name, int& nvalue, int rec)
{
	//==================================================================
	//== return name of record
	//==================================================================

	if(rec < 0 || rec >= last) return -1;
	strcpy(name,x[rec].w);
	nvalue = x[rec].z;
	return x[rec].t;
} //--------------------------------------------------------------------

int NlistA::GetValueI(int rec, int i)
{
	//==================================================================
	//== Return integer value from record
	//==================================================================

	if(rec < 0 || rec >= last) return -1;
	return x[rec].ival[i];
}	//------------------------------------------------------------------

double NlistA::GetValueD(int rec, int i)
{
	//==================================================================
	//== Return double value from record
	//==================================================================

	if(rec < 0 || rec >= last) return -1.;
	return x[rec].dval[i];
}	//------------------------------------------------------------------

int NlistA::Nadd(const char *w_add, double* y, int n) 
{
   if (last <= Maxname) 
   {
      x_min.d = 0.;
      x_max.d = 0.;
      x[last].Add(w_add, y, n, 0, x_min, x_max);
      return last++;
   } 
   else 
      return -1;
}

//---------------------------------------------------------------------- 
int NlistA::Nadd(const char *w_add, double* y, 
                int key, double y_min, double y_max, int n) 
{
   if (last <= Maxname) 
   {
      x_min.d = y_min;
      x_max.d = y_max;
      x[last].Add(w_add, y, n, key, x_min, x_max );
      return last++;
   } 
   else 
      return -1;
}

//---------------------------------------------------------------------- 
int NlistA::Nadd(const char *w_add, float* y, int n) 
{
   if (last <= Maxname) 
   {
      x_min.r = 0.;
      x_max.r = 0.;
      x[last].Add(w_add, y, n, 0, x_min, x_max);
      return last++;
   } 
   else
      return -1;
}

//---------------------------------------------------------------------- 
int NlistA::Nadd(const char *w_add, float* y, 
                int key, float y_min, float y_max, int n) 
{
   if (last <= Maxname) 
   {
      x_min.r = y_min;
      x_max.r = y_max;
      x[last].Add(w_add, y, n, key, x_min, x_max );
      return last++;
   } 
   else 
      return -1;
}

//---------------------------------------------------------------------- 
int NlistA::Nadd(const char *w_add, int* y, int n) 
{
   if (last <= Maxname) 
   {
      x_min.i = 0;
      x_max.i = 0;
      x[last].Add(w_add, y, n, 0, x_min, x_max);
      return last++;
   } 
   else
      return -1;
}

//---------------------------------------------------------------------- 
int NlistA::Nadd(const char *w_add, int* y, 
                int key, int y_min, int y_max, int n) 
{
   if (last <= Maxname) 
   {
      x_min.i = y_min;
      x_max.i = y_max;
      x[last].Add(w_add, y, n, key, x_min, x_max );
      return last++;
   } 
   else 
      return -1;
}

//---------------------------------------------------------------------- 
int NlistA::Nadd(const char *w_add, long* y, int n) 
{
   if (last <= Maxname) 
   {
      x_min.l = 0;
      x_max.l = 0;
      x[last].Add(w_add, y, n, 0, x_min, x_max);
      return last++;
   } 
   else
      return -1;
}

//---------------------------------------------------------------------- 
int NlistA::Nadd(const char *w_add, long* y, 
                int key, long y_min, long y_max, int n) 
{
   if (last <= Maxname) 
   {
      x_min.l = y_min;
      x_max.l = y_max;
      x[last].Add(w_add, y, n, key, x_min, x_max );
      return last++;
   } 
   else 
      return -1;
}

//---------------------------------------------------------------------- 
int NlistA::Nget(istream& fname) 
{

	int i,j,n=0,k=0;	
	char wr[10];
	int kout=0;
	int    *I_ptr;
	long   *L_ptr;
	float  *F_ptr;
	double *D_ptr;

	do
	{
		fname >> wr;
		if(fname.eof())
		{
			eout << "NgetA >> Block data " << Bname
				 << " was not found" << endl;
			return -3;
		}
	} while(strncmp(wr,Bname,strlen(Bname)) != 0);

	for(;;)
	{
		fname >> wr;
		if(fname.eof()) break;

		if(strncmp(wr,"/",1) == 0)
		{
			if(!kout) return n;
			else
			{
				eout << "Part of input data are out of limits " << endl;
				return -kout;
			} //== end else
		} //== end if

		for(i = 0; i < last; ++i)
		{
			//if(strncmp(wr,x[i].w,strlen(x[i].w)) != 0) continue;
			if(strcmp(wr,x[i].w) != 0) continue;


			n++;
			k++;
			//xx.ptr = x[i].ptr;

			switch (x[i].t)
			{
			case(Idouble):
				D_ptr = x[i].dval;
				for(j = 0; j < x[i].z; j++,D_ptr++)
				{
					fname >> *D_ptr;
					if(( ( x[i].klimit == 1) || (x[i].klimit == 3) )
						&&   *D_ptr < x[i].y_min.d)
					{
						eout << "NlistA >> " << setw(12) << wr
							 << " ... "      << setw(12) << *D_ptr
							 << " out of low  limit "
							 << setw(12) << x[i].y_min.d << endl;
						kout++;
					} //== end if
					else if(( (x[i].klimit == 2) || (x[i].klimit == 3) )
						&& *D_ptr > x[i].y_max.d)
					{
						eout << "NlistA >> " << setw(12) << wr
							 << " ... "      << setw(12) << *D_ptr
							 << " out of high limit "
							 << setw(12) << x[i].y_max.d << endl;
						kout++;
					}
				} //== end for(j = 0; j < x[i].z
			break;

			case(Ifloat):
				F_ptr = x[i].rval;
				for(j = 0; j < x[i].z; j++,F_ptr++)
				{
					fname >> *F_ptr;
					if(( (x[i].klimit == 1) || (x[i].klimit == 3))
						&&  *F_ptr < x[i].y_min.r)
					{
						eout << "NlistA >> " << setw(12) << wr
							 << " ... "      << setw(12) << *F_ptr
                             << " out of low  limit "
                             << setw(12) << x[i].y_min.r << endl;
                        kout++;
					} //== end if
					else if(( (x[i].klimit == 2) || (x[i].klimit == 3) )
						&& *F_ptr > x[i].y_max.r)
					{
						eout << "NlistA >> " << setw(12) << wr
							 << " ... "      << setw(12) << *F_ptr
							 << " out of high limit "
							 << setw(12) << x[i].y_max.r << endl;
						kout++;
					} //== end else if
				} //== end for(j = 0; j < x[i].z
			break;

			case(Iint):
				I_ptr = x[i].ival;
				for(j = 0; j < x[i].z; j++,I_ptr++)
				{
					fname >> *I_ptr;
					if(( (x[i].klimit == 1) || ( x[i].klimit == 3))
						&& *I_ptr < x[i].y_min.i)
					{
						eout << "NlistA >> " << setw(12) << wr
							 << " ... "      << setw(12) << *I_ptr
							 << " out of low  limit "
							 << setw(12) << x[i].y_min.i << endl;                         
						printf("NlistA::%12s ... ",wr);
						kout++;
					} //== end if
					else if(( (x[i].klimit == 2) || (x[i].klimit == 3))
						&& *I_ptr > x[i].y_max.i)
					{
						eout << "NlistA >> " << setw(12) << wr
							 << " ... "      << setw(12) << *I_ptr
							 << " out of high limit "
							 << setw(12) << x[i].y_max.i << endl;                         
						printf("NlistA::%12s ... ",wr);
                        kout++;
					}
				} //== end for(j = 0;
			break;

			case(Ilong):
				L_ptr = x[i].lval;
				for(j = 0; j < x[i].z; j++,L_ptr++)
				{
					fname >> *L_ptr;
					if(( (x[i].klimit == 1) || (x[i].klimit == 3))
						&& *L_ptr < x[i].y_min.l)
					{
						eout << "NlistA >> " << setw(12) << wr
							 << " ... "      << setw(12) << *L_ptr
							 << " out of low  limit "
							 << setw(12) << x[i].y_min.l << endl;
						kout++;
					} //== end if
					else if(( (x[i].klimit == 2) || (x[i].klimit == 3))
						&& *L_ptr > x[i].y_max.l)
					{
						eout << "NlistA >> " << setw(12) << wr
							 << " ... "      << setw(12) << *L_ptr 
							 << " out of high limit " 
							 << setw(12) << x[i].y_max.l << endl;                      
						kout++;                     
					} //== end else if                                                                   
				} //== end for(j = 0;
			break;
			}  //-- END SWITCH
			break;

		} //== end for(i = 0;

		if(k == 0) 
		{
			eout << "NlistA >> The word: " << wr
				 << " was not found in  the List" << endl;
			for(j = 0; j < last; ++j)
				eout << setw(4) << j << setw(15) << x[j].w << endl;
			return -2;
		} 
		else
			k = 0;
	} //== end for(;;)
	return -1;
} //-------------------------------------------------------------------

void NlistA::Nput() 
{

   int i,j,k;
   double *D_ptr;
   float  *F_ptr;
   long   *L_ptr;
   int    *I_ptr;

   eout << "Block data " << Bname << endl;
   for(i = 0; i < last; ++i)
   {
      eout << x[i].w;

      switch (x[i].t)
      {

          case(Idouble):
             D_ptr = x[i].dval;
             for(k = 0; k < x[i].z; k += 5)
             {
                for(j = k; j < min(k+5,x[i].z); j++,D_ptr++)
                   eout << setw(13) << *D_ptr;

                if(k+5 < x[i].z) eout << endl << setw(13) << " ";
             }
             eout << endl;
             break;

          case(Ifloat):
             F_ptr = x[i].rval;
             for(k = 0; k < x[i].z; k += 5)
             {
                for(j = k; j < min(k+5,x[i].z); j++,F_ptr++)
                   eout << setw(13) << *F_ptr;

                if(k+5 < x[i].z) eout << endl << setw(13) << " ";
             }
             eout << endl;
             break;

          case(Iint):
             I_ptr = x[i].ival;
             for(k = 0; k < x[i].z; k += 5)
             {
                for(j = k; j < min(k+5,x[i].z); j++,I_ptr++)
                   eout << setw(13) << *I_ptr;
                if(k+5 < x[i].z) eout << endl << setw(13) << " ";
             }
             eout << endl;
             break;

          case(Ilong):
             L_ptr = x[i].lval;
             for(k = 0; k < x[i].z; k += 5)
             {
                for(j = k; j < min(k+5,x[i].z); j++,L_ptr++)
                   eout << setw(13) << *L_ptr;

                if(k+5 < x[i].z) eout << endl << setw(13) << " ";
             }
             eout << endl;
             break;

      }  //-- END SWITCH
   }  //-- END FOR
} //-------------------------------------------------------------------

void NlistA::Nput(ostream& fout) 
{

   int i,j,k;
   double *D_ptr;
   float  *F_ptr;
   long   *L_ptr;
   int    *I_ptr;

   fout << Bname << endl;
   for(i = 0; i < last; ++i)
   {
      fout << x[i].w;

      switch (x[i].t)
      {

          case(Idouble):
             D_ptr = x[i].dval;
             for(k = 0; k < x[i].z; k += 5)
             {
                for(j = k; j < min(k+5,x[i].z); j++,D_ptr++)
                   fout << setw(13) << *D_ptr;

                if(k+5 < x[i].z) fout << endl << setw(13) << " ";
             }
             fout << endl;
             break;

          case(Ifloat):
             F_ptr = x[i].rval;
             for(k = 0; k < x[i].z; k += 5)
             {
                for(j = k; j < min(k+5,x[i].z); j++,F_ptr++)
                   fout << setw(13) << *F_ptr;

                if(k+5 < x[i].z) fout << endl << setw(13) << " ";
             }
             fout << endl;
             break;

          case(Iint):
             I_ptr = x[i].ival;
             for(k = 0; k < x[i].z; k += 5)
             {
                for(j = k; j < min(k+5,x[i].z); j++,I_ptr++)
                   fout << setw(13) << *I_ptr;
                if(k+5 < x[i].z) fout << endl << setw(13) << " ";
             }
             fout << endl;
             break;

          case(Ilong):
             L_ptr = x[i].lval;
             for(k = 0; k < x[i].z; k += 5)
             {
                for(j = k; j < min(k+5,x[i].z); j++,L_ptr++)
                   fout << setw(13) << *L_ptr;

                if(k+5 < x[i].z) fout << endl << setw(13) << " ";
             }
             fout << endl;
             break;

      }  //-- END SWITCH
   }  //-- END FOR
} //-------------------------------------------------------------------
