#ifndef EMPTYNES_MAPPERS_MMC3_MMC3_H
#define EMPTYNES_MAPPERS_MMC3_MMC3_H
#include <stdint.h>
#include "util/compiler.h"

void fastcall mmc3_reg(uint8_t reg, uint8_t val);

#define set_mmc3_low_bank(bank_) mmc3_reg(6, bank_)

#endif  // EMPTYNES_MAPPERS_MMC3_MMC3_H
