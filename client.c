#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

struct bcm2835_system_timer {
	uint32_t cs;
	uint32_t clo;
	uint32_t chi;
	uint32_t c0;
	uint32_t c1;
	uint32_t c2;
	uint32_t c3;
};

int main(void)
{
	int fd;
	struct bcm2835_system_timer timer_info;

	fd = open("/sys/kernel/debug/bcm_system_timer", O_RDONLY);

	if(fd == -1){
		puts("Failed to open debug file.");
		return 0;
	}

	(void)read(fd, &timer_info, sizeof(struct bcm2835_system_timer));
	(void)close(fd);

	printf("%d %d\n", timer_info.chi, timer_info.clo);

	return 0;
}
