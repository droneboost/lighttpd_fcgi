TARGET = request.fcgi
OBJ = request.o

LINKFLAGS += \
    -lpthread \
    -lfcgi \
    -lrt

#INCLUDES = -I../../../../../../mtk/release/BDP_Generic/build/sysroot/usr/include
#           $(BDP_GENERIC_DIR)/build/sysroot/usr/include



.PHONY : default

default:$(TARGET)

$(OBJ) : $(SRC)

$(TARGET) : $(OBJ) 
	$(CXX) $(CXXFLAGS) $(CFLAGS) $(INCLUDES) $^ $(LINKFLAGS)  -o $@

#	$(CXX) $(CXXFLAGS) -o $@ ${OBJECT} ${LIBDIR} $(LINKFLAGS)

.c.o:
	$(CXX) ${CXXFLAGS} -c $< ${INCLUDES}

clean:
	-rm -f request.fcgi request.o

request.o: request.c 



