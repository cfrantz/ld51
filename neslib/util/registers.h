#ifndef EMPTYNES_NESLIB_UTIL_REGISTERS_H
#define EMPTYNES_NESLIB_UTIL_REGISTERS_H

#define register_write8(reg_, val_)  ( *(uint8_t*)(reg_) = (uint8_t)(val_) )
#define register_read8(reg_)         ( *(uint8_t*)(reg_) )
#define register_write16(reg_, val_) ( *(uint16_t*)(reg_) = (uint16_t)(val_) )
#define register_read16(reg_)        ( *(uint16_t*)(reg_) )

#endif // EMPTYNES_NESLIB_UTIL_REGISTERS_H
