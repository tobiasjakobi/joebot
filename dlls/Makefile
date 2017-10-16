# ---------------------------------------------------------------------------
# Definitions
# ---------------------------------------------------------------------------
include ../Config.mk

# build object paths
ifeq ($(OPT), debug)
 BASE_OBJDIR = $(OBJDIR_DEBUG)
 EXT_OBJDIR = $(OBJDIR_DEBUG)
else
 BASE_OBJDIR = $(OBJDIR_RELEASE)
 EXT_OBJDIR = $(OBJDIR_RELEASE)
endif

BASE_DLLNAME = joebot
ifeq ($(DLLTYPE),std)
 DLLNAME = $(BASE_DLLNAME)
 OBJDIR = $(BASE_OBJDIR)/std
else
 DLLNAME = $(BASE_DLLNAME)_mm
 OBJDIR = $(BASE_OBJDIR)/mm
endif

ifeq ($(OS),Windows_NT)
 OBJDIR := $(OBJDIR)_mingw
 EXT_OBJDIR := $(EXT_OBJDIR)_mingw
endif

# build external source list
include ../NNSim/som/Config.mk
SRC_SOM := $(SRC:%.cpp=../NNSim/som/%.cpp)
OBJ_SOM := $(SRC:%.cpp=../NNSim/som/$(EXT_OBJDIR)/%.o)
include ../NNSim/Config.mk
SRC_NNSIM := $(SRC:%.cpp=../NNSim/%.cpp)
OBJ_NNSIM := $(SRC:%.cpp=../NNSim/$(EXT_OBJDIR)/%.o)

# build source list
include Config.mk
SRC_DLLS := $(SRC:%.cpp=%.cpp)
OBJ_DLLS := $(SRC:%.cpp=$(OBJDIR)/%.o)

ifeq ($(OS),Windows_NT)
 SRC_RES = $(OBJDIR)/$(BASE_DLLNAME).rc
 OBJ_RES = $(OBJDIR)/$(BASE_DLLNAME).res
endif

SRC = $(SRC_DLLS) $(SRC_NNSIM) $(SRC_SOM) $(SRC_RES)
OBJ = $(OBJ_DLLS) $(OBJ_NNSIM) $(OBJ_SOM) $(OBJ_RES)

CC = gcc
RC = windres
CP = cp
RM = rm -fv

# includes
BASECFLAGS = -I$(JOEBOTDIR)/engine -I$(JOEBOTDIR)/common\
 -I$(JOEBOTDIR)/pm_shared -I$(MMDIR)/metamod -I$(JOEBOTDIR)/NNSim/som\
 -I$(JOEBOTDIR)/NNSim -I$(JOEBOTDIR)/dlls

BASECFLAGS += $(EXTRA_CFLAGS)

# compiler flags
ifeq ($(OPT),debug)
 CFLAGS = -D_DEBUG $(BASECFLAGS) -g
else
 CFLAGS = $(BASECFLAGS) -O2 -march=i586 -O2 -ffast-math -funroll-loops\
  -fomit-frame-pointer -fexpensive-optimizations
 ifeq ($(OS),Windows_NT)
  CFLAGS += -falign-loops=2 -falign-jumps=2 -falign-functions=2
 else
  CFLAGS += -malign-loops=2 -malign-jumps=2 -malign-functions=2
 endif
endif

# linker flags
ifeq ($(OS),Windows_NT)
 DLL = $(DLLNAME).dll
 LD = dllwrap
 LDFLAGS = -mwindows --add-stdcall-alias --output-def $(DLLNAME)_mingw.def
 LDLIBS = -lstdc++
else
 DLL = $(DLLNAME)_i386.so
 LD = gcc
 LDFLAGS = -fPIC -shared
 LDLIBS = -lstdc++ -ldl
endif

# ---------------------------------------------------------------------------
# Targets
# ---------------------------------------------------------------------------
all: $(OBJDIR) som NNSim $(OBJDIR)/$(DLL)

install: all
	$(CP) $(OBJDIR)/$(DLL) $(CSDIR)
	$(CP) $(OBJDIR)/$(DLL) $(SERVERDIR)
	$(CP) $(OBJDIR)/$(DLL) $(SERVERDIR_CS15)

som:
	$(MAKE) -C ../NNSim/som OPT=$(OPT)

NNSim:
	$(MAKE) -C ../NNSim OPT=$(OPT)

$(OBJDIR):
	mkdir -p $(OBJDIR)
	
clean:
	-$(RM) $(OBJDIR)/*.o $(OBJDIR)/*.res $(OBJDIR)/$(DLL)

spotless:
	-$(RM) -r $(OBJDIR)

rebuild: clean all

# ---------------------------------------------------------------------------
# Rules
# ---------------------------------------------------------------------------
$(OBJDIR)/$(DLL): $(OBJ) $(OBJ_RES)
	$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

$(OBJDIR)/$(BASE_DLLNAME).res: $(BASE_DLLNAME).rc
	$(RC) $(RCFLAGS) -i$< -O coff -o $@

$(OBJDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
