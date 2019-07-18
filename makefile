SYS = WINDOWS64
DEBUG = NO

ADDIR = 
ACDDIR = 

ifeq ($(DEBUG),YES)
DEBUG_FLG = -g
ADDIR = $(DDIR)
ACDDIR = $(CDDIR)
endif 

DEF_CMP_FLG = $(DEBUG_FLG) $(INC_PTH)

HEADS1 = utils.h \
geom.h \
gui.h \
clock.h \
game.h \
game_draw.h \
d_game_query.h \
e_game_query.h \
game_gui.h \
maze.h \
images.h \
gui_util.h \
game_config.h \
texts.h \
game_extrap.h \
endgame.h \
direct_ex.h \

OBJ_NAMES = $(patsubst %.h,%.o, $(HEADS1))
OBJ_NAMES += main.o
OBJS = $(patsubst %.o, $(DIR)%.o, $(OBJ_NAMES))

HEADS = $(HEADS1) \
game_query.h \
game_consts.h \
ex_in_events.h \

ifeq ($(SYS), WINDOWS64)
#windows64
CC = g++
DIR = windows64/
CDIR = windows64\\
DDIR = debug/
CDDIR = debug\\
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
DDIR = debug/
CDDIR = debug\\
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
DDIR = debug/
CDDIR = $(DDIR)
EXEC = $(DIR)tanktrouble
CLN = rm
CMP_FLG = -std=c++11 $(DEF_CMP_FLG)
LNK_FLG = $(LIB_PTH) -static-libgcc -static-libstdc++ -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lenet
LIB_PTH = -L"enet/.libs"
INC_PTH = -I"enet/include" -I"/usr/include"

endif
endif
endif

DIR := $(DIR)$(ADDIR)
CDIR := $(CDIR)$(ACDDIR)

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CMP_FLG) $(OBJS) -o $(EXEC) $(LNK_FLG)

.SECONDEXPANSION:
$(DIR)main.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)utils.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) utils.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)gui_util.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) utils.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)geom.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) geom.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)clock.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) clock.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)texts.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) images.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)images.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) images.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)maze.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) maze.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)game_config.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) game_config.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)gui.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)game.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)d_game_query.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)e_game_query.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)game_draw.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)game_gui.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)game_extrap.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)endgame.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)direct_ex.o: $$(patsubst $(DIR)%.o, %.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@

CEXEC = $(patsubst $(DIR)%,$(CDIR)%, $(EXEC))
COBJS = $(patsubst $(DIR)%,$(CDIR)%, $(OBJS))

clean_all:
	$(CLN) $(CEXEC) $(COBJS)
clean:
	$(CLN) $(COBJS)