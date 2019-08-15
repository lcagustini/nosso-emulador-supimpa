.segment "HEADER"
.byte "NES" ; signature
.byte $1A   ; signature
.byte $01   ; # of 16kb PRG-ROM banks
.byte $00   ; # of 8kb VROM banks
.byte $00   ; ROM control byte one
.byte $00   ; ROM control byte two
.byte $00   ; # of 8kb RAM banks
.byte $00   ; reserved

.segment "VECTORS"
.addr nmi
.addr reset
.addr irq

.segment "CODE"

reset:
  sei        ; ignore IRQs
  cld        ; disable decimal mode
  ldx #$40
  stx $4017  ; disable APU frame IRQ
  ldx #$ff
  txs        ; Set up stack
  inx        ; now X = 0
  stx $2000  ; disable NMI
  stx $2001  ; disable rendering
  stx $4010  ; disable DMC IRQs

; Optional (omitted):
; Set up mapper and jmp to further init code here.

; If the user presses Reset during vblank, the PPU may reset
; with the vblank flag still true.  This has about a 1 in 13
; chance of happening on NTSC or 2 in 9 on PAL.  Clear the
; flag now so the @vblankwait1 loop sees an actual vblank.
  bit $2002

; First of two waits for vertical blank to make sure that the
; PPU has stabilized
vblankwait1:  
  bit $2002
  bpl vblankwait1

; We now have about 30,000 cycles to burn before the PPU stabilizes.
; One thing we can do with this time is put RAM in a known state.
; Here we fill it with $00, which matches what (say) a C compiler
; expects for BSS.  Conveniently, X is still 0.
  txa
clrmem:
  sta $000,x
  sta $100,x
  sta $300,x
  sta $400,x
  sta $500,x
  sta $600,x
  sta $700,x  ; Remove this if you're storing reset-persistent data

; We skipped $200,x on purpose.  Usually, RAM page 2 is used for the
; display list to be copied to OAM.  OAM needs to be initialized to
; $EF-$FF, not 0, or you'll get a bunch of garbage sprites at (0, 0).

  inx
  bne clrmem

; Other things you can do between vblank waits are set up audio
; or set up other mapper registers.

vblankwait2:
  bit $2002
  bpl vblankwait2



nmi:
  rti

irq:
  rti

.segment "CHARS"
.segment "TILES"
