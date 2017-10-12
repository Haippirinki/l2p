.PHONY: all

SOURCES = $(wildcard common/*.cpp) $(wildcard common/*/*.cpp)
LEVEL_SOURCES = $(wildcard levels/level-*.py)

ifeq ($(shell uname -s),Darwin)
	SOURCES += osx/main.mm osx/Platform.mm
	CXX = clang++
	CXXFLAGS = -std=c++11 -I.
	LDFLAGS = -framework AppKit -framework OpenGL
	EXE_SUFFIX =
else
	SOURCES += win32/main.cpp win32/GL_3_3.cpp win32/Platform.cpp
	CXX = g++
	CXXFLAGS = -std=c++11 -I.
	LDFLAGS = -mwindows -lopengl32
	EXE_SUFFIX = .exe
endif

OBJS = $(addprefix build/,$(patsubst %.cpp,%.o,$(patsubst %.mm,%.o,$(SOURCES))))
LEVELS = $(addprefix assets/,$(patsubst %.py,%.level,$(LEVEL_SOURCES)))
LEVEL_INDEX = assets/levels/index
EXE = build/l2p$(EXE_SUFFIX)

all: $(EXE) $(LEVELS) $(LEVEL_INDEX)

$(EXE): $(OBJS) Makefile
	@echo Linking $(@F)
	@mkdir -p $(@D)
	@$(CXX) -o $@ $(OBJS) $(LDFLAGS)

build/%.o: %.mm Makefile
	@echo Compiling $(<F)
	@mkdir -p $(@D)
	@$(CXX) -M -MP -MT $@ -MF $(patsubst %.o,%.dep,$@) $(CXXFLAGS) $<
	@$(CXX) -c -Wall -Wfloat-conversion -Wno-missing-braces -o $@ $(CXXFLAGS) $<

build/%.o: %.cpp Makefile
	@echo Compiling $(<F)
	@mkdir -p $(@D)
	@$(CXX) -M -MP -MT $@ -MF $(patsubst %.o,%.dep,$@) $(CXXFLAGS) $<
	@$(CXX) -c -Wall -Wfloat-conversion -Wno-missing-braces -o $@ $(CXXFLAGS) $<

assets/%.level: %.py levels/gen.py Makefile
	@echo Generating $(@F)
	@mkdir -p $(@D)
	@PYTHONPATH=levels python -B $< > $@

$(LEVEL_INDEX): $(LEVELS) Makefile
	@echo Generating the level index
	@mkdir -p $(@D)
	@echo $(sort $(LEVELS)) | tr " " "\n" > $@

-include $(patsubst %.o,%.dep,$(OBJS))
