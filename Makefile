# Modifiqueu aquestes dues linies 
MY_AI = AIAIAI #Nom de la vostre AI
CHROME = chromium-browser #/usr/bin/google-chrome-stable #Comanda del vostre navegador preferit

PWD = $(shell pwd)

# Uncomment the line for your platform to link the Dummy player to the executable
DUMMY_OBJ = AIDummy-Linux32bits.o
#DUMMY_OBJ = AIDummy-Linux64bits.o

# Add here any extra .o player files you want to link to the executable
EXTRA_OBJS =

# Config
OPTIMIZE = 2 # Optimization level (0 to 3)
DEBUG    = 0 # Compile for debugging (0 or 1)
PROFILE  = 0 # Compile for profile (0 or 1)
32BITS   = 0 # Produce 32 bits objects on 64 bits systems (0 or 1)


# Do not edit past this line
############################

# The following two lines will detect all your players (files matching "AI*.cc")

PLAYERS_SRC = $(wildcard AI*.cc)
PLAYERS_OBJ = $(patsubst %.cc, %.o, $(PLAYERS_SRC)) $(EXTRA_OBJS) $(DUMMY_OBJ)

# Flags

ifeq ($(strip $(PROFILE)),1)
	PROFILEFLAGS=-pg
endif
ifeq ($(strip $(DEBUG)),1)
	DEBUGFLAGS=-DDEBUG -g -rdynamic
endif
ifeq ($(strip $(32BITS)),1)
	ARCHFLAGS=-m32 -L/usr/lib32
endif

CXXFLAGS = -std=c++0x -Wall -Wno-unused-variable $(ARCHFLAGS) $(PROFILEFLAGS) $(DEBUGFLAGS) -O$(strip $(OPTIMIZE)) #-Wextra
LDFLAGS  = -std=c++0x -lm $(ARCHFLAGS) $(PROFILEFLAGS) $(DEBUGFLAGS) -O$(strip $(OPTIMIZE))

# Rules

all: Game  

clean:
	mv $(DUMMY_OBJ) $(DUMMY_OBJ).keep
	rm *.o
	mv $(DUMMY_OBJ).keep $(DUMMY_OBJ)

$(DUMMY_OBJ):

Game: Utils.o Board.o Action.o Player.o Registry.o Game.o Main.o $(PLAYERS_OBJ)
	$(CXX) $(LDFLAGS) $^ -o $@

SecGame: Utils.o Board.o Action.o Player.o Registry.o SecGame.o SecMain.o
	$(CXX) $(LDFLAGS) $^ -o $@

%.exe: %.o Utils.o Board.o Action.o Player.o Registry.o SecGame.o SecMain.o
	$(CXX) $(LDFLAGS)  $^ -o $@

obj:
	mkdir obj

Makefile.deps: *.cc
	$(CXX) $(CXXFLAGS) -MM *.cc > Makefile.deps

doxygen:
	doxygen
	cd latex && make
	cp latex/refman.pdf .
launch:
	$(CHROME) --args -allow-file-access-from-files %U "file://$(PWD)/Viewer/viewer.html?start=yes&game=file://$(PWD)/default.res" &

dummyTest:
	./Game $(MY_AI) Dummy Dummy Dummy < default.cnf > default.res 2> out.txt

nullTest:
	./Game $(MY_AI) Null Null Null < default.cnf > default.res 2> out.txt
fullNulli:
	make 
	make nullTest
	make launch
fullDummy:
	make
	make dummyTest
	make launch
resultDummy:
	./Game $(MY_AI) Dummy Dummy Dummy < default.cnf > default.res
jfonsTest:
	make
	./Game $(MY_AI) $(MY_AI) $(MY_AI) $(MY_AI) < default.cnf > default.res 2> out.txt
include Makefile.deps

