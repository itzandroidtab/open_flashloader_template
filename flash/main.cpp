#include <cstdint>
#include <array>

#include "flash_os.hpp"

/**
 * @brief Helper function to get the minimum of two values
 * 
 * @tparam T 
 * @param a 
 * @param b 
 * @return constexpr auto 
 */
template <typename T>
constexpr auto min(const T a, const T b) {
    return (a > b) ? b : a;
}

int main() {
    // flag to mark if we want to run the test code
    constexpr static bool debug = false;

    // check if we want to run the debug code
    if constexpr (!debug) {
        // we do not want to run the debug code exit
        return 0;
    }

    // test data buffer we use to write to the flash
    static const std::array<uint8_t, 512> testdata = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,

        // the rest is just zeroes
    };
    
    // buffer we store the data we read back into
    static std::array<uint8_t, 512> buffer = {};

    // call the init function we want to erase
    Init(FlashDevice.base_address, 0, 1);

    // erase a sector
    const auto erase_result = EraseSector(FlashDevice.base_address);

    // check for errors
    if (erase_result != 0) {
        // we have an error, lock up here so the user can debug
        while (true) {}
    }

    // uninit the flash with parameter erase
    UnInit(1);

    // call the init function we want to program
    Init(FlashDevice.base_address, 0, 2);

    // program a page
    const auto program_result = ProgramPage(
        FlashDevice.base_address, 
        min(testdata.size(), static_cast<std::size_t>(FlashDevice.page_size)),
        testdata.data()
    );

    // check for errors
    if (program_result != 0) {
        // we have an error, lock up here so the user can debug
        while (true) {}
    }

    // uninit the flash with parameter program
    UnInit(2);

    // call the init function we want to read
    Init(FlashDevice.base_address, 0, 0);

    // read back the data
    const auto read_result = SEGGER_OPEN_Read(
        FlashDevice.base_address, 
        min(buffer.size(), static_cast<std::size_t>(FlashDevice.size)), 
        buffer.data()
    );

    // check for errors
    if (read_result < 0) {
        // we have an error, lock up here so the user can debug
        while (true) {}
    }

    // uninit the flash with parameter read
    UnInit(0);

    return 0;
};