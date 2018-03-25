include ../../toolchain.mk

.PHONY: clean 
.PRECIOUS: *.o

SRCS=macro.c
OBJS=$(SRCS:.c=.o) crt0.o
ASMS=$(SRCS:.c=.s)

libinit.a: $(OBJS)
	$(AR) r $@ $(OBJS)

crt0.o: crt0.S famitone.S music.S neslib.S sounds.S
	$(AS) $<

%.o: %.c
	$(CC) -Oi $< --add-source
	$(AS) $*.s

clean:
	@rm -fv $(OBJS) $(ASMS)
