include ../../toolchain.mk

.PHONY: clean 
.PRECIOUS: *.o

SRCS=
OBJS=$(SRCS:.c=.o) crt0.o
ASMS=$(SRCS:.c=.s)

libinit.a: $(OBJS)
	$(AR) r $@ $(OBJS)

crt0.o: crt0.s
	$(AS) $<

%.o: %.c
	$(CC) -Oi $< --add-source
	$(AS) $*.s

clean:
	@rm -fv $(OBJS) $(ASMS)
