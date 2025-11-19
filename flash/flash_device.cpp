#include <cstdint>
#include "flash_os.hpp"

/**
 * @brief Smallest amount of data that can be programmed
 * 
 * <PageSize> = 2 ^ Shift. Shift = 3 => <PageSize> = 2^3 = 8 bytes
 * 
 */
#define PAGE_SIZE_SHIFT (8)

/**
 * @brief If value is false the device does not support native read. This 
 * makes the loader use the read function instead of using memory mapped 
 * io.
 * 
 */
#define NATIVE_READ (false)

/**
 * @brief Marks if the device supports a chip erase. This can speed up erasing
 * a chip.
 * 
 */
#define CHIP_ERASE (false)

/**
 * @brief If value is true only uniform sectors are allowed on the device. 
 * Speeds up erasing as it can erase multiple sectors at once.
 * 
 */
#define UNIFORM_SECTORS (true)

/**
 * @brief Sector size shift for when using uniform sector erase
 * 
 * <SectorSize> = 2 ^ Shift. Shift = 12 => <SectorSize> = 2 ^ 12 = 4096 bytes
 * 
 */
#define SECTOR_SIZE_SHIFT (12)

/**
 * @brief Use a custom verify. Is optional. Speeds up verifying
 * 
 */
#define CUSTOM_VERIFY (false)

/**
 * @brief Enable changes to the sector layout at runtime. Can be used to create
 * one flash loader that supports multiple chips (like multiple variants of the
 * same NOR flash)
 * 
 */
#define RUNTIME_SECTORS (false)


/**
 * @brief Device specific infomation
 * 
 */
extern "C" {
    // Mark start of <PrgData> segment. Non-static to make sure linker can keep this 
    // symbol. Dummy needed to make sure that <PrgData> section in resulting ELF file 
    // is present. Needed by open flash loader logic on PC side
    volatile int PRGDATA_StartMarker __attribute__ ((section ("PrgData"), __used__));
}

// definition for the flash device
const __attribute__ ((section("DevDscr"), __used__)) flash_device FlashDevice = {
    flash_drv_version, // driver version
    "test device", // device name
    device_type::on_chip, // device type
    0xA0000000, // base address
    0x00000400, // flash size
    4, // page size
    0, // reserved
    0xff, // blank value
    100, // page program timeout
    3000, // sector erase timeout

    // flash sectors
    {
        {0x00000400, 0x00000000},
        device::end_of_sectors
    }
};

// function overrides when parts are not in use
#if NATIVE_READ
    #define BLANK_CHECK_FUNC nullptr
    #define OPEN_READ_FUNC nullptr
#else
    #define BLANK_CHECK_FUNC BlankCheck
    #define OPEN_READ_FUNC SEGGER_OPEN_Read
#endif

#if CUSTOM_VERIFY
    #define VERIFY_FUNC Verify
#else 
    #define VERIFY_FUNC nullptr
#endif

#if CHIP_ERASE
    #define CHIP_ERASE_FUNC EraseChip
#else
    #define CHIP_ERASE_FUNC nullptr
#endif

#if UNIFORM_SECTORS
    #define UNIFORM_ERASE_FUNC SEGGER_OPEN_Erase
#else
    #define UNIFORM_ERASE_FUNC nullptr
#endif

#if RUNTIME_SECTORS
    #define RUNTIME_SECTORS_FUNC SEGGER_OPEN_GetFlashInfo
#else
    #define RUNTIME_SECTORS_FUNC nullptr
#endif

/**
 * @brief array with all the functions for the segger software
 * 
 */
extern "C" {
    // declaration for the OFL Api. If we initialize it here we get
    // a wrong name in the symbol table
    extern const uint32_t SEGGER_OFL_Api[];
}

