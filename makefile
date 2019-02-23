SYS = WINDOWS64
DEBUG = NO

ifeq ($(DEBUG),YES)
DEBUG_FLG = -g
endif 

DEF_CMP_FLG = $(DEBUG_FLG) $(INC_PTH)

HEADS1 = \
utils.h \
geom.h \
gui.h \
clock.h \
main_scr.h \
texts.h \
utf8.h \
keys.h \
network.h

GOBJ_NAMES = $(patsubst %.h,%.o, $(HEADS))
COBJ_NAMES = $(GOBJ_NAMES) main.o
SOBJ_NAMES = $(GOBJ_NAMES) server_main.o
COBJS = $(patsubst %.o, $(DIR)%.o, $(COBJ_NAMES))
SOBJS = $(patsubst %.o, $(DIR)%.o, $(SOBJ_NAMES))

HEADS = $(HEADS1)

ifeq ($(SYS), WINDOWS64)
#windows64
CC = g++
DIR = windows64/
CDIR = windows64\\
CEXEC = $(DIR)tanktrouble.exe
SEXEC = $(DIR)tanktrouble_server.exe
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
CEXEC = $(DIR)tanktrouble.exe
SEXEC = $(DIR)tanktrouble_server.exe
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
CEXEC = $(DIR)tanktrouble
SEXEC = $(DIR)tanktrouble_server
CLN = rm
CMP_FLG = -std=c++11 $(DEF_CMP_FLG)
LNK_FLG = $(LIB_PTH) -static-libgcc -static-libstdc++ -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lenet
LIB_PTH = -L"enet/.libs"
INC_PTH = -I"enet/include" -I"/usr/include"

endif
endif
endif

all: $(CEXEC) $(SEXEC)

$(CEXEC): $(COBJS)
	$(CC) $(CMP_FLG) $^ -o $@ $(LNK_FLG)
$(SEXEC): $(SOBJS)
	$(CC) $(CMP_FLG) $^ -o $@ $(LNK_FLG)

.SECONDEXPANSION:
$(DIR)main.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)server_main.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)keys.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) keys.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)utf8.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) utf8.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)utils.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) utils.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)geom.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) geom.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)clock.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) clock.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)texts.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) texts.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)network.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) network.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)gui.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)main_scr.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@

CLEXEC = $(patsubst $(DIR)%,$(CDIR)%, $(EXEC))
CLOBJS = $(patsubst $(DIR)%,$(CDIR)%, $(OBJS))

clean_all:
	$(CLN) $(CLEXEC) $(CLOBJS)
clean:
	$(CLN) $(CLOBJS)