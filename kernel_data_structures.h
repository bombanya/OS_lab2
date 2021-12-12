struct bus_data {
    unsigned char number;
    int domain_nr;
};

struct device_coordinates{
    int domain_nr;
    unsigned char bus_number;
    unsigned int devfn;
};

struct device_data{
    int domain_nr;
    unsigned char bus_number;
    unsigned int devfn;
    unsigned int class;
    unsigned short vendor;
    unsigned short device;
};

struct bpf_redirect_data {
    unsigned int flags;
    unsigned int tgt_index;
    void *tgt_value;
    void *map;
    unsigned int kern_flags;
    unsigned int nh_family;
};