
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <string>

#include <linux/joystick.h>
#include <vector>

#define NAME_LENGTH 128

int main (int argc, char **argv)
{
	int fd;
	unsigned char axes = 2;
	unsigned char buttons = 2;
	int version = 0x000800;
	char name[NAME_LENGTH] = "Unknown";

	if ((fd = open("/dev/input/js1", O_RDONLY|O_NONBLOCK)) < 0) {
		exit(1);
	}

	ioctl(fd, JSIOCGVERSION, &version);
	ioctl(fd, JSIOCGAXES, &axes);
	ioctl(fd, JSIOCGBUTTONS, &buttons);
	ioctl(fd, JSIOCGNAME(NAME_LENGTH), name);

	printf("Joystick (%s) has %d axes and %d buttons. Driver version is %d.%d.%d.\n",
		name, axes, buttons, version >> 16, (version >> 8) & 0xff, version & 0xff);
	printf("Testing ... (interrupt to exit)\n");

	struct js_event js;
	fcntl(fd, F_SETFL, O_NONBLOCK);

	struct Value {
		short value;
		time_t time;
		Value() : value(0), time(0) {}
	};

	std::vector<Value> axesVals;
	std::vector<Value> buttonVals;

	axesVals.resize(axes);
	buttonVals.resize(buttons);

	while (1) {

		size_t readSize = 0;
		while ((readSize = read(fd, &js, sizeof(struct js_event))) == sizeof(struct js_event))  {
			auto e = errno;
			if (e && e != EAGAIN) {
				perror("\njstest: error reading");
				exit (1);
			}

			if (js.type & JS_EVENT_INIT) {
				if(js.type & JS_EVENT_AXIS) {
					if(axesVals[js.number].time < js.time) {
						axesVals[js.number].value = js.value;
						axesVals[js.number].time = js.time;
					}
				}
				if (js.type & JS_EVENT_BUTTON) {
					if(buttonVals[js.number].time < js.time) {
						buttonVals[js.number].value = js.value;
						buttonVals[js.number].time = js.time;
					}
				}
			} else {
				bool print = false;
				if(js.type & JS_EVENT_AXIS) {
					if(axesVals[js.number].time < js.time) {
						if(axesVals[js.number].value != js.value) {
							axesVals[js.number].value = js.value;
							print = true;
						}
					}
				}
				if (js.type & JS_EVENT_BUTTON) {
					if(buttonVals[js.number].time < js.time) {
						if(buttonVals[js.number].value != js.value) {
							buttonVals[js.number].value = js.value;
							print = true;
						}
					}
				}

				if (print)
					printf("Event: type %d, time %d, number %d, value %d\n",
							js.type, js.time, js.number, js.value);
			}
		}
	}

	return 0;
}

