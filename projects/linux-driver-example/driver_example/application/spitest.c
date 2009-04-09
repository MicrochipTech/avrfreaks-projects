#include <sys/ioctl.h>
#include <stdio.h>


int main(int argc, char *args[])
{
  int device_node;

  device_node = open("/dev/spitest", 2);

  if(device_node == -1)
  {
    printf("Error: Could not open /dev/spitest\n"); 
    fflush(stdout);
  }

  /* Test ioctl call */
  ioctl(device_node, 5, 15);
  
  close(device_node);

  return 0;
}


