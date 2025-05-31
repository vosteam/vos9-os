#ifndef SATA_H
#define SATA_H

#include "../kernel.h"

// AHCI (Advanced Host Controller Interface) constants
#define AHCI_BASE           0x400000
#define SATA_ATA            0x00000101
#define SATA_ATAPI          0xEB140101
#define SATA_SEMB           0xC33C0101
#define SATA_PM             0x96690101

// AHCI registers
#define HBA_CAP             0x00
#define HBA_GHC             0x04
#define HBA_IS              0x08
#define HBA_PI              0x0C
#define HBA_VS              0x10

// Port registers
#define PORT_CLB            0x00
#define PORT_CLBU           0x04
#define PORT_FB             0x08
#define PORT_FBU            0x0C
#define PORT_IS             0x10
#define PORT_IE             0x14
#define PORT_CMD            0x18
#define PORT_TFD            0x20
#define PORT_SIG            0x24
#define PORT_SSTS           0x28
#define PORT_SCTL           0x2C
#define PORT_SERR           0x30
#define PORT_SACT           0x34
#define PORT_CI             0x38

// ATA commands
#define ATA_CMD_READ_DMA    0xC8
#define ATA_CMD_WRITE_DMA   0xCA
#define ATA_CMD_IDENTIFY    0xEC

// SATA device structure
typedef struct {
    int port;
    int present;
    char model[41];
    uint32_t sectors;
    uint32_t sector_size;
} sata_device_t;

// Function declarations
int sata_init(void);
int sata_detect_drives(void);
int sata_read_sectors(int drive, uint32_t lba, uint32_t count, void *buffer);
int sata_write_sectors(int drive, uint32_t lba, uint32_t count, const void *buffer);
int sata_get_drive_count(void);
sata_device_t* sata_get_drive(int index);

#endif
