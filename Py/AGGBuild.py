import struct
import binascii
import math
import os
from bitstring import Bits, BitArray
from ROOT import TCanvas, TFile, TF1, TF2, TH1S, TH2S, TH3S
from ROOT import gROOT

tape_header = struct.Struct('>HHH 4b 40s40s 9s9s HH')
file_header = struct.Struct('>HHHHH 40s40s')
buffer_header = struct.Struct('>HHHHHHHHHHH')
event_header = struct.Struct('>bbbbbbHHHHHHH')

log_file = open('log.txt', 'w+')
data_file = open('file1.dat','rb')

def checkTH(th, lf):
	if(th[0]==1 and th[1]==112 and th[2]==1):
		lf.write('============\n')
		lf.write('New Tape Started, time and date:\n')
		lf.write(th[9])
		lf.write(th[10])
		lf.write('\n')
		return 1
	else:
		lf.write('Tape header check error!\n')
		return 0


def checkFH(th, lf):
	if(th[0]==2 and th[1]==90):
		lf.write('----------\n')
		lf.write('New File Started.\n')
		return 1
	else:
		lf.write('File header check error!\n')
		return 0

def checkBH(bh, lf):
	if bh[0]==3 and bh[1]==16384 and bh[3]==22 and bh[10]==48059:
		lf.write('New buffer started.\n')
		bitflags = BitArray(int = bh[7], length = 16)
		bitflags.reverse();
		data_format = '>HHH'
		if bitflags[4] or bitflags[5]:
			data_format += 'H'
		if bitflags[5]:
			data_format += 'HH'
		if bitflags[6]: 
			data_format += 'HH'
		return (data_format)
	else:
		lf.write('Buffer header error at: ')
		lf.write(str(data_file.tell()) + '\n')
		return ('0')

theader = tape_header.unpack(data_file.read(tape_header.size))
checkTH(theader, log_file)

fheader = file_header.unpack(data_file.read(file_header.size))
checkFH(fheader, log_file)
	
if (data_file.read(1)):

	data_file.seek(-1,1)
	bheader = buffer_header.unpack(data_file.read(buffer_header.size))
	buffer_format = checkBH(bheader, log_file)
	if(buffer_format == '0'):
		#break
		print('O')
		
	buffer_rest = bheader[8] * 2
	gamma_single = struct.Struct(buffer_format)
	read_counter = 0
	

	
	while(read_counter < buffer_rest):
		eheader = event_header.unpack(data_file.read(event_header.size))
		read_counter += event_header.size
		
		if(eheader[0] == -128):
			gamma_ct = eheader[3] + eheader[4] #+ eheader[5]
			ite = 0;
			
			event_len = eheader[1]
			ext_len = event_len - (gamma_ct * 4) - 10
			
			g_id = [0] * gamma_ct
			g_e = [0] * gamma_ct
			g_tac = [0] * gamma_ct
			
			#print(buffer_format)
			
			while(gamma_ct > 0):
				singlegamma = gamma_single.unpack(data_file.read(gamma_single.size))
				g_id[ite] = singlegamma[0] & 0x00ff
				g_e[ite] = singlegamma[1] & 0x3fff
				g_tac[ite] = singlegamma[3] & 0x1fff
				gamma_ct -= 1
				ite += 1
				read_counter += gamma_single.size
				
			#print(g_id)
			#print(g_e)
			#print(g_tac)		
			
			exth = binascii.hexlify(data_file.read(4))
			if exth[0:4] == 'ff00':
				ext_x = data_file.read(ext_len * 2 - 4)
				ext_form = '>'
				for i in range (ext_len - 2):
					ext_form += 'H'
				extd = struct.unpack(ext_form, ext_x)
				read_counter += ext_len * 2 - 4
				
				cur_vsn = 0
				chan_loc = 0
				in_chan = 0
				for a in extd: 
					if(a == 0xffff and cur_vsn >= 13):
						#print(buffer_rest)
						continue
					if(a == 0x8b01 and cur_vsn == 0):
						in_chan = 0
						continue
					if(a == 0x8b0a and cur_vsn == 7):
						in_chan = 0
						continue
					if(in_chan == 0):
						if(a == 0x0000):
							cur_vsn += 1
							continue
						else:
							cur_vsn += 1 
							chan_loc = bin(a).count("1")
							in_chan = 1;
							continue
					if(in_chan == 1):
						chan_flag = (a & 0xf000) >> 12
						chan_data = (a & 0x0fff)
						#print(chan_flag)
						#print(chan_data)
						chan_loc = chan_loc - 1
						if(chan_loc == 0):
							in_chan = 0
			else:
				log_file.write('Event header error!\n')
				
		next_test = binascii.hexlify(data_file.read(1))
		if(next_test != '80'):
			log_file.write(str(read_counter) + '\n')
			log_file.write(str(data_file.tell()))
			log_file.write(' - location: next buffer header error. \n')		
			while(read_counter < 16384):
				read_counter += 1
				data_file.seek(1,1)
		else:
			data_file.seek(-1,1)
		
		if(read_counter > 16384):
			log_file.write(str(read_counter) + '\n')
			log_file.write(str(data_file.tell()))
			log_file.write(' - location: file pointer out of buffer bounds. \n')		
			while(read_counter > 16384):
				read_counter = read_counter - 1
				data_file.seek(-1,1)
			
	
os.chmod('log.txt', 0o777)
log_file.close()
data_file.close()
