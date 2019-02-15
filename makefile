SYS = WINDOWS64
DEBUG = NO

ifeq ($(DEBUG),YES)
DEBUG_FLG = -g
endif 

DEF_CMP_FLG = $(DEBUG_FLG) $(INC_PTH)

HEADS = utils.h geom.h gui.h clock.h

OBJ_NAMES = $(patsubst %.h,%.o, $(HEADS))
OBJ_NAMES += main.o
OBJS = $(patsubst %.o, $(DIR)%.o, $(OBJ_NAMES))

ifeq ($(SYS), WINDOWS64)
#windows64
CC = g++
DIR = windows64/
CDIR = windows64\\
EXEC = $(DIR)tanktrouble.exe
CLN = del
CMP_FLG = $(DEF_CMP_FLG)
LNK_FLG = $(LIB_PTH) -static-libgcc -static-libstdc++ -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lenet64 -lws2_32 -lwinmm
LIB_PTH = -L"C:\MinGW\lib" -L"enet"
INC_PTH = -I"C:\MinGW\include" -I"enet\include"

else
ifeq ($(SYS), WINDOWS)
#windows
CC = g++
DIR = windows/
CDIR = windows\\
EXEC = $(DIR)tanktrouble.exe
CLN = del
CMP_FLG = -std=c++11 $(DEF_CMP_FLG)
LNK_FLG = $(LIB_PTH) -static-libgcc -static-libstdc++ -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lenet -lws2_32 -lwinmm
LIB_PTH = -L"C:\Program Files\mingw-w64\i686-8.1.0-posix-dwarf-rt_v6-rev0\mingw32\lib" -L"enet"
INC_PTH = -I"C:\Program Files\mingw-w64\i686-8.1.0-posix-dwarf-rt_v6-rev0\mingw32\include" -I"enet\include"

else
ifeq ($(SYS), LINUX)
#linux
CC = g++
DIR = linux/
CDIR = $(DIR)
EXEC = $(DIR)tanktrouble
CLN = rm
CMP_FLG = -std=c++11 $(DEF_CMP_FLG)
LNK_FLG = $(LIB_PTH) -static-libgcc -static-libstdc++ -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lenet
LIB_PTH = -L"enet/.libs"
INC_PTH = -I"enet/include" -I"/usr/include"

endif
endif
endif

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CMP_FLG) $(OBJS) -o $(EXEC) $(LNK_FLG)

.SECONDEXPANSION:
$(DIR)main.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)utils.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) utils.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)geom.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) geom.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)clock.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) clock.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)gui.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@

CEXEC = $(patsubst $(DIR)%,$(CDIR)%, $(EXEC))
COBJS = $(patsubst $(DIR)%,$(CDIR)%, $(OBJS))

clean_all:
	$(CLN) $(CEXEC) $(COBJS)
clean:
	$(CLN) $(COBJS)