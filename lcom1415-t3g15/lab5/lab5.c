#include <minix/drivers.h>

#include "test5.h"
#include "video_gr.h"

static int proc_args(int argc, char *argv[]);
static unsigned short parse_ushort(char *str, int base);
static void print_usage(char *argv[]);
static unsigned long parse_ulong(char *str, int base);

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
			"\t service run %s -args \"init <unsigned short mode, unsigned short delay>\" \n",
			"\t service run %s -args \"square <unsigned short x, unsigned short y, unsigned short size, unsigned long color>\" \n",
			"\t service run %s -args \"line <unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, unsigned long color>\" \n",
			"\t service run %s -args \"xpm <unsigned short xi, unsigned short yi, char *xpm[]>\" \n",
			"\t service run %s -args \"move <unsigned short xi, unsigned short yi, char *xpm[], unsigned short hor, short delta, unsigned short time>\" \n",
			"\t service run %s -args \"controller \" \n",
			argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {

	unsigned short mode, delay, x, y, size, xi, yi, xf, yf, hor, time;
	unsigned long color;
	/* check the function to test: if the first characters match, accept it */
	if (strncmp(argv[1], "init", strlen("init")) == 0) {
		if( argc != 4 ) {
			printf("vg: wrong no of arguments for test of test_init() \n");
			return 1;
		}
		if( (mode = parse_ushort(argv[2], 10)) == USHRT_MAX )
			return 1;
		if( (delay = parse_ushort(argv[3], 10)) == USHRT_MAX )
			return 1;
		printf("vg: test_init()\n");
		test_init(mode, delay);
		return 0;
	} else if (strncmp(argv[1], "square", strlen("square")) == 0) {
		if( argc != 6 ) {
			printf("vg: wrong no of arguments for test of test_square() \n");
			return 1;
		}
		if( (x = parse_ushort(argv[2], 10)) == USHRT_MAX )
			return 1;
		if( (y = parse_ushort(argv[3], 10)) == USHRT_MAX )
			return 1;
		if( (size = parse_ushort(argv[4], 10)) == USHRT_MAX )
			return 1;
		if( (color = parse_ulong(argv[5], 10)) == ULONG_MAX )
			return 1;
		printf("vg: test_square()\n");
		test_square(x, y, size, color);
		return 0;
	} else if (strncmp(argv[1], "line", strlen("line")) == 0) {
		if( argc != 7 ) {
			printf("vg: wrong no of arguments for test of test_line() \n");
			return 1;
		}
		if( (xi = parse_ushort(argv[2], 10)) == USHRT_MAX )
			return 1;
		if( (yi = parse_ushort(argv[3], 10)) == USHRT_MAX )
			return 1;
		if( (xf = parse_ushort(argv[4], 10)) == USHRT_MAX )
			return 1;
		if( (yf = parse_ushort(argv[5], 10)) == USHRT_MAX )
			return 1;
		if( (color = parse_ulong(argv[6], 10)) == ULONG_MAX )
			return 1;
		printf("vg: test_line()\n");
		test_line(xi, yi, xf, yf, color);
		return 0;
	} else if (strncmp(argv[1], "xpm", strlen("xpm")) == 0) {
		if( argc != 5) {
			printf("vg: wrong no of arguments for test of test_xpm() \n");
			return 1;
		}
		if( (xi = parse_ushort(argv[2], 10)) == USHRT_MAX )
			return 1;
		if( (yi = parse_ushort(argv[3], 10)) == USHRT_MAX )
			return 1;
		if( (xpm_NUMBER = parse_ushort(argv[4], 10)) == ULONG_MAX )
					return 1;

		if(xpm_NUMBER > 5 || xpm_NUMBER < 1)
		{printf("vg: Error in xpm number\n");
		return -1;
		}
		printf("vg: test_xpm()\n");

		begin_xpm(xi, yi, xpm_NUMBER);
		return 0;
	}/* else if (strncmp(argv[1], "move", strlen("move")) == 0) {
		if( argc != 8) {
			printf("vg: wrong no of arguments for test of test_move() \n");
			return 1;
		}
		if( (xi = parse_ushort(argv[2], 10)) == USHRT_MAX )
			return 1;
		if( (yi = parse_ushort(argv[3], 10)) == USHRT_MAX )
			return 1;
		if( (xpm_NUMBER = parse_ushort(argv[4], 10)) == ULONG_MAX )
					return 1;
		if( (hor = parse_ushort(argv[5], 10)) == USHRT_MAX )
					return 1;
		short delta = (short) argv[6];
		if( (time = parse_ushort(argv[7], 10)) == USHRT_MAX )
					return 1;

		if(xpm_NUMBER > 5 || xpm_NUMBER < 1)
		{printf("vg: Error in xpm number\n");
		return -1;
		}
		printf("vg: test_xpm()\n");

		begin_move(xi, yi, xpm_NUMBER, hor, delta, time);
		return 0;
	}*/ else {
		printf("vg: non valid function \"%s\" to test\n", argv[1]);
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

static unsigned long parse_ulong(char *str, int base) {
	char *endptr;
	unsigned long val;

	val = strtoul(str, &endptr, base);

	if ((errno == ERANGE && val == ULONG_MAX )
			|| (errno != 0 && val == 0)) {
		perror("strtol");
		return ULONG_MAX;
	}

	if (endptr == str) {
		printf("timer: parse_ulong: no digits were found in %s \n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
