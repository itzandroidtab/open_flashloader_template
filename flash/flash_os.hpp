#ifndef FLASH_OS_HPP
#define FLASH_OS_HPP

#include <cstdint>

// driver version. Do not modify
constexpr static uint16_t flash_drv_version = 0x101;

// max amount of sectors in the flash device. Can be modified
// to allow more sectors in the flash device
constexpr static uint32_t max_sectors = 4;

/**
 * @brief Flash device types
 * 
 */
enum class device_type: uint8_t {
    unknown = 0,
    on_chip = 1,
    external_8_bit = 2,
    external_16_bit = 3,
    external_32_bit = 4,
    external_spi = 5
};

/**
 * @brief Information about a flash sector
 * 
 */
struct flash_sector {
    // Sector size in bytes
    uint32_t size;

    // Address offset on the base address
    uint32_t offset;
};

// end of the sector list. Must be present at the end of the sector list
constexpr static flash_sector end_of_sectors = {0xffffffff, 0xffffffff};

/**
 * @brief Information about the flash device
 * 
 */
struct flash_device {
    // version number
    uint16_t version;

    // name of the flash device.
    char name[128];

    // type of the flash device
    device_type type;

    // flash base address
    uint32_t base_address;

    // total flash size of the device
    uint32_t size;

    // programming page size
    uint32_t page_size;

    // reserved for future extension. should be 0
    uint32_t reserved;

    // value after erasing a sector/page (0xFF for most devices)
    uint8_t erase_value;

    // timeout to program a page in msec
    uint32_t programming_timeout;

    // timeout to erase a sector in msec
    uint32_t erase_timeout;

    // flash sector layout definition
    flash_sector sectors[max_sectors];
};

/**
 * @brief Extern C as the Segger application is only searching the 
 * elf for C functions. This prevents a error popup.
 * 
 */
extern "C" {
    /**
     * @brief Keil / CMSIS API
     * 
     */

    /**
     * @brief Initialize Flash Programming Functions
     * 
     * @warning Mandatory
     * 
     * @param Addr Address to init
     * @param Freq Clock frequency (Hz) 
     * @param Func function code. (1 - Erase, 2 = Program, 3 = Verify)
     * @return int 0 = OK, 1 = Failed
     */
    int Init(const uint32_t address, const uint32_t frequency, const uint32_t function);

    /**
     * @brief De-Initialize Flash Programming Functions
     * 
     * @warning Mandatory
     * 
     * @param Func function code. (1 - Erase, 2 = Program, 3 = Verify)
     * @return int 0 = OK, 1 = Failed
     */
    int UnInit(const uint32_t function);

    /**
     * @brief Erase Sector in Flash Memory
     * 
     * @warning Mandatory
     * 
     * @param Addr 
     * @return int 0 = OK, 1 = Failed
     */
    int EraseSector(const uint32_t sector_address);

    /**
     * @brief Program a page in flash memory
     * 
     * @warning Mandatory
     * 
     * @param Addr 
     * @param NumBytes 
     * @param pSrcBuff 
     * @return int 0 = OK, 1 = Failed
     */
    int ProgramPage(const uint32_t address, const uint32_t size, const uint8_t *const data);

    /**
     * @brief Checks if the flash is "Blank". Necessary if flash is not memory mapped
     * 
     * @param Addr 
     * @param NumBytes 
     * @param BlankData 
     * @return int 0 = OK, 1 = OK not blank, < 0 = Error
     */
    int BlankCheck(const uint32_t address, const uint32_t size, const uint8_t blank_data);

    /**
     * @brief Erase the complete device. When not provided erase sector is called for
     * every sector
     * 
     * @return int 0 = OK, 1 = Failed
     */
    int EraseChip(void);

    /**
     * @brief Verifies the memory. Necessary if flash is not memory mapped
     * 
     * @param Addr 
     * @param NumBytes 
     * @param pSrcBuff 
     * @return uint32_t == Address + Size = OK, != Address + Size = Failed
     */
    uint32_t Verify(uint32_t Addr, uint32_t NumBytes, uint8_t *pSrcBuff);

    /**
     * @brief Segger extensions
     * 
     */

    /**
     * @brief Feed the watchdog of the device
     * 
     */
    void FeedWatchdog();

    /**
     * @brief Read from memory. Necessary if flash is not memory mapped 
     * 
     * @param Addr 
     * @param NumBytes 
     * @param pDestBuff 
     * @return int < 0 = Error, >= 0 OK number of bytes read
     */
    int SEGGER_OPEN_Read(uint32_t Addr, uint32_t NumBytes, uint8_t *pDestBuff);

    /**
     * @brief Programs multiple pages at once in 1 ramcode call. Speeds up programming. 
     * 
     * @param DestAddr 
     * @param NumBytes 
     * @param pSrcBuff 
     * @return int 0 = OK, 1 = Failed
     */
    int SEGGER_OPEN_Program(uint32_t DestAddr, uint32_t NumBytes, uint8_t *pSrcBuff);

    /**
     * @brief Erases multiple pages at once. Only works when uniform pages are enabled
     * 
     * @param SectorAddr 
     * @param SectorIndex 
     * @param NumSectors 
     * @return int 0 = OK, 1 = Failed
     */
    int SEGGER_OPEN_Erase(uint32_t SectorAddr, uint32_t SectorIndex, uint32_t NumSectors);
}

#endif