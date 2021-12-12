#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/pci.h>

#include "kernel_data_structures.h"

MODULE_LICENSE("GPL");

static struct dentry* dir;

static struct bus_data* bus_array;
static size_t bus_number = 0;
static struct debugfs_blob_wrapper bus_blob;

static struct device_coordinates* devices_on_bus_array;
static size_t devices_on_bus_number = 0;
static struct debugfs_blob_wrapper devices_on_bus_wrapper;

static struct device_data device;
static struct debugfs_blob_wrapper device_wrapper;

static struct bus_data* device_path_array;
static size_t device_path_len = 0;
static struct debugfs_blob_wrapper device_path_wrapper;

static int select_device_bus = 0;
static unsigned int select_device_devfn = 0;

static void find_buses(void){
    struct pci_bus* bus = NULL;

    while ((bus = pci_find_next_bus(bus))) bus_number++;
    bus_array = kmalloc(bus_number * sizeof(struct bus_data), GFP_KERNEL);

    size_t i = 0;
    while ((bus = pci_find_next_bus(bus))){
        bus_array[i].number = bus->number;
        bus_array[i].domain_nr = 0;
        i++;
    }
    bus_blob.data = bus_array;
    bus_blob.size = bus_number * sizeof(struct bus_data);
    debugfs_create_blob("buses", 0444, dir, &bus_blob);
}

static void find_bus_devices(int busnr){
    kfree(devices_on_bus_array);
    devices_on_bus_array = NULL;
    devices_on_bus_number = 0;

    struct pci_bus* bus = pci_find_bus(0, busnr);
    if (bus){
        struct pci_dev* dev;
        list_for_each_entry(dev, &bus->devices, bus_list){
            devices_on_bus_number++;
        }
        devices_on_bus_array = kmalloc(devices_on_bus_number * sizeof(struct device_coordinates), GFP_KERNEL);
        size_t i = 0;
        list_for_each_entry(dev, &bus->devices, bus_list){
            devices_on_bus_array[i].bus_number = bus->number;
            devices_on_bus_array[i].domain_nr = 0;
            devices_on_bus_array[i].devfn = dev->devfn;
            i++;
        }
    }
    devices_on_bus_wrapper.size = devices_on_bus_number * sizeof(struct device_coordinates);
    devices_on_bus_wrapper.data = devices_on_bus_array;
}

static void find_device(int busnr, unsigned int devfn){
    kfree(device_path_array);
    device_path_len = 0;

    struct pci_dev* devp;
    devp = pci_get_domain_bus_and_slot(0, busnr, devfn);
    if (devp){
        device.domain_nr = 0;
        device.bus_number = busnr;
        device.devfn = devp->devfn;
        device.class = devp->class;
        device.vendor = devp->vendor;
        device.device = devp->device;

        struct pci_bus* bus = devp->bus;
        while (bus){
            device_path_len++;
            bus = bus->parent;
        }
        //printk(KERN_INFO "%zu", device_path_len);
        device_path_array = kmalloc(device_path_len * sizeof(struct bus_data), GFP_KERNEL);
        size_t i = 0;
        bus = devp->bus;
        while (bus){
            device_path_array[i].domain_nr = 0;
            device_path_array[i].number = bus->number;
            i++;
            bus = bus->parent;
        }
        pci_dev_put(devp);
    }
    else device.domain_nr = -1;

    device_path_wrapper.data = device_path_array;
    device_path_wrapper.size = device_path_len * sizeof(struct bus_data);
}

static int select_bus_write_op(void *data, u64 value){
    find_bus_devices(value);
    return 0;
}

static int select_device_bus_write_op(void *data, u64 value){
    select_device_bus = value;
    find_device(select_device_bus, select_device_devfn);
    return 0;
}

static int select_device_devfn_write_op(void *data, u64 value){
    select_device_devfn = value;
    find_device(select_device_bus, select_device_devfn);
    return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(select_bus_ops, NULL, select_bus_write_op, "%lld\n");
DEFINE_SIMPLE_ATTRIBUTE(select_device_bus_ops, NULL, select_device_bus_write_op, "%lld\n");
DEFINE_SIMPLE_ATTRIBUTE(select_device_devfn_ops, NULL, select_device_devfn_write_op, "%lld\n");

static int __init my_module_init(void) {
    printk(KERN_INFO "init\n");
    dir = debugfs_create_dir("laba_pci", NULL);
    find_buses();

    debugfs_create_blob("bus_devices", 0444, dir, &devices_on_bus_wrapper);
    debugfs_create_file("select_bus", 0222, dir, NULL, &select_bus_ops);

    device_wrapper.data = &device;
    device_wrapper.size = sizeof(struct device_data);
    debugfs_create_blob("device", 0444, dir, &device_wrapper);
    debugfs_create_blob("device_path", 0444, dir, &device_path_wrapper);
    debugfs_create_file("select_device_bus", 0222, dir, NULL, &select_device_bus_ops);
    debugfs_create_file("select_device_devfn", 0222, dir, NULL, &select_device_devfn_ops);

    return 0;
}

static void __exit my_module_exit(void){
    kfree(devices_on_bus_array);
    kfree(bus_array);
    kfree(device_path_array);
    debugfs_remove_recursive(dir);
    printk(KERN_INFO "dead\n");
}

module_init(my_module_init);
module_exit(my_module_exit);