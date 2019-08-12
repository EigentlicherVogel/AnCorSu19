# Makefile for gs_tree
# Read tapes and create tree of selected data

include $(ROOTSYS)/etc/Makefile.arch

#------------------------------------------------------------------------------
TEST		= 
LDFLAGS		+= $(TEST)
CXXFLAGS	+= $(TEST) 
NAME 		= gs_readtapes
TEXTC           = $(wildcard *.cpp)
TEXTH           = $(wildcard *.h)
OBJC            = $(patsubst %cpp,%o,$(TEXTC))

PROGRAMS	= $(NAME)

$(NAME):	$(OBJC)  
	$(LD) $(LDFLAGS)  $(OBJC) $(LIBS)   $(OutPutOpt) $(NAME)

all:	$(PROGRAMS)

$(OBJC): $(TEXTC) $(TEXTH)
	$(CXX) $(CXXFLAGS) -g -c $(TEXTC)


