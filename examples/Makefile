include ../toolchain.mk

.PHONY: clean 
.PRECIOUS: *.o

SRCS=../chars.c
OBJS=$(SRCS:.c=.o)
ASMS=$(SRCS:.c=.s)

MAPPER=../mappers/mmc3

hello.nes: hello.o $(OBJS)
	ld65 -C $(MAPPER)/nes.cfg -vm --mapfile hello.map -o $@ \
		hello.o $(OBJS) \
		$(MAPPER)/libinit.a \
		../util/libutil.a \
		$(NESLIB)

mmc5_audio_check.nes: mmc5_audio_check.o $(OBJS)
	ld65 -C ../mappers/mmc5/nes.cfg -vm --mapfile mmc5_audio_check.map -o $@ \
		mmc5_audio_check.o $(OBJS) \
		../mappers/mmc5/libinit.a \
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

clean:
	@rm -fv $(OBJS) $(ASMS) *.nes *.map \
		hello.o hello.s \
		mmc5_audio_check.o mmc5_audio_check.s 
