
##### Source files and executable ############################################

SRCS 		= main.cpp 

OBJECTS 	= main.o 

EXECNAME 	= main


##### Libraries and paths ####################################################

LIBS            = 
INCDIRS 	= 
LIBDIRS 	= 
 
##### Compiler information ###################################################

CPP		= g++ -std=c++11
CPPFLAGS 	= 

##### Target compilation #####################################################

all:$(EXECNAME)

$(EXECNAME): 	$(OBJECTS)
	$(CPP) $(CPPFLAGS) $(LIBDIRS) $^ $(LIBS) -o $(EXECNAME) 

.cpp.o:	
	$(CPP) $(CPPFLAGS) $(INCDIRS) -c $<

.cc.o:	
	$(CPP) $(CPPFLAGS) $(INCDIRS) -c $<

clean:
	rm -rf *~ *.o *.a $(EXECNAME) 

##############################################################################

