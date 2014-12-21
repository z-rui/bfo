#include "bf.h"
#include <stdio.h>
#include <string.h>

#ifdef BF_DEBUG
void dumpop(struct bfop *op)
{
	printf(
		"{ type = %d"
#ifdef BF_COLLECT_OPTIMIZE
		", repeat = %d"
#ifdef BF_LOOP_OPTIMIZE
		", offset = %d"
#endif
#endif
		" }\n",
		op->type
#ifdef BF_COLLECT_OPTIMIZE
		, op->repeat
#ifdef BF_LOOP_OPTIMIZE
		, op->offset
#endif
#endif
      );
}
#endif	/* BF_DEBUG */

static void bf_usage(void)
{
	fprintf(stderr,
"Optimized brainf*** intepreter. Compiled at "__TIME__", "__DATE__".\n"
"(c) 2014, Zhang Rui (zr) <zrui16@hotmail.com>.\nAll rights reserved.\n\n"
"Configuration:\n"
"\t"BF_CONFIG(BF_PRGM_SIZE)",\n"
"\t"BF_CONFIG(BF_MEM_SIZE)",\n"
"\t"BF_CONFIG(BF_CELL_TYPE)"(%d)\n"
"Supported optimization:\n"
#ifdef BF_COLLECT_OPTIMIZE
"\tBF_COLLECT_OPTIMIZE\n"
#ifdef BF_LOOP_OPTIMIZE
"\tBF_LOOP_OPTIMIZE\n"
#endif
#else
"(none)\n"
#endif
	, (int) sizeof (bf_cell_t));
}

int main(int argc, char *argv[])
{
	FILE *src;
	static struct bfop bf[BF_PRGM_SIZE];
	struct bfop *end;
#ifdef BF_DEBUG
	struct bfop *p;
#endif

	if (argc == 2) {
		if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
			bf_usage();
			return 0;
		}
		src = fopen(argv[1], "r");
	} else {
		src = stdin;
	}
	if (src == 0) {
		perror(argv[0]);
		return 1;
	}
	end = bf_parse(bf, bf + BF_PRGM_SIZE, src);
#ifdef BF_DEBUG
	for (p = bf; p < end; p++) {
		printf("%04x\t", (unsigned) (p - bf));
		dumpop(p);
	}
#else
	bf_exec(bf, end);
#endif
	return 0;
}
