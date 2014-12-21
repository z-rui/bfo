#include "bf.h"

static void dump_c(struct bfop *pc, struct bfop *end, FILE *out)
{
	fprintf(out,
		"#include <stdio.h>\n"
		"int main() {\n"
		"static "BF_CELL_TYPE_STR" cell["BF_MEM_SIZE_STR"];\n"
		BF_CELL_TYPE_STR" *p = cell;\n\n");
	while (pc < end) {
#ifdef BF_COLLECT_OPTIMIZE
		int i;
#endif

		switch (pc->type) {
		case IN:
#ifdef BF_COLLECT_OPTIMIZE
			for (i = 0; i < pc->repeat; i++)
#endif
				fprintf(out, "*p=getchar();\n");
			break;
		case OUT:
#ifdef BF_COLLECT_OPTIMIZE
			for (i = 0; i < pc->repeat; i++)
#endif
				fprintf(out, "putchar(*p);\n");
			break;
		case LEFT:
#ifdef BF_COLLECT_OPTIMIZE
			if (pc->repeat > 1)
				fprintf(out, "p-=%d;\n", pc->repeat);
			else
#endif
				fprintf(out, "--p;\n");
			break;
		case RIGHT:
#ifdef BF_COLLECT_OPTIMIZE
			if (pc->repeat > 1)
				fprintf(out, "p+=%d;\n", pc->repeat);
			else
#endif
				fprintf(out, "++p;\n");
			break;
		case INC:
#ifdef BF_COLLECT_OPTIMIZE
			if (pc->repeat > 1)
				fprintf(out, "*p+=%d;\n", pc->repeat);
			else
#endif
				fprintf(out, "++*p;\n");
			break;
		case DEC:
#ifdef BF_COLLECT_OPTIMIZE
			if (pc->repeat > 1)
				fprintf(out, "*p-=%d;\n", pc->repeat);
			else
#endif
				fprintf(out, "--*p;\n");
			break;
		case LOOP:
			fprintf(out, "while(*p) {\n");
			break;
		case LEND:
			fprintf(out, "}\n");
			break;
#ifdef BF_LOOP_OPTIMIZE
		case CLEAR:
			fprintf(out, "*p=0;\n");
			break;
		case MULADD:
			if (pc->repeat == 1)
				fprintf(out, "p[%d]+=*p;\n", pc->offset);
			else
				fprintf(out, "p[%d]+=%d**p;\n", pc->offset, pc->repeat);
			break;
#endif
		default:
			bfi_fatal("BUG: unrecognized op");
			break;
		}
		++pc;
	}
	fprintf(out, "return 0;\n}\n");
}

int main(int argc, char *argv[])
{
	FILE *src;
	static struct bfop bf[BF_PRGM_SIZE];
	struct bfop *end;

	if (argc == 2) {
		src = fopen(argv[1], "r");
	} else {
		src = stdin;
	}
	if (src == 0) {
		perror(argv[0]);
		return 1;
	}
	end = bf_parse(bf, bf + BF_PRGM_SIZE, src);
	dump_c(bf, end, stdout);
	fprintf(stderr, "stats\nprogram size=%d\n", (int) (end-bf));
	return 0;
}
