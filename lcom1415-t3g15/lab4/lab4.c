#include <minix/drivers.h>

#include "i8042.h"
#include "test4.h"

static int proc_args(int argc, char *argv[]);
static unsigned short parse_ushort(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv) {

	sef_startup();

	if ( argc == 1 ) {
		print_usage(argv);
		return 0;
	} else {
		proc_args(argc, argv);
	}
	return 0;

}

static void print_usage(char *argv[]) {
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"packet <unsigned short cnt>\" \n"
			"\t service run %s -args \"async <unsigned short idle_time>\" \n"
			"\t service run %s -args \"config \" \n",
			"\t service run %s -args \"gesture <short length, unsigned short tolerance>\" \n",
			argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {

	unsigned short cnt, tolerance, i_time;
	short length = 0;
	/* check the function to test: if the first characters match, accept it */
	if (strncmp(argv[1], "packet", strlen("packet")) == 0) {
		if( argc != 3 ) {
			printf("mouse: wrong no of arguments for test of test_packet() \n");
			return 1;
		}
		if( (cnt = parse_ushort(argv[2], 10)) == USHRT_MAX )
			return 1;
		printf("mouse: test_packet()\n");
		test_packet(cnt);
		return 0;
	} else if (strncmp(argv[1], "async", strlen("async")) == 0) {
		if( argc != 3 ) {
			printf("mouse: wrong no of arguments for test of test_async() \n");
			return 1;
		}
		if( (i_time = parse_ushort(argv[2], 10)) == USHRT_MAX )
			return 1;
		printf("mouse: test_async()\n");
		test_async(i_time);
		return 0;
	} else if (strncmp(argv[1], "config", strlen("config")) == 0) {
		if( argc != 2 ) {
			printf("mouse: wrong no of arguments for test of test_config() \n");
			return 1;
		}
		printf("mouse: test_config\n");
		test_config();
		return 0;
	} else if (strncmp(argv[1], "gesture", strlen("gesture")) == 0) {
			if( argc != 4 ) {
				printf("mouse: wrong no of arguments for test of test_gesture() \n");
				return 1;
			}
			length = parse_ushort(argv[2], 10);
			if( (tolerance = parse_ushort(argv[3], 10)) == USHRT_MAX )
				return 1;
			printf("mouse: test_gesture()\n");
			test_gesture(length, tolerance);
			return 0;
	} else {
		printf("mouse: non valid function \"%s\" to test\n", argv[1]);
		return 1;
	}
}

static unsigned short parse_ushort(char *str, int base) {
	char *endptr;
	unsigned short val;

	val = strtoul(str, &endptr, base);

	if ((errno == ERANGE && val == USHRT_MAX )
			|| (errno != 0 && val == 0)) {
		perror("strtol");
		return USHRT_MAX;
	}

	if (endptr == str) {
		printf("kbd: parse_ushort: no digits were found in %s \n", str);
		return USHRT_MAX;
	}

	/* Successful conversion */
	return val;
}
