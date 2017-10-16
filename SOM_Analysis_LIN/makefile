CPP = g++

CPPFLAGS = -O2 -fexpensive-optimizations -fforce-mem -fforce-addr -w -DCPU=686 -I../NNSim/ -I../engine -I../common -I../pm_shared

OBJ = ../NNSim/SOM/nVec.o ../NNSim/SOM/CqSOM2d.o ../NNSim/SOM/SOMPattern.o ../NNSim/SOM/SOMPatternElem.o ../NNSim/SOM/fNBorKernel.o ../dlls/nn.o

exe:	${OBJ}
	${CPP} ${CPPFLAGS} ${OBJ} main.cpp -o $@

clean:
	-rm -rvf *.o
	-rm exe

%.o:	%.cpp
	${CPP} ${CPPFLAGS} -c $< -o $@