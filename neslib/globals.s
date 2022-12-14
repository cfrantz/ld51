; Globals used by neslib

.exportzp _frame_count
.exportzp nptr, nlen, ntmp
.exportzp pal_update, pal_ptr
.exportzp _ppu_macro_len
.exportzp ppu_ctrl_var, ppu_mask_var
.exportzp scroll_x0, scroll_y0

.export pal_buf
.export _ppu_macro
.export scroll_x1, scroll_y1, scroll_nt, scroll_addr

.ZEROPAGE
_frame_count:               .res 2
nptr:                       .res 2
nlen:                       .res 2
ntmp:                       .res 4
pal_update:                 .res 1
pal_ptr:                    .res 2

_ppu_macro_len:             .res 1
ppu_ctrl_var:               .res 1
ppu_mask_var:               .res 1
scroll_x0:                  .res 1
scroll_y0:                  .res 1

.BSS
pal_buf:                    .res 32
_ppu_macro:                 .res 64
scroll_x1:                  .res 1
scroll_y1:                  .res 1
scroll_nt:                  .res 1
scroll_addr:                .res 1
