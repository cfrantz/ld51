include ../toolchain.mk

.PHONY: clean 
.PRECIOUS: *.o

SRCS=bcd.c nesutil.c
OBJS=$(SRCS:.c=.o)
ASMS=$(SRCS:.c=.s)

libutil.a: $(OBJS)
	$(AR) r $@ $(OBJS)

%.o: %.c
	$(CC) -Oi $< --add-source
	$(AS) $*.s

clean:
	@rm -fv $(OBJS) $(ASMS) *.a
