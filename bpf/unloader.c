#include <linux/if_link.h>
#include <err.h>
#include <unistd.h>
#include "bpf_test.skel.h"

int main(int argc, char **argv)
{
    __u32 flags = XDP_FLAGS_SKB_MODE;
    bpf_set_link_xdp_fd(1, -1, flags);
}
