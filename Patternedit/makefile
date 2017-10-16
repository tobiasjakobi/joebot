CPP = gcc 
CPPFLAGS = -march=pentium -O2 -fexpensive-optimizations -w -I../engine -I../common -I../pm_shared -I../NNSim -I../NNSim/SOM

OBJ = bot.o bot_client.o bot_combat.o bot_names.o bot_chat.o bot_navigate.o dll.o engine.o \
	h_export.o linkfunc.o util.o waypoint.o nn.o NNWeapon.o ../NNSim/SOM/CqSOM2d.o \
	../NNSim/SOM/nVec.o ../NNSim/SOM/fNBorKernel.o ../NNSim/SOM/SOMPattern.o \
	../NNSim/SOM/SOMPatternElem.o CParser.o bot_wpdir.o

joebot.dll: ${OBJ} HPB_bot_ming.def
	dllwrap -s --def HPB_bot_ming.def --driver-name gcc -o joebot.dll \
	${OBJ} h:/mingw32/lib/libstdc++.a


HPB_bot_ming.def: ${OBJ}
	dllwrap -s --output-def HPB_bot_ming.def --driver-name gcc -o joebot.dll \
	${OBJ} h:/mingw32/lib/libstdc++.a
	type GiveFnpt.dat >> HPB_bot_ming.def

clean:
	-del *.o
	-del *_ming.def
	-del *.dll

%.o: %.cpp
	${CPP} ${CPPFLAGS} -c $< -o $@

%.o: %.c
	${CPP} ${CPPFLAGS} -c $< -o $@


