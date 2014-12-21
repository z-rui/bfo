#ifndef BF_H
#define BF_H

/* bf opcodes */
typedef enum {
	IN = ',',
	OUT = '.',
	LEFT = '<',
	RIGHT = '>',
	INC = '+',
	DEC = '-',
	LOOP = '[',
	LEND = ']',
#ifdef BF_LOOP_OPTIMIZE
	CLEAR = -1,
	MULADD = -2,
#endif
} bf_opcode_t;

/* bf op */
struct bfop {
	bf_opcode_t type;
#ifdef BF_COLLECT_OPTIMIZE
	int repeat;
#ifdef BF_LOOP_OPTIMIZE
	int offset;
#endif
#endif
};

#define BF_CELL_TYPE unsigned char
typedef BF_CELL_TYPE bf_cell_t;
#ifndef BF_PRGM_SIZE
# define BF_PRGM_SIZE 65536
#endif
#ifndef BF_MEM_SIZE
# define BF_MEM_SIZE 30000
#endif

/* string constants */
#define BF_STR_(x) #x
#define BF_STR(x) BF_STR_(x)
#define BF_CONFIG(x) #x"="BF_STR_(x)
#define BF_CELL_TYPE_STR BF_STR(BF_CELL_TYPE)
#define BF_MEM_SIZE_STR BF_STR(BF_MEM_SIZE)
#define BF_PRGM_SIZE_STR BF_STR(BF_PRGM_SIZE)

/* extern funcs from parse.c */

#include <stdio.h>	/* for FILE */
extern struct bfop *
bf_parse(struct bfop *base, struct bfop *lim, FILE *src);
extern void bfi_fatal(const char *msg);

/* extern funcs from bf.c */

/* extern funcs from exec.c */
extern void bf_exec(struct bfop *pc, struct bfop *end);

#endif
