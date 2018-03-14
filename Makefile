.PHONY: clean all
.PRECIOUS: *.o

PROJECT=project
all: $(PROJECT).nes
NESLIB=/usr/local/cc65/share/cc65/lib/nes.lib

SRCS=main.c bcd.c entity.c nesutil.c chars.c sprites.c screen.c \
	 levels.c \
	 level1.c \
	 level2.c
OBJS=$(SRCS:.c=.o)
ASMS=$(SRCS:.c=.s)

clean:
	@rm -fv $(OBJS) $(ASMS) crt0-mmc3.o lildigger.nes

crt0-mmc3.o: crt0-mmc3.s neslib-mmc3.s
	ca65 crt0-mmc3.s

level-util.o: level-util.s
	ca65 level-util.s

%.o: %.c
	cc65 -Oi $< --add-source
	ca65 $*.s

$(PROJECT).nes: nes-mmc3.cfg crt0-mmc3.o level-util.o $(OBJS)
	ld65 -C nes-mmc3.cfg -vm --mapfile $(PROJECT).map -o $@ \
		crt0-mmc3.o level-util.o \
		$(OBJS) \
		$(NESLIB)
