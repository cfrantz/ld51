include ../../toolchain.mk

.PHONY: clean 
.PRECIOUS: *.o

TOPDIR=../..
SRCS=../../chars.c
OBJS=$(SRCS:.c=.o)
ASMS=$(SRCS:.c=.s)

MAPPER=../../mappers/mmc5

mmc5_scanline_split.nes: mmc5_scanline_split.o $(OBJS)
	ld65 -C $(MAPPER)/nes.cfg -vm --mapfile mmc5_scanline_split.map -o $@ \
		mmc5_scanline_split.o $(OBJS) \
		$(MAPPER)/libinit.a \
		../../util/libutil.a \
		$(NESLIB)

mmc5_vsplit.nes: mmc5_vsplit.o $(OBJS)
	ld65 -C $(MAPPER)/nes.cfg -vm --mapfile mmc5_vsplit.map -o $@ \
		mmc5_vsplit.o $(OBJS) \
		$(MAPPER)/libinit.a \
		../../util/libutil.a \
		$(NESLIB)

mmc5_extram_fillmode.nes: mmc5_extram_fillmode.o $(OBJS)
	ld65 -C $(MAPPER)/nes.cfg -vm --mapfile mmc5_extram_fillmode.map -o $@ \
		mmc5_extram_fillmode.o $(OBJS) \
		$(MAPPER)/libinit.a \
		../../util/libutil.a \
		$(NESLIB)

mmc5_audio_check.nes: mmc5_audio_check.o $(OBJS)
	ld65 -C $(MAPPER)/nes.cfg -vm --mapfile mmc5_audio_check.map -o $@ \
		mmc5_audio_check.o $(OBJS) \
		$(MAPPER)/libinit.a \
		../../util/libutil.a \
		$(NESLIB)


%.o: %.c
	$(CC) -I$(TOPDIR) -Oi $< --add-source
	$(AS) $*.s

%.o: %.S
	$(AS) $*.S

clean:
	@rm -fv $(OBJS) $(ASMS) *.nes *.map \
		mmc5_scanline_split.o mmc5_scanline_split.s \
		mmc5_vsplit.o mmc5_vsplit.s \
		mmc5_extram_fillmode.o mmc5_extram_fillmode.s \
		mmc5_audio_check.o mmc5_audio_check.s 
