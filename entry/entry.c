#include <stdbool.h>
#include <stdint.h>

// type for constructors
typedef void (*entry_constructor)(void);

// declaration to the main function
int main();

/**
 * @brief Reset handler when the target starts running. This function
 * initilizes the bss and data segements
 * 
 * @details declares all linker variables as extern. Then we refer to the
 * value using the &operator as the variables is at a valid data address.
 * 
 * Functions that need to be called before main are run should have the
 * attribute "__constructor__". When marked the function will be added to
 * the ".init_array" segment and called before main is called.
 * 
 * @note When running this program in the debugger this will not be called.
 * This means the constructors and other items are not called during startup
 * 
 */
void __attribute__((__noreturn__, __naked__)) __reset_handler() {
    // initialize the stack pointer. As we are running from ram
    // the stack pointer is not setup yet. Move it to the stack
    // end segment to prevent a hardfault
    extern uint32_t __stack_end;
    asm volatile ("mov sp, %0" : : "r" (&__stack_end) : );

    extern uint8_t __bss_start;
    extern uint8_t __bss_end;

    // set the bss section to 0x00
    for (uint32_t i = 0; i < (&__bss_end - &__bss_start); i++) {
        ((volatile uint8_t*)(&__bss_start))[i] = 0x00;
    }

    // run main
    (void)main();

    // we should never be here. If this happens loop to make 
    // sure we never exit the reset handler
    while (true) {};
}

/**
 * @brief Default handler that locks the cpu. 
 * 
 */
void __default_handler() {
    // do nothing and wait
    while (true) {}
}

// called when a vft entry is not yet filled in
void __cxa_pure_virtual() {}
