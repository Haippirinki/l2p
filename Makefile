SOURCES = osx/main.mm common/Application.cpp

INCLUDE = -I.
FRAMEWORKS = -framework AppKit -framework OpenGL

OBJS = $(addprefix build/,$(patsubst %.cpp,%.o,$(patsubst %.mm,%.o,$(SOURCES))))

build/l2p: $(OBJS) Makefile
	@mkdir -p $(@D)
	clang++ -o build/l2p $(OBJS) $(FRAMEWORKS)

build/%.o: %.mm Makefile
	@mkdir -p $(@D)
	clang++ -c -o $@ $(INCLUDE) $<

build/%.o: %.cpp Makefile
	@mkdir -p $(@D)
	clang++ -c -o $@ $(INCLUDE) $<
