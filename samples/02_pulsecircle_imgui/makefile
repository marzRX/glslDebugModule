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
DIR_IMGUI = $(CURROOT)External/imgui
INC = -I$(DIR_GLCOMMON) -I$(DIR_IMGUI) -I$(DIR_GL3W)
ifdef WINDIR
  # Windows (msys2)
  LIBS = -lopengl32 -lglfw3
ifdef GLEW
  LIBS += -lglew32
endif
else
  # Linux
  LIBS = -lGL -lglfw -ldl
ifdef GLEW
  LIBS += -lGLEW -ldl
endif
endif

V?=2
TARGET = prog$(V).exe
OBJS = prog$(V).o marGLSLLite.o marTexture.o \
imgui.o imgui_widgets.o imgui_draw.o imgui_impl_glfw.o imgui_impl_opengl3.o imgui_tables.o
ifdef GL3W
  OBJS += gl3w.o
endif

$(TARGET): $(OBJS)
	$(CC) $(INC) -o $(TARGET) $(OBJS) $(LIBS)

marGLSLLite.o: $(DIR_GLCOMMON)/marGLSLLite.cpp
	$(CC) $(INC) $(CFLAGS) $(DIR_GLCOMMON)/marGLSLLite.cpp

marTexture.o: $(DIR_GLCOMMON)/marTexture.cpp
	$(CC) $(INC) $(CFLAGS) $(DIR_GLCOMMON)/marTexture.cpp

gl3w.o: $(DIR_GL3W)/gl3w.c
	$(CC) $(INC) $(CFLAGS) $(DIR_GL3W)/gl3w.c

prog$(V).o: prog$(V).cpp
	$(CC) $(INC) $(CFLAGS) prog$(V).cpp

imgui.o: $(DIR_IMGUI)/imgui.cpp
	$(CC) $(INC) $(CFLAGS) $(DIR_IMGUI)/imgui.cpp

imgui_widgets.o: $(DIR_IMGUI)/imgui_widgets.cpp
	$(CC) $(INC) $(CFLAGS) $(DIR_IMGUI)/imgui_widgets.cpp

imgui_draw.o: $(DIR_IMGUI)/imgui_draw.cpp
	$(CC) $(INC) $(CFLAGS) $(DIR_IMGUI)/imgui_draw.cpp

imgui_impl_glfw.o: $(DIR_IMGUI)/imgui_impl_glfw.cpp
	$(CC) $(INC) $(CFLAGS) $(DIR_IMGUI)/imgui_impl_glfw.cpp

imgui_impl_opengl3.o: $(DIR_IMGUI)/imgui_impl_opengl3.cpp
	$(CC) $(INC) $(CFLAGS) $(DIR_IMGUI)/imgui_impl_opengl3.cpp

imgui_tables.o: $(DIR_IMGUI)/imgui_tables.cpp
	$(CC) $(INC) $(CFLAGS) $(DIR_IMGUI)/imgui_tables.cpp

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