// definition of OFL Api
const uint32_t SEGGER_OFL_Api[] __attribute__ ((section ("PrgCode"), __used__)) = {
    reinterpret_cast<uint32_t>(FeedWatchdog),
    reinterpret_cast<uint32_t>(Init),
    reinterpret_cast<uint32_t>(UnInit),
    reinterpret_cast<uint32_t>(EraseSector),
    reinterpret_cast<uint32_t>(ProgramPage),
    reinterpret_cast<uint32_t>(BLANK_CHECK_FUNC),
    reinterpret_cast<uint32_t>(CHIP_ERASE_FUNC),
    reinterpret_cast<uint32_t>(VERIFY_FUNC),
    reinterpret_cast<uint32_t>(nullptr), // SEGGER_OPEN_CalcCRC
    reinterpret_cast<uint32_t>(OPEN_READ_FUNC),
    reinterpret_cast<uint32_t>(SEGGER_OPEN_Program),
    reinterpret_cast<uint32_t>(UNIFORM_ERASE_FUNC),
    reinterpret_cast<uint32_t>(nullptr), // SEGGER_OPEN_Start for turbo mode
    reinterpret_cast<uint32_t>(RUNTIME_SECTORS_FUNC),
};

void __attribute__ ((noinline)) FeedWatchdog(void) {
    // TODO: implement something to keep the watchdog happy
    return;
}

int __attribute__ ((noinline)) Init(const uint32_t address, const uint32_t frequency, const uint32_t function) {
    // TODO: implement init

    return 0;
}

int __attribute__ ((noinline)) UnInit(const uint32_t function) {
    // TODO: implement uninit

    return 0;
}

int __attribute__ ((noinline)) EraseSector(const uint32_t sector_address) {
    // TODO: implement sector erase

    return 0;
}

int __attribute__ ((noinline)) ProgramPage(const uint32_t address, const uint32_t size, const uint8_t *const data) {
    // TODO: implement program page

    return 0;
}

int __attribute__ ((noinline)) SEGGER_OPEN_Program(uint32_t address, uint32_t size, uint8_t *data) {
    // get the amount of pages to write
    const uint32_t pages = size >> PAGE_SIZE_SHIFT;

    for (uint32_t i = 0; i < pages; i++) {
        // program a page
        int r = ProgramPage(address, (0x1 << PAGE_SIZE_SHIFT), data);

        // check if something went wrong
        if (r) {
            // return a error
            return 1;
        }

        address += (0x1 << PAGE_SIZE_SHIFT);
        data += (0x1 << PAGE_SIZE_SHIFT);
    }

    // return everything went oke
    return 0;
}

#if CHIP_ERASE == true
    int __attribute__ ((noinline)) EraseChip(void) {
        // TODO: implement chip erase
        return 0;
    }
#endif

#if UNIFORM_SECTORS
    int __attribute__ ((noinline)) SEGGER_OPEN_Erase(uint32_t SectorAddr, uint32_t SectorIndex, uint32_t NumSectors) {
        // feed the watchdog
        FeedWatchdog();

        for (uint32_t i = 0; i < NumSectors; i++) {
            // erase a sector
            int r = EraseSector(SectorAddr);

            // check for errors
            if (r) {
                // return we have a error
                return 1;
            }

            // go to the next sector address
            SectorAddr += (1 << SECTOR_SIZE_SHIFT);
        }

        // return everything went oke
        return 0;
    }
#endif

#if CUSTOM_VERIFY
    uint32_t __attribute__ ((noinline, __used__)) Verify(uint32_t Addr, uint32_t NumBytes, uint8_t *pBuff) {
        // TODO: implement verify

        return (Addr + NumBytes);
    }
#endif

#if !NATIVE_READ
    int __attribute__ ((noinline, __used__)) BlankCheck(const uint32_t address, const uint32_t size, const uint8_t blank_value) {
        // TODO: implement blank check

        return 0;
    }

    int __attribute__ ((noinline, __used__)) SEGGER_OPEN_Read(const uint32_t address, const uint32_t size, uint8_t *const data) {
        // TODO: add read implementation

        return size;
    }
#endif

#if RUNTIME_SECTORS
    int __attribute__ ((noinline, __used__)) SEGGER_OPEN_GetFlashInfo(flash_info *const info, uint32_t InfoAreaSize) {
        // set the sector count (max is 7)
        info->count = max_info_sectors;

        // set the flash data
        for (uint32_t i = 0; i < info->count; i++) {
            // set every sector (we need to update every sector 
            // we want to use as the data is not filled in)
            info->sectors[i] = {
                // set the start offset for the current sector
                .offset = i * 0x100,
                
                // set the sector size
                .size = 0x100,

                // set the amount of sectors in the section
                .amount = 10,
            };
        }

        return 0;
    }
#endif
