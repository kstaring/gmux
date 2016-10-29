#include <stdio.h> 
#include <fcntl.h> 
#include <errno.h> 
#include <stdlib.h> 
#include <string.h> 
#include <strings.h> 
#include <sys/types.h>
#include <sys/param.h>
#include <machine/cpufunc.h>

#define GMUX_BASE			0x700
#define GMUX_PORT_SWITCH_DISPLAY	(GMUX_BASE + 0x10)
#define GMUX_PORT_SWITCH_DDC	 	(GMUX_BASE + 0x28)
#define GMUX_PORT_SWITCH_SELECT		(GMUX_BASE + 0x40)
#define GMUX_PORT_DISCRETE_POWER	(GMUX_BASE + 0x50)
#define GMUX_PORT_MAX_BRIGHTNESS	(GMUX_BASE + 0x70)
#define GMUX_PORT_BRIGHTNESS		(GMUX_BASE + 0x74)

#define GMUX_INTEGRATED_GPU		0
#define GMUX_DISCRETE_GPU		1

char *cmd;

void gmux_set_brightness(int brightness)
{
  outb(GMUX_PORT_BRIGHTNESS, brightness);
  outb(GMUX_PORT_BRIGHTNESS + 1, brightness >> 8);
  outb(GMUX_PORT_BRIGHTNESS + 2, brightness >> 16);
  outb(GMUX_PORT_BRIGHTNESS + 3, 0);
}            

int gmux_get_max_brightness()
{
  return inl(GMUX_PORT_MAX_BRIGHTNESS) &  0x00ffffff;
}

int gmux_get_brightness()
{
  return inl(GMUX_PORT_BRIGHTNESS) &  0x00ffffff;
}

int gmux_get_gpu()
{
  return inl(GMUX_PORT_DISCRETE_POWER) & 0xff;
}

void gmux_set_gpu(int value)
{
  if (value == GMUX_INTEGRATED_GPU) {
    outb(GMUX_PORT_SWITCH_DDC, 1);
    outb(GMUX_PORT_SWITCH_DISPLAY, 2);
    outb(GMUX_PORT_SWITCH_SELECT, 2);
    outb(GMUX_PORT_DISCRETE_POWER, 0);
  } else {
    outb(GMUX_PORT_SWITCH_DDC, 2);
    outb(GMUX_PORT_SWITCH_DISPLAY, 3);
    outb(GMUX_PORT_SWITCH_SELECT, 3);
    outb(GMUX_PORT_DISCRETE_POWER, 1);
  }
}

int opt_gpu(char *val)
{
  if (!val) {
    printf("%s gpu %s\n", cmd, gmux_get_gpu() ? "dGPU" : "iGPU");
    return 0;
  } else {
    switch (val[0]) {
      case 'i':
        gmux_set_gpu(GMUX_INTEGRATED_GPU);
        break;
      case 'd':
        gmux_set_gpu(GMUX_DISCRETE_GPU);
        break;
      default:
        fprintf(stderr, "gpu %s: option not in (iGPU, dGPU)\n", val);
        return 1;
    }
  }

  return 0;
}

int opt_brightness(char *val)
{
  int cur_brightness;
  int max_brightness;
  int brightness;

  cur_brightness = gmux_get_brightness();
  max_brightness = gmux_get_max_brightness();
  
  if (!val) {
    printf("# max brightness = %d\n", max_brightness);
    printf("%s brightness %d\n", cmd, cur_brightness);

    return 0;
  }

  switch (val[0]) {
    case '+':
      brightness = cur_brightness + strtol(val + 1, NULL, 10);
      break;
    case '-':
      brightness = cur_brightness - strtol(val + 1, NULL, 10);
      break;
    default:
      brightness = strtol(val, NULL, 10);
      if (brightness == 0 && errno == EINVAL) {
        fprintf(stderr, "brightness '%s': not a number\n\n", val);
        return 1;
      }
      break;
  }
  brightness = MAX(brightness, 0);
  brightness = MIN(brightness, max_brightness);

  gmux_set_brightness(brightness);

  return 0;
}

int main(int argc, char **argv)
{
  char *opt;
  char *val = NULL;

  if (open("/dev/io", O_RDWR) < 0) {
    fprintf(stderr, "No IO permissions");
    return 1;
  }

  cmd = argv[0];

  if (argc == 1) {
    opt_gpu(NULL);
    opt_brightness(NULL);
    return 0;
  }
  if (argc == 2 || argc == 3) {
    opt = argv[1];
    if (argc == 3)
      val = argv[2];
    if (strncasecmp(opt, "brightness", 10) == 0)
      return opt_brightness(val);
    if (strncasecmp(opt, "gpu", 3) == 0)
      return opt_gpu(val);
  }

  fprintf(stderr, "Usage: %s [brightness|gpu] <value>\n\n", argv[0]);

  return 1;
}
