#include <sys/types.h>
#include <sys/ioctl.h>

#include <err.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "race_ioctl.h"

static enum {UNSET, ATTACH, DETACH, QUERY, LIST} action = UNSET;

/*
 * The usage statement: race_config -a | -d unit | -q unit | -l
 */

static void
usage()
{
	/*
	 * Arguments for this program are "either-or." For example,
	 * 'race_config -a' or 'race_config -d unit' are valid; however
	 * 'race_config -a -d unit' is invalid.
	 */

	fprintf(stderr, "usage: race_config -a | -d unit | -q unit | -l\n");
	exit(1);
}

/*
 * This program manages the doubly linked list found in /dev/race. It
 * allows you to add or remove an item, query the existence of an item,
 * or print every item in the list.
 */

int
main(int argc, char *argv[])
{
	int ch, fd, i, unit;
	char *p;

	/*
	 * Parse the command line argument list to determine
	 * the correct course of action.
	 *
	 *  -a:       add an item.
	 *  -d unit:  detach an item.
	 *  -q unit:  query the existence of an item.
	 *  -l:       list every item.
	 */

	while ((ch = getopt(argc, argv, "ad:q:l")) != -1)
		switch (ch) {
			case 'a':
				if (action != UNSET)
					usage();
				action = ATTACH;
				break;
			case 'd':
				if (action != UNSET)
					usage();
				action = DETACH;
				unit = (int)strtol(optarg, &p, 10);
				if (*p)
					errx(1, "illegal unit -- %s", optarg);
				break;
			case 'q':
				if (action != UNSET)
					usage();
				action = QUERY;
				unit = (int)strtol(optarg, &p, 10);
				if (*p)
					errx(1, "illegal unit -- %s", optarg);
				break;
			case 'l':
				if (action != UNSET)
					usage();
				action = LIST;
				break;
			default:
				usage();
		}

	/*
	 * Perform the chosen action.
	 */

	if (action == ATTACH) {
		fd = open("/dev/" RACE_NAME, O_RDWR);
		if (fd < 0)
			err(1, "open(/dev/%s)", RACE_NAME);

		i = ioctl(fd, RACE_IOC_ATTACH, &unit);
		if (i < 0)
			err(1, "ioctl(/dev/%s)", RACE_NAME);

		printf("unit: %d\n", unit);

		close(fd);
	} else if (action == DETACH) {
		fd = open("/dev/" RACE_NAME, O_RDWR);
		if (fd < 0)
			err(1, "open(/dev/%s)", RACE_NAME);

		i = ioctl(fd, RACE_IOC_DETACH, &unit);
		if (i < 0)
			err(1, "ioctl(/dev/%s)", RACE_NAME);

		close(fd);
	} else if (action == QUERY) {
		fd = open("/dev/" RACE_NAME, O_RDWR);
		if (fd < 0)
			err(1, "open(/dev/%s)", RACE_NAME);

		i = ioctl(fd, RACE_IOC_QUERY, &unit);
		if (i < 0)
			err(1, "ioctl(/dev/%s)", RACE_NAME);

		close(fd);
	} else if (action == LIST) {
		fd = open("/dev/" RACE_NAME, O_RDWR);
		if (fd < 0)
			err(1, "open(/dev/%s)", RACE_NAME);

		i = ioctl(fd, RACE_IOC_LIST, NULL);
		if (i < 0)
			err(1, "ioctl(/dev/%s)", RACE_NAME);

		close(fd);
	} else
		usage();

	return (0);
}
