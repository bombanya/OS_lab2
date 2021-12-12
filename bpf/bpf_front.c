#include <stdio.h>

#include "kernel_data_structures.h"

int main(){
    struct bpf_redirect_data data;
    FILE* file;

    file = fopen("/sys/kernel/debug/laba_bpf/update_redirect_info", "w");
    fprintf(file, "%d", 0);
    fclose(file);

    file = fopen("/sys/kernel/debug/laba_bpf/bpf_redirect_info", "rb");
    fread(&data, sizeof(struct bpf_redirect_data), 1, file);
    fclose(file);

    printf("flags: %u\ntgt_index: %u\ntgt_value: %p\nmap: %p\nkern_flags: %u\nnh_family: %u\n",
           data.flags, data.tgt_index, data.tgt_value, data.map, data.kern_flags, data.nh_family);
}
