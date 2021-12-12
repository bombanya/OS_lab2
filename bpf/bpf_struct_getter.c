#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/filter.h>

#include "kernel_data_structures.h"

MODULE_LICENSE("GPL");

static struct dentry* dir;
static struct bpf_redirect_data redirect_info_data;
static struct debugfs_blob_wrapper wrapper;

static int update_redirect_info(void *data, u64 value){
    struct bpf_redirect_info* ri = this_cpu_ptr(&bpf_redirect_info);
    redirect_info_data.flags = ri->flags;
    redirect_info_data.tgt_index = ri->tgt_index;
    redirect_info_data.tgt_value = ri->tgt_value;
    redirect_info_data.map = ri->map;
    redirect_info_data.kern_flags = ri->kern_flags;
    redirect_info_data.nh_family = ri->nh.nh_family;
    return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(update_redirect_info_ops, NULL, update_redirect_info, "%lld\n");

static int __init my_module_init(void) {
    printk(KERN_INFO "init\n");

    dir = debugfs_create_dir("laba_bpf", NULL);

    wrapper.data = &redirect_info_data;
    wrapper.size = sizeof(struct bpf_redirect_data);
    debugfs_create_blob("bpf_redirect_info",
                        0444, dir, &wrapper);
    debugfs_create_file("update_redirect_info",
                        0222, dir, NULL, &update_redirect_info_ops);
    return 0;
}

static void __exit my_module_exit(void){
    debugfs_remove_recursive(dir);
    printk(KERN_INFO "dead\n");
}

module_init(my_module_init);
module_exit(my_module_exit);