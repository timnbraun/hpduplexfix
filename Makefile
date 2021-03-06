
.PHONY: clean all install
.DEFAULT_GOAL = all

# auto-dependency files
DEPFLAGS = -MMD -MP -MF $(DEP_DIR)/$(@F:.o=.d)

CFLAGS =
CPPFLAGS = -Werror -Wall -m64 -mwin32 ${DEPFLAGS}
CXXFLAGS = -std=c++17
LDLIBS += -lversion -lstdc++
LDFLAGS += -m64 -mwin32 -mconsole
EXEEXT = .exe

CROSS_COMPILE = x86_64-w64-mingw32-
CC = ${CROSS_COMPILE}gcc
CXX = ${CROSS_COMPILE}g++
RC = ${CROSS_COMPILE}windres
PROJ_DIR = $(shell pwd)
OBJ_DIR := obj
DEP_DIR := .deps


BUILD_DATE = $(shell date '+%y/%m/%d')

gethpduplex_SRC = gethpduplex.cpp driverfeatures.cpp getversion.cpp \
	gethpduplex_res.rc
gethpduplex_OBJ = $(patsubst %.rc, $(OBJ_DIR)/%.o, \
	$(patsubst %.cpp, $(OBJ_DIR)/%.o, ${gethpduplex_SRC}))

sethpduplex_SRC = sethpduplex.cpp driverfeatures.cpp getversion.cpp \
	sethpduplex_res.rc
sethpduplex_OBJ = $(patsubst %.rc, $(OBJ_DIR)/%.o, \
	$(patsubst %.cpp, $(OBJ_DIR)/%.o, ${sethpduplex_SRC}))

BINS = gethpduplex${EXEEXT} sethpduplex${EXEEXT} # testconsole${EXEEXT}
all: ${BINS}

gethpduplex${EXEEXT}: ${gethpduplex_OBJ}
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) $(OUTPUT_OPTION)

sethpduplex${EXEEXT}: $(addprefix ${OBJ_DIR}/, sethpduplex.o sethpduplex_res.o driverfeatures.o getversion.o)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) $(OUTPUT_OPTION)

testconsole${EXEEXT}: $(OBJ_DIR)/testconsole.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) $(OUTPUT_OPTION)

${OBJ_DIR}/gethpduplex.o: gethpduplex.cpp | ${OBJ_DIR} ${DEP_DIR}
${OBJ_DIR}/sethpduplex.o: sethpduplex.cpp | ${OBJ_DIR} ${DEP_DIR}

${OBJ_DIR}/gethpduplex.o : CPPFLAGS += -DBUILD_DATE=\"${BUILD_DATE}\"
${OBJ_DIR}/sethpduplex.o : CPPFLAGS += -DBUILD_DATE=\"${BUILD_DATE}\"

${OBJ_DIR}/%.o: %.cpp | ${OBJ_DIR} ${DEP_DIR}
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

# On msys2, the windres compiler complains if it's run with a UNC as current directory
${OBJ_DIR}/%.o: %.rc | ${OBJ_DIR}
	cd $(HOME) ; $(RC) -o $(PROJ_DIR)/$@ $(PROJ_DIR)/$<

${OBJ_DIR} ${DEP_DIR} : ; mkdir -p $@

%${EXEEXT}: %.cpp
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) $(OUTPUT_OPTION)

clean:
	rm -f *.o ${BINS}
	rm -rf ${DEP_DIR} ${OBJ_DIR}

# Check if we're making the clean target; only include
# dependencies if we AREN'T
ifneq ($(MAKECMDGOALS), clean)
# Include dependency information
include $(wildcard ${DEP_DIR}/*.d)
endif
