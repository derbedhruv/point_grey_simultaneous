# MultipleCameraEx makefile

CC = g++
OUTPUTNAME = out${D}
INCLUDE = -I./include/h
LIBS = -L/usr/src/flycapture/lib -lflycapture${D}

OUTDIR = .

OBJS = MultipleCameraEx.o

${OUTPUTNAME}: ${OBJS}
	${CC} -o ${OUTPUTNAME} ${OBJS} ${LIBS} ${COMMON_LIBS} 

%.o: %.cpp
	${CC} ${CFLAGS} ${INCLUDE} -Wall -c $*.cpp
	
clean_obj:
	rm -f ${OBJS}	@echo "all cleaned up!"

clean:
	rm -f ${OUTDIR}/${OUTPUTNAME} ${OBJS}	@echo "all cleaned up!"
