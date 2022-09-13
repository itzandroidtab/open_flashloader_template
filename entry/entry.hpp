#ifndef KLIB_ENTRY_HPP
#define KLIB_ENTRY_HPP

#include <stdint.h>

// extern c for c linkage in c++. No ifdef cplusplus as targets 
// should be implemented using c++
extern "C" {
    // pointer to the end of the stack. Definition is done in the 
    // linkerscript. Only the address of the variable should be used. The
    // address points to the correct location of the variable
    extern const uint32_t __stack_end;

    // pointer to the start of the heap. Definition is done in the 
    // linkerscript. Only the address of the variable should be used. The
    // address points to the correct location of the variable
    extern const uint32_t __heap_start;

    // pointer to the end of the heap. Definition is done in the 
    // linkerscript. Only the address of the variable should be used. The
    // address points to the correct location of the variable
    extern const uint32_t __heap_end;

    /**
     * @brief Generic reset handler that initializes the .bss and .data
     * segments. It calls all the constructors and runs main. When code
     * before main needs to be executed the "__constructor__" attribute
     * can be added to the function.
     * 
     */
    void __reset_handler();

    /**
     * @brief Default handler. Should be used to initialize the default
     * arm vector table. 
     * 
     */
    void __default_handler();
}

#endif