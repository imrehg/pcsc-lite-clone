#include <pcscdefines.h>
#include <usbserial.h>



void main() {

  unsigned char s[500];
  int rv;
  unsigned long int len;
  int i;

  rv = OpenUSB(0);

  printf("Open Return %d\n", rv == STATUS_SUCCESS);

  s[0] = 0x90; s[1] = 0x00; s[2] = 0x00;
  s[3] = 0x00; s[4] = 0x00;  s[5] = 0x00;

  rv = WriteUSB(0, 5, s);

  printf("Write Return  %d\n", rv == STATUS_SUCCESS);

  s[0] = 0x83; s[1] = 0x00; s[2] = 0x00;
  s[3] = 0x00; s[4] = 0x00;  s[5] = 0x00;

  len = 69;

  rv = ReadUSB(0, &len, s);


  printf("Read Return  %d\n", rv == STATUS_SUCCESS);


  for (i=0; i < 10; i++) {
    printf("%x ", s[5+i]);
  } printf("\n");
  





}
