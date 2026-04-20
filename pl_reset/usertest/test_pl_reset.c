#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char **argv)
{
    const char *path = "/sys/bus/platform/devices/amba_pl@0:pl_reset/reset";
    int fd;
    ssize_t n;

    if (argc > 1)
        path = argv[1];

    fd = open(path, O_WRONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    n = write(fd, "1", 1);
    if (n < 0) {
        perror("write");
        close(fd);
        return 1;
    }

    close(fd);
    printf("reset triggered: %s\n", path);
    return 0;
}
