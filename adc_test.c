#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

static int readChannel(int fd, int channel, uint32_t speed) {
    // tx[0] = 0  0  0  0  0  1  1  D2
    // tx[1] = D1 D0 ?  ?  ?  ?  ?  ?
    // tx[2] = 0  0  0  0  0  0  0  0

    uint8_t tx[3] = {
        (uint8_t) (0x06 | ((channel & 0x04) >> 2)),
        (uint8_t) ((channel & 0x03) << 6),
        (uint8_t) 0x00
    };
    
    uint8_t rx[3] = {0};

    struct spi_ioc_transfer tr = {0};
    tr.tx_buf        = (unsigned long)tx; // where the outgoing bytes are
    tr.rx_buf        = (unsigned long)rx; // where to put incoming bytes
    tr.len           = 3;                 // how many bytes
    tr.speed_hz      = speed;             // clock rate for this transfer
    tr.bits_per_word = 8;                 // byte-sized chunks

    if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 1) {
        perror("ioctl SPI_IOC_MESSAGE");
        return -1;
    }
    
    return ((rx[1] & 0x0F) << 8) | rx[2];
}

int main (void) {

    int fd = open("/dev/spidev0.0", O_RDWR);
    if (fd < 0) {
        perror("open /dev/spidev0.0");
        return 1;
    }

    printf("Opened SPI device, fd = %d\n", fd);

    uint8_t mode = 0;
    uint8_t bits = 8;
    uint32_t speed = 250000;


    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) == -1) {
        perror("ioctl SPI_IOC_WR_MODE");
        close(fd);
        return 1;
    }

    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) {
        perror("ioctl SPI_IOC_ BITS_PER_WORD");
        close(fd);
        return 1;
    }

    if(ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) {
        perror("ioctl SPI_IOC_WR_MAX_SPEED_HZ");
        close(fd);
        return 1;
    }

    printf("SPI configured: mode %d, %d bits, %u Hz\n", mode, bits, speed);

    while(1) {
        int ch0 = readChannel(fd, 0, speed);
        int ch1 = readChannel(fd, 1, speed);
        printf("CH0 = %4d, CH1 = %4d\n", ch0, ch1);
        usleep(100000);
    }

    close(fd);
    return 0;
}