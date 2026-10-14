REPO_ROOT := $(shell git rev-parse --show-cdup)
# REPO_ROOT の終端は、'/', Git のルートとなるパスを取得してる

CXXFLAGS = -g
#----------------------------------------------------------------------
# GLEW/GL3W switch, default GLEW, GL3W を使うときは make GL3W=1
#----------------------------------------------------------------------
ifdef GL3W
	CXXFLAGS += -DGL3W
else
	CXXFLAGS += -DGLEW
	GLEW = 1
endif
#----------------------------------------------------------------------

CXX = g++
CC = gcc
CFLAGS = -g
DIR_GLCOMMON = $(REPO_ROOT)External/marglCommon
DIR_GL3W = $(REPO_ROOT)External/gl3w
DIR_IMGUI = $(REPO_ROOT)External/imgui
DIR_STB = $(REPO_ROOT)External/stb
INC = -I$(DIR_GLCOMMON) -I$(DIR_IMGUI) -I$(DIR_STB)
ifdef GL3W
INC += -I$(DIR_GL3W)
endif

ifeq ($(OS),Windows_NT)
  # Windows (msys2)
  LIBS = -lopengl32 -lglfw3
ifdef GLEW
  LIBS += -lglew32
endif
else
  # Linux
  LIBS = -lGL -lglfw -ldl
ifdef GLEW
  LIBS += -lGLEW
endif
endif

# prog*.cpp をメインモジュールとする
MAIN_SRC := $(wildcard prog*.cpp)
TARGET_BASE := $(basename $(MAIN_SRC))

TARGET = $(TARGET_BASE).exe
OBJS = $(TARGET_BASE).o marGLSLLite.o marTexture.o \
imgui.o imgui_widgets.o imgui_draw.o imgui_impl_glfw.o imgui_impl_opengl3.o imgui_tables.o
ifdef GL3W
  OBJS += gl3w.o
endif

.PHONY: all clean run
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

marGLSLLite.o: $(DIR_GLCOMMON)/marGLSLLite.cpp
	$(CXX) $(INC) $(CXXFLAGS) -c $< -o $@

marTexture.o: $(DIR_GLCOMMON)/marTexture.cpp
	$(CXX) $(INC) $(CXXFLAGS) -c $< -o $@

gl3w.o: $(DIR_GL3W)/gl3w.c
	$(CC) $(INC) $(CFLAGS) -c $< -o $@

$(TARGET_BASE).o: $(TARGET_BASE).cpp
	$(CXX) $(INC) $(CXXFLAGS) -c $< -o $@

imgui.o: $(DIR_IMGUI)/imgui.cpp
	$(CXX) $(INC) $(CXXFLAGS) -c $< -o $@

imgui_widgets.o: $(DIR_IMGUI)/imgui_widgets.cpp
	$(CXX) $(INC) $(CXXFLAGS) -c $< -o $@

imgui_draw.o: $(DIR_IMGUI)/imgui_draw.cpp
	$(CXX) $(INC) $(CXXFLAGS) -c $< -o $@

imgui_impl_glfw.o: $(DIR_IMGUI)/imgui_impl_glfw.cpp
	$(CXX) $(INC) $(CXXFLAGS) -c $< -o $@

imgui_impl_opengl3.o: $(DIR_IMGUI)/imgui_impl_opengl3.cpp
	$(CXX) $(INC) $(CXXFLAGS) -c $< -o $@

imgui_tables.o: $(DIR_IMGUI)/imgui_tables.cpp
	$(CXX) $(INC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET)
	rm -f *.o

run: $(TARGET)
	./$(TARGET)

##
# $@ ... target_name
# $* ... strip_extension(target_name)
# $< ... 最初の依存ファイル ただし borland make には存在せず
