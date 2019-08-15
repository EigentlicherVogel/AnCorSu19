include $(ROOTSYS)/etc/Makefile.arch

#------------------------------------------------------------------------------
TEST		= 
LDFLAGS		+= $(TEST)
CXXFLAGS	+= $(TEST) 
NAME 		= ReadAllHist
TEXTC           = ReadDats_Hist.cpp
TEXTH           = GSHeaders.h
OBJC            = $(patsubst %cpp,%o,$(TEXTC))

PROGRAMS	= $(NAME) 

$(NAME):	$(OBJC)  
	$(LD) $(LDFLAGS)  $(OBJC) $(LIBS)   $(OutPutOpt) $(NAME) 

all:	$(PROGRAMS) 
 
$(OBJC): $(TEXTC) $(TEXTH) 
	$(CXX) $(CXXFLAGS) -g -c $(TEXTC) 


clean: 
	rm -f $(binaries) *.o 
