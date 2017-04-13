SOURCES = osx/main.m

FRAMEWORKS = -framework AppKit -framework OpenGL

OBJS = $(addprefix build/,$(patsubst %.m,%.o,$(SOURCES)))

build/l2p: $(OBJS) Makefile
	mkdir -p $(@D)
	clang -o build/l2p $(OBJS) $(FRAMEWORKS)

build/%.o: %.m Makefile
	mkdir -p $(@D)
	clang -c -o $@ $<
