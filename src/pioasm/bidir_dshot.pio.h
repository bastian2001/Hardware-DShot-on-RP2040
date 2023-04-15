// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// ----------- //
// bidir_dshot //
// ----------- //

#define bidir_dshot_wrap_target 1
#define bidir_dshot_wrap 27

#define bidir_dshot_DSHOT2400_CLKDIV_INT 1
#define bidir_dshot_DSHOT2400_CLKDIV_FRAC 96
#define bidir_dshot_DSHOT1200_CLKDIV_INT 2
#define bidir_dshot_DSHOT1200_CLKDIV_FRAC 192
#define bidir_dshot_DSHOT600_CLKDIV_INT 5
#define bidir_dshot_DSHOT600_CLKDIV_FRAC 128
#define bidir_dshot_DSHOT300_CLKDIV_INT 11
#define bidir_dshot_DSHOT300_CLKDIV_FRAC 0

static const uint16_t bidir_dshot_program_instructions[] = {
    0xe001, //  0: set    pins, 1                    
            //     .wrap_target
    0xe081, //  1: set    pindirs, 1                 
    0xe043, //  2: set    y, 3                       
    0x8080, //  3: pull   noblock                    
    0xa0c3, //  4: mov    isr, null                  
    0x4002, //  5: in     pins, 2                    
    0xa027, //  6: mov    x, osr                     
    0x8080, //  7: pull   noblock                    
    0xa026, //  8: mov    x, isr                     
    0x00a4, //  9: jmp    x != y, 4                  
    0xa027, // 10: mov    x, osr                     
    0xa0ef, // 11: mov    osr, !osr                  
    0x6070, // 12: out    null, 16                   
    0xee00, // 13: set    pins, 0                [14]
    0x6e01, // 14: out    pins, 1                [14]
    0xe801, // 15: set    pins, 1                [8] 
    0x00ed, // 16: jmp    !osre, 13                  
    0xe080, // 17: set    pindirs, 0                 
    0xa0eb, // 18: mov    osr, !null                 
    0x604a, // 19: out    y, 10                      
    0x0095, // 20: jmp    y--, 21                    
    0x0061, // 21: jmp    !y, 1                      
    0x00d4, // 22: jmp    pin, 20                    
    0xe054, // 23: set    y, 20                      
    0xb5c3, // 24: mov    isr, null              [21]
    0x5d01, // 25: in     pins, 1                [29]
    0x0099, // 26: jmp    y--, 25                    
    0x8000, // 27: push   noblock                    
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program bidir_dshot_program = {
    .instructions = bidir_dshot_program_instructions,
    .length = 28,
    .origin = -1,
};

static inline pio_sm_config bidir_dshot_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + bidir_dshot_wrap_target, offset + bidir_dshot_wrap);
    return c;
}
#endif

