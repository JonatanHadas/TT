SYS = WINDOWS64
DEBUG = NO

ADDIR = 

ifeq ($(DEBUG),YES)
DEBUG_FLG = -g
ADDIR = $(DDIR)
endif 

DEF_CMP_FLG = $(DEBUG_FLG) $(INC_PTH)

HEADS1 = \
utils$(SEP)utils.h \
utils$(SEP)geom.h \
gui_utils$(SEP)gui.h \
gui_utils$(SEP)clock.h \
main_scr.h \
gui_utils$(SEP)texts.h \
utils$(SEP)utf8.h \
keys.h \
network$(SEP)network.h \
game$(SEP)game.h \
game_gui$(SEP)game_draw.h \
game$(SEP)d_game_query.h \
game_gui.h \
game$(SEP)maze.h \
game_gui$(SEP)images.h \
game$(SEP)e_game_query.h \
gui_utils$(SEP)gui_util.h \
game$(SEP)game_config.h \
gui_utils$(SEP)texts.h \
game$(SEP)game_extrap.h \
gui_utils$(SEP)vfx.h \
endgame.h \
game_gui$(SEP)sounds.h \
game$(SEP)direct_ex.h \
network$(SEP)encoding.h \
server$(SEP)game_setup.h \
server$(SEP)netgame.h \
game$(SEP)net_ex.h \

GOBJ_NAMES = $(patsubst %.h,%.o, $(HEADS1))
COBJ_NAMES = $(GOBJ_NAMES) main.o
SOBJ_NAMES = $(GOBJ_NAMES) server$(SEP)server_main.o
COBJS = $(patsubst %.o, $$(DIR)%.o, $(COBJ_NAMES))
SOBJS = $(patsubst %.o, $$(DIR)%.o, $(SOBJ_NAMES))

SOURCES_DIR = src

HEADS = $(HEADS1) \
game$(SEP)game_query.h \
game$(SEP)game_consts.h \
game$(SEP)ex_in_events.h \

ifeq ($(SYS), WINDOWS64)
#windows64
CC = g++
SEP = \\
DIR = windows64$(SEP)
DDIR = debug$(SEP)
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
SEP = \\
CC = g++
DIR = windows$(SEP)
DDIR = debug$(SEP)
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
SEP = /
CC = g++
DIR = linux$(SEP)
DDIR = debug$(SEP)
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

DIR := $(DIR)$(ADDIR)

HEADS := $(patsubst %.h, $(SOURCES_DIR)$(SEP)%.h, $(HEADS))

all: $(CEXEC) $(SEXEC)

.SECONDEXPANSION:
$(CEXEC): $(COBJS)
	$(CC) $(CMP_FLG) $^ -o $@ $(LNK_FLG)
$(SEXEC): $(SOBJS)
	$(CC) $(CMP_FLG) $^ -o $@ $(LNK_FLG)


$(DIR)main.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)server$(SEP)server_main.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)keys.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(SOURCES_DIR)$(SEP)keys.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)utils$(SEP)utf8.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(SOURCES_DIR)$(SEP)utils$(SEP)utf8.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)utils$(SEP)utils.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(SOURCES_DIR)$(SEP)utils$(SEP)utils.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)gui_utils$(SEP)gui_util.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(SOURCES_DIR)$(SEP)gui_utils$(SEP)gui_util.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)utils$(SEP)geom.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(SOURCES_DIR)$(SEP)utils$(SEP)geom.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)gui_utils$(SEP)clock.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(SOURCES_DIR)$(SEP)gui_utils$(SEP)clock.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)gui_utils$(SEP)texts.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(SOURCES_DIR)$(SEP)gui_utils$(SEP)texts.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)network$(SEP)network.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(SOURCES_DIR)$(SEP)network$(SEP)network.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)game_gui$(SEP)images.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(SOURCES_DIR)$(SEP)game_gui$(SEP)images.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)game_gui$(SEP)sounds.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(SOURCES_DIR)$(SEP)game_gui$(SEP)sounds.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)gui_utils$(SEP)vfx.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(SOURCES_DIR)$(SEP)gui_utils$(SEP)vfx.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)game$(SEP)maze.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(SOURCES_DIR)$(SEP)game$(SEP)maze.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)game$(SEP)game_config.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(SOURCES_DIR)$(SEP)game$(SEP)game_config.h
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)gui_utils$(SEP)gui.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)game$(SEP)game.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)game$(SEP)d_game_query.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)game$(SEP)e_game_query.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)game_gui$(SEP)game_draw.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)game_gui.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)game$(SEP)game_extrap.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)endgame.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)game$(SEP)direct_ex.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)main_scr.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)network$(SEP)encoding.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)server$(SEP)game_setup.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)server$(SEP)netgame.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@
$(DIR)game$(SEP)net_ex.o: $$(patsubst $(DIR)%.o, $(SOURCES_DIR)%.cpp, $$@) $(HEADS)
	$(CC) $(CMP_FLG) -c $< -o $@

clean_all:
	$(CLN) $(EXEC) $(COBJS) $(SOBJS)
clean:
	$(CLN) $(COBJS) $(SOBJS)