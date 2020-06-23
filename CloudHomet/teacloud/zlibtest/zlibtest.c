#include <zlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    char *file = "./test.txt";
    char *gzip = "./test.gz";

    gzFile gf = gzopen(gzip, "wb");
    int fd = open(file, O_RDONLY);
    char buf[1024] = {0};
    int ret;
    while((ret = read(fd, buf, 1024)) > 0){
	gzwrite(gf, buf, ret);
    }
    close(fd);
    gzclose(gf);
    unlink(file);
    return 0;
}
