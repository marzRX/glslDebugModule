CURROOT = $(shell git rev-parse --show-cdup)
# CURROOT の終端は、'/', Git のルートとなるパスを取得してる

CFLAGS = -c -g
#----------------------------------------------------------------------
# GLEW/GL3W switch, default GLEW, GL3W を使うときは make GL3W=1
#----------------------------------------------------------------------
ifdef GL3W
	CFLAGS += -DGL3W
else
	CFLAGS += -DGLEW
	GLEW = 1
endif
#----------------------------------------------------------------------

CC = g++
DIR_GLCOMMON = $(CURROOT)External/marglCommon
DIR_GL3W = $(CURROOT)External/gl3w
DIR_STB = $(CURROOT)External/stb
INC = -I$(DIR_GLCOMMON) -I$(DIR_GL3W) -I$(DIR_STB)

ifdef WINDIR
  # Windows (msys2)
  LIBS = -lopengl32 -lglfw3
ifdef GLEW
  LIBS += -lglew32
endif
else
  # Linux
  LIBS = -lGL -lglfw
ifdef GLEW
  LIBS += -lGLEW
else
  LIBS += -ldl
endif
endif

V?=1
TARGET = prog$(V).exe
OBJS = prog$(V).o marGLSLLite.o marTexture.o marHRTimer.o
ifdef GL3W
  OBJS += gl3w.o
endif

$(TARGET): $(OBJS)
	$(CC) $(INC) -o $(TARGET) $(OBJS) $(LIBS)

marGLSLLite.o: $(DIR_GLCOMMON)/marGLSLLite.cpp
	$(CC) $(INC) $(CFLAGS) $(DIR_GLCOMMON)/marGLSLLite.cpp

marTexture.o: $(DIR_GLCOMMON)/marTexture.cpp
	$(CC) $(INC) $(CFLAGS) $(DIR_GLCOMMON)/marTexture.cpp

marHRTimer.o: $(DIR_GLCOMMON)/marHRTimer.cpp
	$(CC) $(INC) $(CFLAGS) $(DIR_GLCOMMON)/marHRTimer.cpp

gl3w.o: $(DIR_GL3W)/gl3w.c
	$(CC) $(INC) $(CFLAGS) $(DIR_GL3W)/gl3w.c

prog$(V).o: prog$(V).cpp
	$(CC) $(INC) $(CFLAGS) prog$(V).cpp

all: $(TARGET)

clean:
	-rm -f $(TARGET)
	-rm -f *.o

run: $(TARGET)
	./$(TARGET)

##
# $@ ... target_name
# $* ... strip_extension(target_name)
# $< ... 最初の依存ファイル ただし borland make には存在せず
