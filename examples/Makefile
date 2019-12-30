include ../toolchain.mk

.PHONY: clean 
.PRECIOUS: *.o

SRCS=../chars.c
OBJS=$(SRCS:.c=.o)
ASMS=$(SRCS:.c=.s)

MAPPER=../mappers/mmc3

hello.nes: hello.o hello_music.o $(OBJS)
	ld65 -C $(MAPPER)/nes.cfg -vm --mapfile hello.map -o $@ \
		hello.o $(OBJS) \
		hello_music.o \
		$(MAPPER)/libinit.a \
		../util/libutil.a \
		$(NESLIB)

vrc6_audio_check.nes: vrc6_audio_check.o $(OBJS)
	ld65 -C $(MAPPER)/nes.cfg -vm --mapfile vrc6_audio_check.map -o $@ \
		vrc6_audio_check.o $(OBJS) \
		$(MAPPER)/libinit.a \
		../util/libutil.a \
		$(NESLIB)

fourscr.nes: fourscr.o $(OBJS)
	ld65 -C $(MAPPER)/nes.cfg -vm --mapfile fourscr.map -o $@ \
		fourscr.o $(OBJS) \
		$(MAPPER)/libinit.a \
		../util/libutil.a \
		$(NESLIB)

%.o: %.c
	$(CC) -I.. -Oi $< --add-source
	$(AS) $*.s

%.o: %.S
	$(AS) $*.S

clean:
	@rm -fv $(OBJS) $(ASMS) *.nes *.map \
		hello.o hello.s \
		mmc5_audio_check.o mmc5_audio_check.s 
