#include "../../include/drivers/sata.h"
#include "../../include/vga.h"
#include "../../include/string.h"

static sata_device_t sata_devices[8];
static int sata_device_count = 0;

// Simplified SATA initialization
int sata_init(void) {
    vga_puts("Initializing SATA controller...\n");
    
    // In a real implementation, we would:
    // 1. Scan PCI bus for AHCI controllers
    // 2. Map AHCI registers
    // 3. Initialize controller
    // 4. Detect connected drives
    
    // For now, simulate detection of a virtual drive
    sata_devices[0].port = 0;
    sata_devices[0].present = 1;
    strcpy(sata_devices[0].model, "Virtual SATA Drive");
    sata_devices[0].sectors = 1024 * 1024; // 512MB
    sata_devices[0].sector_size = 512;
    sata_device_count = 1;
    
    vga_puts("SATA: Found ");
    vga_put_dec(sata_device_count);
    vga_puts(" drive(s)\n");
    
    return sata_device_count > 0 ? 1 : 0;
}

int sata_detect_drives(void) {
    return sata_device_count;
}

int sata_read_sectors(int drive, uint32_t lba, uint32_t count, void *buffer) {
    if (drive >= sata_device_count || !sata_devices[drive].present) {
        return 0;
    }
    
    // Simulate successful read
    vga_puts("SATA: Reading ");
    vga_put_dec(count);
    vga_puts(" sectors from LBA ");
    vga_put_dec(lba);
    vga_puts("\n");
    
    // In real implementation, this would perform actual disk I/O
    return 1;
}

int sata_write_sectors(int drive, uint32_t lba, uint32_t count, const void *buffer) {
    if (drive >= sata_device_count || !sata_devices[drive].present) {
        return 0;
    }
    
    // Simulate successful write
    vga_puts("SATA: Writing ");
    vga_put_dec(count);
    vga_puts(" sectors to LBA ");
    vga_put_dec(lba);
    vga_puts("\n");
    
    return 1;
}

int sata_get_drive_count(void) {
    return sata_device_count;
}

sata_device_t* sata_get_drive(int index) {
    if (index >= 0 && index < sata_device_count) {
        return &sata_devices[index];
    }
    return NULL;
}
