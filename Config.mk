# setup directory paths
OBJDIR_DEBUG_WIN32 = debug/win32
OBJDIR_RELEASE_WIN32 = release/win32
OBJDIR_DEBUG_LINUX = debug/linux
OBJDIR_RELEASE_LINUX = release/linux

ifeq ($(OS),Windows_NT)
 SDKDIR = /c/HLDev/sdk/multiplayer
 JOEBOTDIR = /c/HLDev/bots/joebot/src
 MMDIR = /c/HLDev/server/metamod/src/1.17
 OBJDIR_DEBUG = $(OBJDIR_DEBUG_WIN32)
 OBJDIR_RELEASE = $(OBJDIR_RELEASE_WIN32)
else
 SDKDIR = /home/netgames/install/hl/sdk/multiplayer
 JOEBOTDIR = /home/netgames/install/hl/bots/joebot/src
 MMDIR = /home/netgames/install/hl/server/metamod/src/1.17
 OBJDIR_DEBUG = $(OBJDIR_DEBUG_LINUX)
 OBJDIR_RELEASE = $(OBJDIR_RELEASE_LINUX)
endif

CSDIR = /c/Program\ Files/Steam/SteamApps/cruft@hotmail.com/counter-strike/cstrike/joebot
SERVERDIR = /c/HLServer/cstrike/joebot
SERVERDIR_CS15 = /c/Program\ Files/Half-Life/cstrike/joebot

EXTRA_CFLAGS =

ifneq ($(DLLTYPE),std)
 EXTRA_CFLAGS += -DUSE_METAMOD
endif
