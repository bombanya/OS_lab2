#include <stdio.h>
#include <string.h>

#include "kernel_data_structures.h"

int main(){
    char input_command[100];
    unsigned char select_bus;
    int select_domain;
    unsigned int select_devfn;

    struct bus_data bus;
    struct device_coordinates coords;
    struct device_data device;
    FILE* file;

    while (1){
        printf("%s\n", "input command:");
        fgets(input_command, 100, stdin);
        input_command[strcspn(input_command, "\n")] = 0;

        if (strcmp(input_command, "buses") == 0){
            printf("\nAll buses in the system:\n");
            file = fopen("/sys/kernel/debug/laba_pci/buses", "rb");
            while (fread(&bus, sizeof(struct bus_data), 1, file)){
                printf("Bus:\nDomain: %d\nBus number: %hhu\n\n", bus.domain_nr, bus.number);
            }
            fclose(file);
        }
        else if (strcmp(input_command, "devices") == 0){
            scanf("%hhu", &select_bus);
            fgets(input_command, 100, stdin);
            file = fopen("/sys/kernel/debug/laba_pci/select_bus", "w");
            fprintf(file, "%hhu", select_bus);
            fclose(file);

            file = fopen("/sys/kernel/debug/laba_pci/bus_devices", "rb");
            while (fread(&coords, sizeof(struct device_coordinates), 1, file)){
                printf("\nDevice: %d:%hhu:%u (device: %u, function: %u)\n\n",
                       coords.domain_nr, coords.bus_number, coords.devfn, (coords.devfn) >> 3,
                       (coords.devfn) & 0x7);
            }
            fclose(file);
        }
        else if (strcmp(input_command, "device") == 0){
            scanf("%d:%hhu:%u", &select_domain, &select_bus, &select_devfn);
            fgets(input_command, 100, stdin);
            file = fopen("/sys/kernel/debug/laba_pci/select_device_bus", "w");
            fprintf(file, "%hhu", select_bus);
            fclose(file);
            file = fopen("/sys/kernel/debug/laba_pci/select_device_devfn", "w");
            fprintf(file, "%u", select_devfn);
            fclose(file);

            file = fopen("/sys/kernel/debug/laba_pci/device", "rb");
            fread(&device, sizeof(struct device_data), 1, file);
            fclose(file);
            if (device.domain_nr == -1) printf("\nThere is no such device\n");
            else {
                printf("\nDevice %d:%hhu:%u (device: %u, function: %u) data:\nclass: %u"
                       " (%x %x %x)\nvendor id: %hx\ndevice id: %hx\n",
                       device.domain_nr, device.bus_number, device.devfn,
                       (device.devfn) >> 3, (device.devfn) & 0x7, device.class,
                       (device.class >> 16) & 0xFF, (device.class >> 8) & 0xFF, (device.class) & 0xFF,
                       device.vendor, device.device);
                printf("device path through buses: ");
                file = fopen("/sys/kernel/debug/laba_pci/device_path", "rb");
                while(fread(&bus, sizeof(struct bus_data), 1, file)){
                    printf(" <- %d:%hhu", bus.domain_nr, bus.number);
                }
                printf("\n");
                fclose(file);
            }
        }
        else if (strcmp(input_command, "exit") == 0) break;
        printf("\n-------\n");
    }

	return 0;
}
