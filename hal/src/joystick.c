// implementation for hal/joystick.h

#include "hal/joystick.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

// file scope constants
#define SPI_DEVICE "/dev/spidev0.0"
#define SPI_SPEED_HZ 250000
#define CHANNEL_UPDOWN 0
#define CHANNEL_LEFTRIGHT 1
#define THRESHOLD_LOW 500 // LOW  = 500 => below this means UP (CH0) or LEFT (CH1)
#define THRESHOLD_HIGH 3600 // HIGH = 3600 => above this means DOWN (CH0) or RIGHT (CH1)

static int spi_fd = -1;
static bool is_initialized = false;

// helper
static int readChannel (int channel) {
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
    tr.tx_buf        = (unsigned long)tx; // where out going bytes are
    tr.rx_buf        = (unsigned long)rx; // where to put incoming bytes
    tr.len           = 3;                 // how many bytes
    tr.speed_hz      = SPI_SPEED_HZ;      // clock rate for transfer
    tr.bits_per_word = 8;                 // byte-sized chunk

    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr) < 1) {
        perror("ioctl SPI_IOC_MESSAGE");
        return -1;
    }

    return ((rx[1] & 0x0F) << 8) | rx[2];
}

void joystick_init(void) {
    assert(!is_initialized);

    uint8_t mode = 0;
    uint8_t bits = 8;
    uint32_t speed = SPI_SPEED_HZ;

    spi_fd = open(SPI_DEVICE, O_RDWR);
    if (spi_fd < 0) {
        fprintf(stderr, "Error opening SPI device: %s\n", SPI_DEVICE);
        perror("Reason");
        exit(EXIT_FAILURE);
    }

    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) == -1) {
        fprintf(stderr, "Error setting SPI mode on %s\n", SPI_DEVICE);
        perror("Reason");
        exit(EXIT_FAILURE);
    }

    if (ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) {
        fprintf(stderr, "Error setting bits per word on %s\n", SPI_DEVICE);
        perror("Reason");
        exit(EXIT_FAILURE);
    }

    if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) {
        fprintf(stderr, "Error setting max speed (Hz) on %s\n", SPI_DEVICE);
        perror("Reason");
        exit(EXIT_FAILURE);
    }

    is_initialized = true;
}

joystick_direction_t joystick_get_direction(void) {
    assert(is_initialized);

    int ch0 = readChannel(CHANNEL_UPDOWN);
    if (ch0 < 0) {
        return JOYSTICK_NONE;
    } else if (ch0 < THRESHOLD_LOW) {
        return JOYSTICK_UP;
    } else if(ch0 > THRESHOLD_HIGH) {
        return JOYSTICK_DOWN;
    } else {
        int ch1 = readChannel(CHANNEL_LEFTRIGHT);
        if (ch1 < 0) {
            return JOYSTICK_NONE;
        } else if (ch1 < THRESHOLD_LOW) {
            return JOYSTICK_LEFT;
        } else if (ch1 > THRESHOLD_HIGH){
            return JOYSTICK_RIGHT;
        } else {
            return JOYSTICK_NONE;
        }
    }
}

void joystick_cleanup(void) {
    assert(is_initialized);
    close (spi_fd);
    spi_fd = -1;
    is_initialized = false;
}