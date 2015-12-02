#include <minix/drivers.h>

#include "i8042.h"
#include "test3.h"

static int proc_args(int argc, char *argv[]);
static unsigned short parse_ushort(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv) {

	sef_startup();

	/* Enable IO-sensitive operations for ourselves */
	sys_enable_iop(SELF);

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
			"\t service run %s -args \"scan <unsigned short asm>\" \n"
			"\t service run %s -args \"leds <bits with a space between>\" \n"
			"\t service run %s -args \"timed_scan <unsigned short n>\" \n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {

	unsigned short n, ass, n_timed;
	unsigned short *toogle;
	unsigned short argnr;
	/* check the function to test: if the first characters match, accept it */
	if (strncmp(argv[1], "scan", strlen("scan")) == 0) {
		if( argc != 3 ) {
			printf("timer: wrong no of arguments for test of kbd_test_scan() \n");
			return 1;
		}
		if( (ass = parse_ushort(argv[2], 10)) == USHRT_MAX )
			return 1;
		printf("timer: kbd_test_scan\n");
		kbd_test_scan(ass);
		return 0;
	} else if (strncmp(argv[1], "leds", strlen("leds")) == 0) {

		argnr = (argc - 2);
		n = argnr;
		unsigned short * leds = (unsigned short *) malloc (n);
		unsigned short a = 2;
		unsigned short l = 0;
		for (; a<argc; a++, l++)
		{
			if (parse_ushort(argv[a], 10) > 2 || parse_ushort(argv[a], 10) < 0)
			{
				printf("error : non valid value for LEDS");
				free(leds);
				return 1;
			}
			else leds[l] = parse_ushort(argv[a], 10);
		}
		printf("timer: kbs_test_leds\n");
		kbd_test_leds(n, leds);
		free(leds);
		return 0;
	} else if (strncmp(argv[1], "timed_scan", strlen("timed_scan")) == 0) {
		if( argc != 3 ) {
			printf("timer: wrong no of arguments for test of timer_test_int() \n");
			return 1;
		}
		if( (n_timed = parse_ushort(argv[2], 10)) == USHRT_MAX )
			return 1;
		printf("timer: kbd_test_time_scan\n");
		kbd_test_timed_scan(n_timed);
		return 0;
	} else {
		printf("timer: non valid function \"%s\" to test\n", argv[1]);
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

