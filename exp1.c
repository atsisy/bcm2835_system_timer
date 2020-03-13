#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <time.h>
#include <stdint.h>

struct bcm2835_timer_regs {
    uint32_t cs;
    uint32_t clo;
    uint32_t chi;
    uint32_t c0;
    uint32_t c1;
    uint32_t c2;
    uint32_t c3;
};

struct cfp_data {
        uint64_t bcm2825;
        uint64_t core_timer;
};

inline uint64_t bcm2825_get_count(struct bcm2835_timer_regs *regs)
{
        return (uint64_t)((((uint64_t)regs->chi) << 32) | regs->clo);
}

struct bcm2835_timer_regs *get_bcm2835_timer(int fd)
{
        uint64_t ret;
        struct bcm2835_timer_regs *address;

        address = (struct bcm2835_timer_regs *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x3F003000);
        
        if (address == MAP_FAILED) {
                perror("mmap");
                close(fd);
                exit(-1);
        }
        
        return address;
}

inline uint64_t core_timer_get_count(uint64_t *p)
{
        return *p;
}

uint64_t *get_core_timer(int fd)
{
        void *address;
        uint64_t count;

        address = (void *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x40000000);
        if (address == MAP_FAILED) {
                perror("mmap");
                close(fd);
                exit(-1);
        }

        return (uint64_t *)(address + 0x1c);
}


int main(void)
{
        int fd;
        struct bcm2835_timer_regs *bcm_regs;
        uint64_t *core_timer;

        if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
                perror("open");
                return -1;
        }

        bcm_regs = get_bcm2835_timer(fd);
        core_timer = get_core_timer(fd);
        

        struct timespec ts = {
                .tv_sec = 1,
                .tv_nsec = 0,
        };
        struct cfp_data data;

        while(1){
                data.bcm2825 = bcm2825_get_count(bcm_regs);
                data.core_timer = core_timer_get_count(core_timer);
                
                nanosleep(&ts, NULL);
                data.bcm2825 = bcm2825_get_count(bcm_regs) - data.bcm2825;
                data.core_timer = core_timer_get_count(core_timer) - data.core_timer;
                printf("%llu %llu\n", data.bcm2825, data.core_timer);
        }


        return 0;
}

