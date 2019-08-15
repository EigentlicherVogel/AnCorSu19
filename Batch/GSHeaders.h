#pragma once
typedef unsigned short u_short;
typedef unsigned long  u_long;
struct tape_header
{
	u_short RecordType;      // tape header type= 1 
	u_short RecordLength;    // number of bytes in this record  
	u_short RecordVer;       // record version or subtype       
	char    ByteOrder[4];    // to determine byte ordering      
	char    exp_title1[40];  // first part of exp title         
	char    exp_title2[40];  // 2nd  part of exp title          
	char    time[9];         // hh:mm:ss                        
	char    date[9];         // yy/mm/dd                        
	u_short TapeNum;         // tape number in this experiment  
	u_short  TapeUnit;       // specifies which unit wrote this tape 
};

struct file_header
{
	u_short RecordType;      // file header type= 2             
	u_short RecordLength;    // number of bytes in this record  
	u_short RecordVer;       // record version or subtype       
	u_short RunNumber;       // run number in this experiment   
	u_short FileNumber;      // file number in this experiment  
	char    run_title1[40];  // first part of run title         
	char    run_title2[40];  // 2nd  part of run title          
};

#define EB_SIZE  (8192-11)  // number of data words in event buffer 
struct buffer_header
{
	u_short RecordType;      // 0 event data type= 3              
	u_short RecordLength;    // 1 number of bytes in this record 
	u_short RecordVer;       // 2 record version or subtype       
	u_short HeaderBytes;     // 3 number of bytes in header       
	u_short EffNumber;       // 4 eff processor number            
	u_short StreamID;        // 5 event stream ID                 
	u_short EffSequence;     // 6 eff sequence number             
	u_short ModeFlags;       // 7 event format flags              
	u_short DataLength;      // 8 number of i*2 data words        
	u_short ChecksumType;    // 9 type of checksum                
	u_short Checksum;        // 10 checksum value               
};

struct event_header
{
	u_short len;		//== total length of event
	u_short len_clean;	//== n of clean ge
	char len_dirty;  //== n of dirty ge
	char len_bgo;	//== n of BGO
	u_short ttH;        //== usec-time high bits
	u_short ttM;        //== usec-time medium bits
	u_short ttL;        //== usec-time low bits
	u_short tac1;       //== tac1
	u_short tac2;       //== tac2;
	u_short sumge;      //== ge summed energy
	u_short sumbgo;     //== bgo summed energy
};

struct exdata_05
{
	u_short leading;
	u_short reading[16];
};
typedef struct{
	u_short channels[16];
}vsn_data;
 
