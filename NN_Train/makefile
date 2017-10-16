CPP = g++
CPPFLAGS = -O2 -fexpensive-optimizations -fforce-mem -fforce-addr -DCPU=586

OBJ = nn.o
NN_Train:	${OBJ}
	${CPP} ${OBJ} main.cpp -o $@
clean:
	-rm -vf *.o

%.o:	%.cpp
	${CPP} ${CPPFLAGS} -c $< -o $@