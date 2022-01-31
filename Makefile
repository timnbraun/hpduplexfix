
.PHONY: clean all install
.DEFAULT_GOAL = all

CFLAGS =
CPPFLAGS = -Werror -Wall -m64 -mwin32
CXXFLAGS = -std=c++17
CROSS_COMPILE = x86_64-w64-mingw32-
LDLIBS += -lversion
CC = ${CROSS_COMPILE}gcc
CXX = ${CROSS_COMPILE}g++
RC = ${CROSS_COMPILE}windres
OBJ_DIR = obj
PROJ_DIR = $(shell pwd)

BUILD_DATE = $(shell date '+%y/%m/%d')

BINS = gethpduplex sethpduplex
all: ${BINS}

gethpduplex: $(addprefix ${OBJ_DIR}/, gethpduplex.o gethpduplex_res.o getversion.o)
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) $(OUTPUT_OPTION)

gethpduplex.o: gethpduplex.cpp | hpprinterkeys.h ${OBJ_DIR}
sethpduplex.o: sethpduplex.cpp | hpprinterkeys.h ${OBJ_DIR}

${OBJ_DIR}/gethpduplex.o : CPPFLAGS += -DBUILD_DATE=\"${BUILD_DATE}\"

${OBJ_DIR}/%.o: %.cpp | ${OBJ_DIR}
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

# On msys2, the windres compiler complains if it's run with a UNC as current directory
${OBJ_DIR}/%.o: %.rc | ${OBJ_DIR}
	cd $(HOME) ; $(RC) -o $(PROJ_DIR)/$@ $(PROJ_DIR)/$<

${OBJ_DIR} : ; mkdir -p $@

%: %.cpp
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) $(OUTPUT_OPTION)

clean:
	rm -f *.o ${BINS}
	rm -rf ${OBJ_DIR}
