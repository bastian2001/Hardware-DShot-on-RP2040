## PIO Commands

-   `JMP [cond] target`
    -   jumps to a location (potentially with condition)
    -   cond
        -   !X, !Y, X--, Y--: if X or Y (non-)zero, decrement afterwards
        -   X!=Y: X unequal to Y
        -   pin: depending on input pin EXECCTRL_JMP_PIN
        -   !OSRE: OSR (the one TO the state machine) HAS data (more bits than autopull threshold)
    -   target
        -   absolute position to jump to (best is via label)
-   `WAIT polarity gpio/pin/irq num (rel)`
    -   waits until a gpio/pin/irq has the required value
    -   polarity
        -   whether to wait for 1 or 0
    -   source
        -   gpio: RP pico pins 0...31
        -   pin: SM pins 0...31 (relative)
        -   irq: interrupt request, a bit more complicated
    -   num
        -   the number of the gpio/pin/irq
    -   rel
        -   if used, the irq number is the sum of the sm number and the irq
-   `IN source, bitcount`
    -   shifts bits into the ISR, potentially triggering autopush to the RX FIFO (to the main processor)
    -   source
        -   from where
        -   PINS, X, Y, NULL (just zeros), ISR, OSR
    -   bit count
        -   1...32 bits (00000 is 32 bits)
    -   stalls if RX FIFO full (if autopushed to RX FIFO)
    -   always uses source LSBs
    -   whether data is shifted into the left or right of the ISR is SM-dependent (when initializing the SM)
-   `OUT dest, bitcount`
    -   overrides the destination with bitcount bits from OSR and remaining zeros
    -   whether data from the right or left of the OSR is taken is SM-dependent (when initializing the SM)
    -   possibly triggers autopull from RX FIFO (output shift count reset), if this is empty: block
    -   dest
        -   pins
        -   X, Y
        -   NULL (discard)
        -   PINDIRS: SM pins(?)
        -   PC (program counter)
        -   ISR (also sets the ISR shift counter to bitcount)
        -   EXEC (like JS eval)
    -   bitcount: 1...32
-   `PUSH [iffull] [block/noblock]`
    -   pushes data (32 bit) from ISR into the RX FIFO
    -   iffull
        -   if set, only do it if the ISR is full (at least as many bits as threshold)
    -   block
        -   if set to block, stall if RX FIFO is full, default: block
    -   always (even if noblock is set) clears ISR data and shift count
-   `PULL [ifempty] [block/noblock]`
    -   pulls data (32 bit) from TX FIFO into OSR
    -   ifempty
        -   if set, only pull data if OSR is empty enough (autopull threshold)
    -   block
        -   if set to block, stall if TX FIFO is empty, default: block
        -   if set to unblock and the TX FIFO is empty, copy contents of X into OSR
-   `MOV dest [op] source`
    -   copies data (32 bit) from one to another location
    -   dest
        -   pins, X, Y, EXEC (JS eval), PC (program counter)
        -   ISR (shift counter = 0 => empty), OSR (shift counter = 0 => full)
    -   op
        -   ~, ! invert bitwise
        -   :: reverse bits
    -   source
        -   PINS, X, Y, NULL, STATUS (EXECCTRL_STATUS_SEL, all ones or all zeros), ISR, OSR
-   `IRQ [set/nowait/wait/clear] num (rel)`
    -   Set or clear IRQ flag
    -   options
        -   set/none: set IRQ flag
        -   clear: clears IRQ flag
        -   wait: halt until flag is lowered again by main processor (acknowledged)
-   `SET dest value`
    -   sets level on pins
    -   dest
        -   PINS: set up to 5 "set" pins
        -   X, Y: set the 5 LSBs of X or Y, all other are cleared
        -   PINDIRS: outputs or inputs
