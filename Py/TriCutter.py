import math
import os
from os import path
from bitstring import Bits, BitArray
from ROOT import TCanvas, TFile, TH1, TH3, TH3D, TH3S
from ROOT import gROOT

offset = 0.5

filename = raw_input('Input the file name: ')
filename = str(filename)
if(path.isfile(filename)):
	example = TFile( filename )
	example.ls()
	objname = raw_input('Input the histogram name: ')
	objname = str(objname)
	print(objname)
	histo = example.FindObjectAny("objname")
	if(histo == None):
		print('No such file.')
	elif(histo.InheritsFrom("TH3")):
		xco = raw_input('Input the x channel: ')
		xco = float(xco)
		yco = raw_input('Input the y channel: ')
		xco = float(yco)
		result = histo.ProjectionZ(xco - offset, xco + offset, yco - offset, yco + offset)
		result.Draw()
		quit = raw_input('Press enter to quit.')
	else:
		print('Object not a 3-D histogram.')
	
else:
	print('File not existent. Quitting.')


