#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)                                                   \
  ((byte) & 0x10 ? '1' : '0'), ((byte) & 0x08 ? '1' : '0'),                    \
      ((byte) & 0x04 ? '1' : '0'), ((byte) & 0x02 ? '1' : '0'),                \
      ((byte) & 0x01 ? '1' : '0')

int main(int argc, char *argv[]) {
  int serial_port = open(argv[1], O_RDWR);
  if (serial_port < 0) {
    printf("Error %i from open: %s\n", errno, strerror(errno));
  }

  struct termios tty;

  if (tcgetattr(serial_port, &tty) != 0) {
    printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
    return 1;
  }

  tty.c_cflag &= ~PARENB;
  tty.c_cflag &= ~CSTOPB;

  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;
  tty.c_cflag &= ~CRTSCTS;
  tty.c_cflag |= CREAD | CLOCAL;

  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ECHO;
  tty.c_lflag &= ~ECHOE;
  tty.c_lflag &= ~ECHONL;
  tty.c_lflag &= ~ISIG;
  tty.c_iflag &= ~(IXON | IXOFF | IXANY);
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

  tty.c_oflag &= ~OPOST;

  tty.c_oflag &= ~ONLCR;

  tty.c_cc[VTIME] = 10;

  tty.c_cc[VMIN] = 0;

  cfsetispeed(&tty, B9600);
  cfsetospeed(&tty, B9600);

  if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    return 1;
  }

  unsigned char read_buf[256];

  for (;;) {
    int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));

    if (num_bytes < 0) {
      printf("Error reading: %s", strerror(errno));
      return 1;
    }

    printf(BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(read_buf[0]));
  }

  close(serial_port);
  return 0;
};
