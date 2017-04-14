SOURCES = $(wildcard common/*.cpp)
SOURCES += osx/main.mm

CXXFLAGS = -std=c++11
INCLUDE = -I.
FRAMEWORKS = -framework AppKit -framework OpenGL

OBJS = $(addprefix build/,$(patsubst %.cpp,%.o,$(patsubst %.mm,%.o,$(SOURCES))))

build/l2p: $(OBJS) Makefile
	@mkdir -p $(@D)
	clang++ -o build/l2p $(OBJS) $(FRAMEWORKS)

build/%.o: %.mm Makefile
	@mkdir -p $(@D)
	clang++ -c -o $@ $(CXXFLAGS) $(INCLUDE) $<

build/%.o: %.cpp Makefile
	@mkdir -p $(@D)
	clang++ -c -o $@ $(CXXFLAGS) $(INCLUDE) $<
