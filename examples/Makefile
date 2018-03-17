include ../toolchain.mk

.PHONY: clean 
.PRECIOUS: *.o

SRCS=../chars.c
OBJS=$(SRCS:.c=.o)
ASMS=$(SRCS:.c=.s)

MAPPER=../mappers/mmc5

hello.nes: hello.o $(OBJS)
	ld65 -C $(MAPPER)/nes.cfg -vm --mapfile hello.map -o $@ \
		hello.o $(OBJS) \
		$(MAPPER)/libinit.a \
		../util/libutil.a \
		$(NESLIB)

mmc5_audio_check.nes: mmc5_audio_check.o $(OBJS)
	ld65 -C $(MAPPER)/nes.cfg -vm --mapfile mmc5_audio_check.map -o $@ \
		mmc5_audio_check.o $(OBJS) \
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
