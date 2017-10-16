# ---------------------------------------------------------------------------
# Definitions
# ---------------------------------------------------------------------------
include ../../Config.mk

ifeq ($(OPT),debug)
 OBJDIR = $(OBJDIR_DEBUG)
else
 OBJDIR = $(OBJDIR_RELEASE)
endif

ifeq ($(OS),Windows_NT)
 OBJDIR := $(OBJDIR)_mingw
endif

# build source list
include Config.mk
OBJ := $(SRC:%.cpp=$(OBJDIR)/%.o)

CC = gcc
RM = rm -fv

# includes
BASECFLAGS = -I$(JOEBOTDIR)/engine -I$(JOEBOTDIR)/common\
 -I$(JOEBOTDIR)/pm_shared -I$(MMDIR)/metamod -I$(JOEBOTDIR)/NNSim/som\
 -I$(JOEBOTDIR)/NNSim -I$(JOEBOTDIR)/dlls

BASECFLAGS += $(EXTRA_CFLAGS)

ifeq ($(OPT),debug)
 CFLAGS = $(BASECFLAGS) -g
else
 CFLAGS = $(BASECFLAGS) -O2 -march=i586 -O2 -ffast-math -funroll-loops\
  -fomit-frame-pointer -fexpensive-optimizations
 ifeq ($(OS),Windows_NT)
  CFLAGS += -falign-loops=2 -falign-jumps=2 -falign-functions=2
 else
  CFLAGS += -malign-loops=2 -malign-jumps=2 -malign-functions=2
 endif
endif

# ---------------------------------------------------------------------------
# Targets
# ---------------------------------------------------------------------------
all: $(OBJDIR) $(OBJ)

$(OBJDIR):
	mkdir -p $(OBJDIR)
	
clean:
	-$(RM) $(OBJDIR)/*.o

spotless:
	-$(RM) -r $(OBJDIR)

rebuild: clean all

# ---------------------------------------------------------------------------
# Rules
# ---------------------------------------------------------------------------
$(OBJDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
