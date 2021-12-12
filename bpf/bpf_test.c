#include "vmlinux.h"
#include <bpf/bpf_helpers.h>

SEC("xdp/simple")
int simple(void *ctx)
{
    bpf_printk("alive\n");
    bpf_redirect(2, 0);
    return XDP_PASS;
}

char LICENSE[] SEC("license") = "GPL";
