# Open Flash loader template (OFL)
Flash loader template without the need for a Keil/Segger DSK license. OFL uses Cmake and arm-none-eabi to build. OFL Uses parts of [klib](https://github.com/itzandroidtab/klib/)

## Create a Flash loader executable
To create a OFL executable you need the following:
* A driver for the peripheral the memory is connected to
* A driver to communicate with the flash memory
* Way to feed the watchdog if enabled
* A way to restore modified registers after deinit

## Stack usage
In the previous documentation Segger reserved 256 bytes of stack for Open flash loaders. In newer versions the documentation mention 512 bytes of stack with a fallback to 256 bytes for devices with low amounts of memory (debugging linkerscript is set to 256 bytes of stack). 

## Create a flash loader xml file
SEGGER J-Link uses xml files to add support for external loaders. The xml file configures the follwing:
* The target Microcontroller for the loader
* The address the memory is located
* What executable file to load
* The name it shows up as in the J-Link software

Example xml file for the LPC1756 is25lq040b open flash loader.
```xml
<DataBase>
    <Device>
        <ChipInfo Vendor="NXP" Name="LPC1756" />
        <FlashBankInfo Name="NOR Flash" BaseAddr="0xA0000000">
            <LoaderInfo Name="SPI flash loader" MaxSize="0x01000000" Loader="flash_loader.elf" LoaderType="FLASH_ALGO_TYPE_OPEN" />
        </FlashBankInfo>
    </Device>
</DataBase>
```

More info can be found at https://wiki.segger.com/J-Link_Device_Support_Kit

## Installing your open flash loader
There are 2 ways to install your open flash loader
1. Manually copying the executable and xml to `%appdata%/SEGGER/JlinkDevices/vendor/device_family`
2. Using a installer (like [open flashloader installer](https://github.com/itzandroidtab/open_flashloader_installer))

## Example
A example is available for the [LPC1756 and is25lq040b](https://github.com/itzandroidtab/ofl_lpc1756_is25lq040b). Made using the drivers from: [klib](https://github.com/itzandroidtab/klib).
