#include <cstdint>

#include "flash_os.hpp"

int main() {
    // flag to mark if we want to run the test code
    constexpr static bool debug = false;

    // check if we want to run the debug code
    if constexpr (!debug) {
        // we do not want to run the debug code exit
        return 0;
    }

    // TODO: add a example here to test the flash algorithm

    return 0;
};