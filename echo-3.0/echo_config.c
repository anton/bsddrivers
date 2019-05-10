#include <sys/types.h>
#include <sys/ioctl.h>

#include <err.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ECHO_CLEAR_BUFFER	_IO('E', 1)
#define ECHO_SET_BUFFER_SIZE	_IOW('E', 2, int)

static enum {UNSET, CLEAR, SETSIZE} action = UNSET;

/*
 * The usage statement: echo_config -c | -s size
 */

static void
usage()
{
	/*
	 * Arguments for this program are "either-or." That is,
	 * 'echo_config -c' and 'echo_config -s size' are valid; however,
	 * 'echo_config -c -s size' is invalid.
	 */

	fprintf(stderr, "usage: echo_config -c | -s size\n");
	exit(1);
}

/*
 * This program clears or resizes the memory buffer
 * found in /dev/echo.
 */

int
main(int argc, char *argv[])
{
	int ch, fd, i, size;
	char *p;

	/*
	 * Parse the command-line argument list to determine
	 * the correct course of action.
	 *
	 *   -c:	clear the memory buffer.
	 *   -s size:	resize the memory buffer to size.
	 */

	while ((ch = getopt(argc, argv, "cs:")) != -1)
		switch(ch) {
		case 'c':
			if (action != UNSET)
				usage();
			action = CLEAR;
			break;
		case 's':
			if (action != UNSET)
				usage();
			action = SETSIZE;
			size = (int)strtol(optarg, &p, 10);
			if (*p)
				errx(1, "illegal size -- %s", optarg);
			break;
		default:
			usage();
		}
	/*
	 * Perform the chose action
	 */

	if (action == CLEAR) {
		fd = open("/dev/echo", O_RDWR);
		if (fd < 0)
			err(1, "open(/dev/echo)");

		i = ioctl(fd, ECHO_CLEAR_BUFFER, NULL);
		if (i < 0)
			err(1, "ioctl(/dev/echo)");
		close(fd);
	} else if (action == SETSIZE) {
		fd = open("/dev/echo", O_RDWR);
		if (fd < 0)
			err(1, "open(/dev/echo)");
		i = ioctl(fd, ECHO_SET_BUFFER_SIZE, &size);
		if (i < 0)
			err(1, "ioctl(/dev/echo)");
		close(fd);
	} else
		usage();
	return 0;
}
