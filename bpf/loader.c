#include <linux/if_link.h>
#include <err.h>
#include <unistd.h>
#include "bpf_test.skel.h"

int main(int argc, char **argv)
{
    __u32 flags = XDP_FLAGS_SKB_MODE;
    struct bpf_test *obj;

    obj = bpf_test__open_and_load();
    if (!obj)
        err(1, "failed to open and/or load BPF object\n");

    bpf_set_link_xdp_fd(1, -1, flags);
    bpf_set_link_xdp_fd(1, bpf_program__fd(obj->progs.simple), flags);

cleanup:
    bpf_test__destroy(obj);
}
