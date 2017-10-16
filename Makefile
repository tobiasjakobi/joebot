# ---------------------------------------------------------------------------
# Definitions
# ---------------------------------------------------------------------------
include Config.mk

ifndef $(OPT)
 OPT = release
endif

# ---------------------------------------------------------------------------
# Targets
# ---------------------------------------------------------------------------
all: $(OPT)

release:
	$(MAKE) -C dlls OPT=release DLLTYPE=mm
	$(MAKE) -C dlls OPT=release DLLTYPE=std

debug:
	$(MAKE) -C dlls OPT=debug DLLTYPE=mm
	$(MAKE) -C dlls OPT=debug DLLTYPE=std

release_install:
	$(MAKE) -C dlls install OPT=release DLLTYPE=mm
	$(MAKE) -C dlls install OPT=release DLLTYPE=std

debug_install:
	$(MAKE) -C dlls install OPT=debug DLLTYPE=mm 
	$(MAKE) -C dlls install OPT=debug DLLTYPE=std

release_clean:
	$(MAKE) clean OPT=release DLLTYPE=mm
	$(MAKE) clean OPT=release DLLTYPE=std

debug_clean:
	$(MAKE) clean OPT=debug DLLTYPE=mm
	$(MAKE) clean OPT=debug DLLTYPE=std

release_rebuild:
	$(MAKE) rebuild OPT=debug DLLTYPE=mm
	$(MAKE) rebuild OPT=debug DLLTYPE=std

debug_rebuild:
	$(MAKE) rebuild OPT=debug DLLTYPE=mm
	$(MAKE) rebuild OPT=debug DLLTYPE=std

install: $(OPT)
	$(MAKE) -C dlls $@

clean:
	$(MAKE) -C dlls $@
	$(MAKE) -C NNSim $@
	$(MAKE) -C NNSim/som $@

rebuild:
	$(MAKE) -C dlls $@
	$(MAKE) -C NNSim $@
	$(MAKE) -C NNSim/som $@

# ---------------------------------------------------------------------------
# Rules
# ---------------------------------------------------------------------------
