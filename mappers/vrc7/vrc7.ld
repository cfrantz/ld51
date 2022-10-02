MEMORY {
    # First 28 bytes of the zero page are used by NES library

    ZP: start = $30, size = $d0, type = rw, define = yes;

    # INES Cartridge Header

    HEADER: start = $0, size = $10, file = %O ,fill = yes;

    # 2 16K ROM Banks
    # - startup
    # - code
    # - rodata
    # - data (load)

    # ROM starts at $8000
    BANK0: start = $8000, size = $2000, file = %O ,fill = yes, define = yes;
    BANK1: start = $8000, size = $2000, file = %O ,fill = yes, define = yes;
    BANK2: start = $8000, size = $2000, file = %O ,fill = yes, define = yes;
    BANK3: start = $8000, size = $2000, file = %O ,fill = yes, define = yes;
    BANK4: start = $8000, size = $2000, file = %O ,fill = yes, define = yes;

    # VRC7
    PRG: start = $A000, size = $5d80, file = %O ,fill = yes, define = yes;

    # Hardware Vectors at end of the ROM
    VRC7BOOT: start = $fd80, size = $27a, file = %O, fill = yes;
    VECTORS: start = $fffa, size = $6, file = %O, fill = yes;

    # 1 8K CHR Bank

    CHR: start = $0000, size = $2000, file = %O, fill = yes;

    # standard 2K SRAM (-zeropage)
    # $0200 oam buffer
    # $0300..$800 ca65 stack

    #RAM: start = $0300, size = $0500, define = yes;

    # Use this definition instead if you going to use extra 8K RAM
    RAM: start = $6000, size = $2000, define = yes;
}

SEGMENTS {
    HEADER:   load = HEADER,         type = ro;
    LVLDAT0:  load = BANK0,          type = ro,  define = yes;
    LVLDAT1:  load = BANK1,          type = ro,  define = yes;
    LVLDAT2:  load = BANK2,          type = ro,  define = yes;
    LVLDAT3:  load = BANK3,          type = ro,  define = yes;
    LVLDAT4:  load = BANK4,          type = ro,  define = yes;
    STARTUP:  load = PRG,            type = ro,  define = yes;
    LOWCODE:  load = PRG,            type = ro,                optional = yes;
    ONCE:     load = PRG,            type = ro,  define = yes, optional = yes;
    INIT:     load = PRG,            type = ro,  define = yes, optional = yes;
    CODE:     load = PRG,            type = ro,  define = yes;
    RODATA:   load = PRG,            type = ro,  define = yes;
    DATA:     load = PRG, run = RAM, type = rw,  define = yes;
    VRC7BOOT: load = VRC7BOOT,       type = ro,  define = yes;
    VECTORS:  load = VECTORS,        type = ro,  define = yes;
    CHARS:    load = CHR,            type = rw;
    BSS:      load = RAM,            type = bss, define = yes;
    HEAP:     load = RAM,            type = bss, optional = yes;
    ZEROPAGE: load = ZP,             type = zp;
}

FEATURES {
    CONDES: segment = INIT,
            type = constructor,
            label = __CONSTRUCTOR_TABLE__,
            count = __CONSTRUCTOR_COUNT__;
    CONDES: segment = RODATA,
            type = destructor,
            label = __DESTRUCTOR_TABLE__,
            count = __DESTRUCTOR_COUNT__;
    CONDES: type = interruptor,
            segment = RODATA,
            label = __INTERRUPTOR_TABLE__,
            count = __INTERRUPTOR_COUNT__;
}

SYMBOLS {
    __STACKSIZE__: type = weak, value = $0500;  # 5 pages stack
}
