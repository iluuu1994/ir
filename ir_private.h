#ifndef IR_PRIVATE_H
#define IR_PRIVATE_H
#include <string.h>
#include <malloc.h>

#ifdef IR_DEBUG
# include <assert.h>
# define IR_ASSERT(x) assert(x)
#else
# define IR_ASSERT(x)
#endif

#ifdef __has_builtin
# if __has_builtin(__builtin_expect)
#   define EXPECTED(condition)   __builtin_expect(!!(condition), 1)
#   define UNEXPECTED(condition) __builtin_expect(!!(condition), 0)
# endif
# if __has_attribute(always_inline)
#  define IR_ALWAYS_INLINE static inline __attribute__((always_inline))
# endif
# if __has_attribute(noinline)
#  define IR_NEVER_INLINE __attribute__((noinline))
# endif
#endif
#ifndef EXPECTED
# define EXPECTED(condition)   (condition)
# define UNEXPECTED(condition) (condition)
#endif
#ifndef IR_ALWAYS_INLINE
# define IR_ALWAYS_INLINE static inline
#endif
#ifndef IR_NEVER_INLINE
# define IR_NEVER_INLINE
#endif

/*** Helper routines ***/

#define IR_ALIGNED_SIZE(size, alignment) \
	(((size) + ((alignment) - 1)) & ~((alignment) - 1))

#define IR_MAX(a, b)          (((a) > (b)) ? (a) : (b))
#define IR_MIN(a, b)          (((a) < (b)) ? (a) : (b))

#define IR_IS_POWER_OF_TWO(x) (!((x) & ((x) - 1)))

#define IR_LOG2(x) ir_ntzl(x)

IR_ALWAYS_INLINE uint8_t ir_rol8(uint8_t op1, uint8_t op2)
{
	return (op1 << op2) | (op1 >> (8 - op2));
}

IR_ALWAYS_INLINE uint16_t ir_rol16(uint16_t op1, uint16_t op2)
{
	return (op1 << op2) | (op1 >> (16 - op2));
}

IR_ALWAYS_INLINE uint32_t ir_rol32(uint32_t op1, uint32_t op2)
{
	return (op1 << op2) | (op1 >> (32 - op2));
}

IR_ALWAYS_INLINE uint64_t ir_rol64(uint64_t op1, uint64_t op2)
{
	return (op1 << op2) | (op1 >> (64 - op2));
}

IR_ALWAYS_INLINE uint8_t ir_ror8(uint8_t op1, uint8_t op2)
{
	return (op1 >> op2) | (op1 << (8 - op2));
}

IR_ALWAYS_INLINE uint16_t ir_ror16(uint16_t op1, uint16_t op2)
{
	return (op1 >> op2) | (op1 << (16 - op2));
}

IR_ALWAYS_INLINE uint32_t ir_ror32(uint32_t op1, uint32_t op2)
{
	return (op1 >> op2) | (op1 << (32 - op2));
}

IR_ALWAYS_INLINE uint64_t ir_ror64(uint64_t op1, uint64_t op2)
{
	return (op1 >> op2) | (op1 << (64 - op2));
}

/* Number of trailing zero bits (0x01 -> 0; 0x40 -> 6; 0x00 -> LEN) */
IR_ALWAYS_INLINE uint32_t ir_ntz(uint32_t num)
{
#if (defined(__GNUC__) || __has_builtin(__builtin_ctz))
	return __builtin_ctz(num);
#elif defined(_WIN32)
	uint32_t index;

	if (!BitScanForward(&index, num)) {
		/* undefined behavior */
		return 32;
	}

	return index;
#else
	int n;

	if (num == 0) return 32;

	n = 1;
	if ((num & 0x0000ffff) == 0) {n += 16; num = num >> 16;}
	if ((num & 0x000000ff) == 0) {n +=  8; num = num >>  8;}
	if ((num & 0x0000000f) == 0) {n +=  4; num = num >>  4;}
	if ((num & 0x00000003) == 0) {n +=  2; num = num >>  2;}
	return n - (num & 1);
#endif
}

/* Number of trailing zero bits (0x01 -> 0; 0x40 -> 6; 0x00 -> LEN) */
IR_ALWAYS_INLINE uint32_t ir_ntzl(uint64_t num)
{
#if (defined(__GNUC__) || __has_builtin(__builtin_ctzl))
	return __builtin_ctzl(num);
#elif defined(_WIN32)
	unsigned long index;

#if defined(_WIN64)
	if (!BitScanForward64(&index, num)) {
#else
	if (!BitScanForward(&index, num)) {
#endif
		/* undefined behavior */
		return 64;
	}

	return (uint32_t) index;
#else
	uint32_t n;

	if (num == Z_UL(0)) return 64;

	n = 1;
	if ((num & 0xffffffff) == 0) {n += 32; num = num >> Z_UL(32);}
	if ((num & 0x0000ffff) == 0) {n += 16; num = num >> 16;}
	if ((num & 0x000000ff) == 0) {n +=  8; num = num >>  8;}
	if ((num & 0x0000000f) == 0) {n +=  4; num = num >>  4;}
	if ((num & 0x00000003) == 0) {n +=  2; num = num >>  2;}
	return n - (num & 1);
#endif
}

/* Number of leading zero bits (Undefined for zero) */
IR_ALWAYS_INLINE int ir_nlz(uint32_t num)
{
#if (defined(__GNUC__) || __has_builtin(__builtin_clz))
	return __builtin_clz(num);
#elif defined(_WIN32)
	uint32_t index;

	if (!BitScanReverse(&index, num)) {
		/* undefined behavior */
		return 32;
	}

	return (int) (32 - 1) - index;
#else
	uint32_t x;
	uint32_t n;

	n = 32;
	x = num >> 16; if (x != 0) {n -= 16; num = x;}
	x = num >> 8;  if (x != 0) {n -=  8; num = x;}
	x = num >> 4;  if (x != 0) {n -=  4; num = x;}
	x = num >> 2;  if (x != 0) {n -=  2; num = x;}
	x = num >> 1;  if (x != 0) return n - 2;
	return n - num;
#endif
}

IR_ALWAYS_INLINE int ir_nlzl(uint64_t num)
{
#if (defined(__GNUC__) || __has_builtin(__builtin_clzll))
	return __builtin_clzll(num);
#elif defined(_WIN32)
	uint32_t index;

	if (!BitScanReverse64(&index, num)) {
		/* undefined behavior */
		return 64;
	}

	return (int) (64 - 1) - index;
#else
	uint64_t x;
	uint65_t n;

	n = 64;
	x = num >> 32; if (x != 0) {n -= 32; num = x;}
	x = num >> 16; if (x != 0) {n -= 16; num = x;}
	x = num >> 8;  if (x != 0) {n -=  8; num = x;}
	x = num >> 4;  if (x != 0) {n -=  4; num = x;}
	x = num >> 2;  if (x != 0) {n -=  2; num = x;}
	x = num >> 1;  if (x != 0) return n - 2;
	return n - num;
#endif
}

/*** Helper data types ***/

/* Bitsets */
typedef uintptr_t *ir_bitset;

IR_ALWAYS_INLINE uint32_t ir_bitset_len(uint32_t n)
{
	return (n + (sizeof(uintptr_t) * 8 - 1)) / (sizeof(uintptr_t) * 8);
}

IR_ALWAYS_INLINE ir_bitset ir_bitset_malloc(uint32_t n)
{
	return ir_mem_calloc(ir_bitset_len(n), sizeof(uintptr_t));
}

IR_ALWAYS_INLINE void ir_bitset_incl(ir_bitset set, uint32_t n)
{
	set[n / (sizeof(uintptr_t) * 8)] |= 1UL << (n % (sizeof(uintptr_t) * 8));
}

IR_ALWAYS_INLINE void ir_bitset_excl(ir_bitset set, uint32_t n)
{
	set[n / (sizeof(uintptr_t) * 8)] &= ~(1UL << (n % (sizeof(uintptr_t) * 8)));
}

IR_ALWAYS_INLINE bool ir_bitset_in(ir_bitset set, uint32_t n)
{
	return (set[(n / (sizeof(uintptr_t) * 8))] & (1UL << (n % (sizeof(uintptr_t) * 8)))) != 0;
}

IR_ALWAYS_INLINE void ir_bitset_clear(ir_bitset set, uint32_t len)
{
	memset(set, 0, len * sizeof(uintptr_t));
}

IR_ALWAYS_INLINE void ir_bitset_fill(ir_bitset set, uint32_t len)
{
	memset(set, 0xff, len * sizeof(uintptr_t));
}

IR_ALWAYS_INLINE bool ir_bitset_empty(ir_bitset set, uint32_t len)
{
	uint32_t i;
	for (i = 0; i < len; i++) {
		if (set[i]) {
			return 0;
		}
	}
	return 1;
}

IR_ALWAYS_INLINE bool ir_bitset_equal(ir_bitset set1, ir_bitset set2, uint32_t len)
{
    return memcmp(set1, set2, len * sizeof(uintptr_t)) == 0;
}

IR_ALWAYS_INLINE void ir_bitset_copy(ir_bitset set1, ir_bitset set2, uint32_t len)
{
    memcpy(set1, set2, len * sizeof(uintptr_t));
}

IR_ALWAYS_INLINE void ir_bitset_intersection(ir_bitset set1, ir_bitset set2, uint32_t len)
{
    uint32_t i;

    for (i = 0; i < len; i++) {
		set1[i] &= set2[i];
	}
}

IR_ALWAYS_INLINE void ir_bitset_union(ir_bitset set1, ir_bitset set2, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++) {
		set1[i] |= set2[i];
	}
}

IR_ALWAYS_INLINE void ir_bitset_difference(ir_bitset set1, ir_bitset set2, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++) {
		set1[i] = set1[i] & ~set2[i];
	}
}

IR_ALWAYS_INLINE bool ir_bitset_subset(ir_bitset set1, ir_bitset set2, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++) {
		if (set1[i] & ~set2[i]) {
			return 0;
		}
	}
	return 1;
}

IR_ALWAYS_INLINE int ir_bitset_first(ir_bitset set, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++) {
		if (set[i]) {
			if (sizeof(uintptr_t) == 4) {
				return 32 * i + ir_ntz(set[i]);
			} else {
				return 64 * i + ir_ntzl(set[i]);
			}
		}
	}
	return -1; /* empty set */
}

IR_ALWAYS_INLINE int ir_bitset_last(ir_bitset set, uint32_t len)
{
	uint32_t i = len;

	while (i > 0) {
		i--;
		if (set[i]) {
			uint32_t j = (sizeof(uintptr_t) * 8) * i - 1;
			uintptr_t x = set[i];
			do {
				x = x >> 1;
				j++;
			} while (x != 0);
			return j;
		}
	}
	return -1; /* empty set */
}

IR_ALWAYS_INLINE int ir_bitset_pop_first(ir_bitset set, uint32_t len) {
	int i = ir_bitset_first(set, len);
	if (i >= 0) {
		ir_bitset_excl(set, i);
	}
	return i;
}

#define IR_BITSET_FOREACH(set, len, bit) do { \
	ir_bitset _set = (set); \
	uint32_t _i, _len = (len); \
	for (_i = 0; _i < _len; _i++) { \
		uintptr_t _x = _set[_i]; \
		if (_x) { \
			(bit) = (sizeof(uintptr_t) * 8) * _i; \
			for (; _x != 0; _x >>= 1, (bit)++) { \
				if (!(_x & 1)) continue;

#define IR_BITSET_FOREACH_END() \
			} \
		} \
	} \
} while (0)

/* Dynamic array of numeric references */
typedef struct _ir_array {
	ir_ref   *refs;
	uint32_t  size;
} ir_array;

void ir_array_grow(ir_array *a, uint32_t size);
void ir_array_insert(ir_array *a, uint32_t i, ir_ref val);
void ir_array_remove(ir_array *a, uint32_t i);

IR_ALWAYS_INLINE void ir_array_init(ir_array *a, uint32_t size)
{
	a->refs = ir_mem_calloc(size, sizeof(ir_ref));
	a->size = size;
}

IR_ALWAYS_INLINE void ir_array_free(ir_array *a)
{
	ir_mem_free(a->refs);
	a->refs = NULL;
	a->size = 0;
}

IR_ALWAYS_INLINE uint32_t ir_array_size(ir_array *a)
{
	return a->size;
}

IR_ALWAYS_INLINE ir_ref ir_array_get(ir_array *a, uint32_t i)
{
	return (i < a->size) ? a->refs[i] : IR_UNUSED;
}

IR_ALWAYS_INLINE ir_ref ir_array_at(ir_array *a, uint32_t i)
{
	IR_ASSERT(i < a->size);
	return a->refs[i];
}

IR_ALWAYS_INLINE void ir_array_set(ir_array *a, uint32_t i, ir_ref val)
{
	if (i >= a->size) {
		ir_array_grow(a, i + 1);
	}
	a->refs[i] = val;
}

/* List/Stack of numeric references */
typedef struct _ir_list {
	ir_array a;
	uint32_t len;
} ir_list;

bool ir_list_contains(ir_list *l, ir_ref val);
void ir_list_insert(ir_list *l, uint32_t i, ir_ref val);
void ir_list_remove(ir_list *l, uint32_t i);

IR_ALWAYS_INLINE void ir_list_init(ir_list *l, uint32_t size)
{
	ir_array_init(&l->a, size);
	l->len = 0;
}

IR_ALWAYS_INLINE void ir_list_free(ir_list *l)
{
	ir_array_free(&l->a);
	l->len = 0;
}

IR_ALWAYS_INLINE void ir_list_clear(ir_list *l)
{
	l->len = 0;
}

IR_ALWAYS_INLINE uint32_t ir_list_len(ir_list *l)
{
	return l->len;
}

IR_ALWAYS_INLINE uint32_t ir_list_capasity(ir_list *l)
{
	return ir_array_size(&l->a);
}

IR_ALWAYS_INLINE void ir_list_push(ir_list *l, ir_ref val)
{
	ir_array_set(&l->a, l->len++, val);
}

IR_ALWAYS_INLINE ir_ref ir_list_pop(ir_list *l)
{
	IR_ASSERT(l->len > 0);
	return ir_array_at(&l->a, --l->len);
}

IR_ALWAYS_INLINE ir_ref ir_list_peek(ir_list *l)
{
	IR_ASSERT(l->len > 0);
	return ir_array_at(&l->a, l->len - 1);
}

IR_ALWAYS_INLINE ir_ref ir_list_at(ir_list *l, uint32_t i)
{
	IR_ASSERT(i < l->len);
	return ir_array_at(&l->a, i);
}

/* Worklist (unique list) */
typedef struct _ir_worklist {
	ir_list l;
	ir_bitset visited;
} ir_worklist;

IR_ALWAYS_INLINE void ir_worklist_init(ir_worklist *w, uint32_t size)
{
	ir_list_init(&w->l, size);
	w->visited = ir_bitset_malloc(size);
}

IR_ALWAYS_INLINE void ir_worklist_free(ir_worklist *w)
{
	ir_list_free(&w->l);
	ir_mem_free(w->visited);
}

IR_ALWAYS_INLINE uint32_t ir_worklist_len(ir_worklist *w)
{
	return ir_list_len(&w->l);
}

IR_ALWAYS_INLINE uint32_t ir_worklist_capasity(ir_worklist *w)
{
	return ir_list_capasity(&w->l);
}

IR_ALWAYS_INLINE void ir_worklist_clear(ir_worklist *w)
{
	ir_list_free(&w->l);
	ir_bitset_clear(w->visited, ir_bitset_len(ir_worklist_capasity(w)));
}

IR_ALWAYS_INLINE bool ir_worklist_push(ir_worklist *w, ir_ref val)
{
	IR_ASSERT(val >= 0 && val < ir_worklist_capasity(w));
	if (ir_bitset_in(w->visited, val)) {
		return 0;
	}
	ir_bitset_incl(w->visited, val);
	ir_list_push(&w->l, val);
	return 1;
}

IR_ALWAYS_INLINE ir_ref ir_worklist_pop(ir_worklist *w)
{
	return ir_list_pop(&w->l);
}

IR_ALWAYS_INLINE ir_ref ir_worklist_peek(ir_worklist *w)
{
	return ir_list_peek(&w->l);
}

/*** IR OP info ***/
extern const uint8_t ir_type_flags[IR_LAST_TYPE];
extern const char *ir_type_name[IR_LAST_TYPE];
extern const char *ir_type_cname[IR_LAST_TYPE];
extern const uint8_t ir_type_size[IR_LAST_TYPE];
extern const uint32_t ir_op_flags[IR_LAST_OP];
extern const char *ir_op_name[IR_LAST_OP];

#define IR_IS_CONST_OP(op)       ((op) > IR_NOP && (op) <= IR_C_FLOAT)
#define IR_IS_FOLDABLE_OP(op)    ((op) <= IR_LAST_FOLDABLE_OP)

/* IR OP flags */
#define IR_OP_FLAG_OPERANDS_SHIFT 3

#define IR_OP_FLAG_EDGES_MSK      0x07
#define IR_OP_FLAG_OPERANDS_MSK   0x38
#define IR_OP_FLAG_MEM_MASK       ((1<<6)|(1<<7))

#define IR_OP_FLAG_DATA           (1<<8)
#define IR_OP_FLAG_CONTROL        (1<<9)
#define IR_OP_FLAG_MEM            (1<<10)
#define IR_OP_FLAG_COMMUTATIVE    (1<<11)
#define IR_OP_FLAG_BB_START       (1<<12)
#define IR_OP_FLAG_BB_END         (1<<13)
#define IR_OP_FLAG_TERMINATOR     (1<<14)

#define IR_OP_FLAG_MEM_LOAD       ((0<<6)|(0<<7))
#define IR_OP_FLAG_MEM_STORE      ((0<<6)|(1<<7))
#define IR_OP_FLAG_MEM_CALL       ((1<<6)|(0<<7))
#define IR_OP_FLAG_MEM_ALLOC      ((1<<6)|(1<<7))

#define IR_OPND_UNUSED            0x0
#define IR_OPND_DATA              0x1
#define IR_OPND_CONTROL           0x2
#define IR_OPND_CONTROL_DEP       0x3
#define IR_OPND_CONTROL_REF       0x4
#define IR_OPND_VAR               0x5
#define IR_OPND_STR               0x6
#define IR_OPND_NUM               0x7
#define IR_OPND_PROB              0x8

#define IR_OP_FLAGS(op_flags, op1_flags, op2_flags, op3_flags) \
	((op_flags) | ((op1_flags) << 20) | ((op2_flags) << 24) | ((op3_flags) << 28))

#define IR_INPUT_EDGES_COUNT(flags) (flags & IR_OP_FLAG_EDGES_MSK)
#define IR_OPERANDS_COUNT(flags)    ((flags & IR_OP_FLAG_OPERANDS_MSK) >> IR_OP_FLAG_OPERANDS_SHIFT)

#define IR_OPND_KIND(flags, i) \
	(((flags) >> (16 + (4 * (((i) > 3) ? 3 : (i))))) & 0xf)

IR_ALWAYS_INLINE ir_ref ir_variable_inputs_count(ir_insn *insn)
{
	uint32_t n = insn->inputs_count;
	if (n == 0) {
		n = 2;
	}
	return n;
}

IR_ALWAYS_INLINE ir_ref ir_operands_count(ir_ctx *ctx, ir_insn *insn)
{
	uint32_t flags = ir_op_flags[insn->op];
	uint32_t n = IR_OPERANDS_COUNT(flags);
	if (n == 4) {
		/* MERGE or CALL */
		n = ir_variable_inputs_count(insn);
		if (n == 0) {
			n = 2;
		}
	} else if (n == 5) {
		/* PHI */
		n = ir_variable_inputs_count(&ctx->ir_base[insn->op1]) + 1;
	}
	return n;
}

IR_ALWAYS_INLINE ir_ref ir_input_edges_count(ir_ctx *ctx, ir_insn *insn)
{
	uint32_t flags = ir_op_flags[insn->op];
	uint32_t n = IR_INPUT_EDGES_COUNT(flags);
	if (n == 4) {
		/* MERGE or CALL */
		n = ir_variable_inputs_count(insn);
	} else if (n == 5) {
		/* PHI */
		n = ir_variable_inputs_count(&ctx->ir_base[insn->op1]) + 1;
	}
	return n;
}

/*** IR Use Lists ***/
struct _ir_use_list {
	ir_ref        refs; /* index in ir_ctx->use_edges[] array */
	ir_ref        count;
};

/*** IR Basic Blocks info ***/
#define IR_IS_BB_START(op) \
	((ir_op_flags[op] & IR_OP_FLAG_BB_START) != 0)

#define IR_IS_BB_MERGE(op) \
	((op) == IR_MERGE || (op) == IR_LOOP_BEGIN)

#define IR_IS_BB_END(op) \
	((ir_op_flags[op] & IR_OP_FLAG_BB_END) != 0)

#define IR_BB_UNREACHABLE      (1<<0)
#define IR_BB_START            (1<<1)
#define IR_BB_ENTRY            (1<<2)
#define IR_BB_LOOP_HEADER      (1<<3)
#define IR_BB_IRREDUCIBLE_LOOP (1<<4)
#define IR_BB_DESSA_MOVES      (1<<5) /* translation out of SSA requires MOVEs      */
#define IR_BB_MAY_SKIP         (1<<6) /* empty BB                                   */

struct _ir_block {
	uint32_t flags;
	ir_ref   start;              /* index of first instruction                 */
	ir_ref   end;                /* index of last instruction                  */
	uint32_t successors;         /* index in ir_ctx->cfg_edges[] array         */
	uint32_t successors_count;
	uint32_t predecessors;       /* index in ir_ctx->cfg_edges[] array         */
	uint32_t predecessors_count;
	union {
		int  dom_parent;         /* immediate dominator block                  */
		int  idom;               /* immediate dominator block                  */
	};
	union {
		int  dom_depth;          /* depth from the root of the dominators tree */
		int  postnum;            /* used temporary during tree constructon     */
	};
	int      dom_child;          /* first dominated blocks                     */
	int      dom_next_child;     /* next dominated block (linked list)         */
	int      loop_header;
	int      loop_depth;
};

/*** Folding Engine (see ir.c and ir_fold.h) ***/
typedef enum _ir_fold_action {
	IR_FOLD_DO_RESTART,
	IR_FOLD_DO_CSE,
	IR_FOLD_DO_EMIT,
	IR_FOLD_DO_COPY,
	IR_FOLD_DO_CONST
} ir_fold_action;

ir_ref ir_folding(ir_ctx *ctx, uint32_t opt, ir_ref op1, ir_ref op2, ir_ref op3, ir_insn *op1_insn, ir_insn *op2_insn, ir_insn *op3_insn);

/*** IR Live Info ***/
typedef ir_ref                   ir_live_pos;
typedef struct _ir_use_pos       ir_use_pos;
typedef struct _ir_live_range    ir_live_range;
typedef struct _ir_live_interval ir_live_interval;

#define IR_SUB_REFS_COUNT                4

#define IR_LOAD_SUB_REF                  0
#define IR_USE_SUB_REF                   1
#define IR_DEF_SUB_REF                   2
#define IR_SAVE_SUB_REF                  3

#define IR_LIVE_POS_TO_REF(pos)          ((pos) / IR_SUB_REFS_COUNT)
#define IR_LIVE_POS_TO_SUB_REF(pos)      ((pos) % IR_SUB_REFS_COUNT)

#define IR_LIVE_POS_FROM_REF(ref)        ((ref) * IR_SUB_REFS_COUNT)

#define IR_START_LIVE_POS_FROM_REF(ref)  ((ref) * IR_SUB_REFS_COUNT)
#define IR_LOAD_LIVE_POS_FROM_REF(ref)   ((ref) * IR_SUB_REFS_COUNT + IR_LOAD_SUB_REF)
#define IR_USE_LIVE_POS_FROM_REF(ref)    ((ref) * IR_SUB_REFS_COUNT + IR_USE_SUB_REF)
#define IR_DEF_LIVE_POS_FROM_REF(ref)    ((ref) * IR_SUB_REFS_COUNT + IR_DEF_SUB_REF)
#define IR_SAVE_LIVE_POS_FROM_REF(ref)   ((ref) * IR_SUB_REFS_COUNT + IR_SAVE_SUB_REF)
#define IR_END_LIVE_POS_FROM_REF(ref)    ((ref) * IR_SUB_REFS_COUNT + IR_SUB_REFS_COUNT)

/* ir_use_pos.flags bits */
#define IR_USE_MUST_BE_IN_REG            (1<<0)
#define IR_USE_SHOULD_BE_IN_REG          (1<<1)
#define IR_DEF_REUSES_OP1_REG            (1<<2)
#define IR_DEF_CONFLICTS_WITH_INPUT_REGS (1<<3)

#define IR_PHI_USE                       (1<<7)

struct _ir_use_pos {
	uint16_t       op_num; /* 0 - means result */
	int8_t         hint;
	uint8_t        flags;
	ir_ref         hint_ref;
	ir_live_pos    pos;
	ir_use_pos    *next;
};

struct _ir_live_range {
	ir_live_pos    start; /* inclusive */
	ir_live_pos    end;   /* exclusive */
	ir_live_range *next;
};

/* ir_live_interval.flags bits (two low bits are reserved for temporary register number) */
#define IR_LIVE_INTERVAL_TEMP_NUM_MASK   0x3
#define IR_LIVE_INTERVAL_FIXED           (1<<2)
#define IR_LIVE_INTERVAL_TEMP            (1<<3)
#define IR_LIVE_INTERVAL_VAR             (1<<4)
#define IR_LIVE_INTERVAL_COALESCED       (1<<5)
#define IR_LIVE_INTERVAL_HAS_HINTS       (1<<6)
#define IR_LIVE_INTERVAL_MEM_PARAM       (1<<7)
#define IR_LIVE_INTERVAL_MEM_LOAD        (1<<8)
#define IR_LIVE_INTERVAL_REG_LOAD        (1<<9)

struct _ir_live_interval {
	uint8_t           type;
	int8_t            reg;
	uint16_t          flags;
	int32_t           vreg;
	int32_t           stack_spill_pos;
	ir_live_range     range;
	ir_live_pos       end;       /* end of the last live range (cahce of ival.range.{next->}end) */
	ir_live_range    *current_range;
	ir_use_pos       *use_pos;
	ir_live_interval *top;
	ir_live_interval *next;
	ir_live_interval *list_next; /* linked list of active, inactive or unhandled intervals */
};

typedef int (*emit_copy_t)(ir_ctx *ctx, uint8_t type, ir_ref from, ir_ref to);

int ir_gen_dessa_moves(ir_ctx *ctx, int b, emit_copy_t emit_copy);
void ir_free_live_ranges(ir_live_range *live_range);
void ir_free_live_intervals(ir_live_interval **live_intervals, int count);

#if defined(IR_REGSET_64BIT)

/*** Register Sets ***/
#if IR_REGSET_64BIT
typedef uint64_t ir_regset;
#else
typedef uint32_t ir_regset;
#endif

#define IR_REGSET_EMPTY 0

#define IR_REGSET_IS_EMPTY(regset) \
	(regset == IR_REGSET_EMPTY)

#define IR_REGSET_IS_SINGLETON(regset) \
	(regset && !(regset & (regset - 1)))

#if IR_REGSET_64BIT
# define IR_REGSET(reg) \
	(1ull << (reg))
#else
# define IR_REGSET(reg) \
	(1u << (reg))
#endif

#if IR_REGSET_64BIT
# define IR_REGSET_INTERVAL(reg1, reg2) \
	(((1ull << ((reg2) - (reg1) + 1)) - 1) << (reg1))
#else
# define IR_REGSET_INTERVAL(reg1, reg2) \
	(((1u << ((reg2) - (reg1) + 1)) - 1) << (reg1))
#endif

#define IR_REGSET_IN(regset, reg) \
	(((regset) & IR_REGSET(reg)) != 0)

#define IR_REGSET_INCL(regset, reg) \
	(regset) |= IR_REGSET(reg)

#define IR_REGSET_EXCL(regset, reg) \
	(regset) &= ~IR_REGSET(reg)

#define IR_REGSET_UNION(set1, set2) \
	((set1) | (set2))

#define IR_REGSET_INTERSECTION(set1, set2) \
	((set1) & (set2))

#define IR_REGSET_DIFFERENCE(set1, set2) \
	((set1) & ~(set2))

#if IR_REGSET_64BIT
# define IR_REGSET_FIRST(set) ((ir_reg)ir_ntzl(set))
# define ir_REGSET_LAST(set)  ((ir_reg)(ir_nlzl(set)(set)^63))
#else
# define IR_REGSET_FIRST(set) ((ir_reg)ir_ntz(set))
# define IR_REGSET_LAST(set)  ((ir_reg)(ir_nlz(set)^31))
#endif

#define IR_REGSET_FOREACH(set, reg) \
	do { \
		ir_regset _tmp = (set); \
		while (!IR_REGSET_IS_EMPTY(_tmp)) { \
			ir_reg _reg = IR_REGSET_FIRST(_tmp); \
			IR_REGSET_EXCL(_tmp, _reg); \
			reg = _reg; \

#define IR_REGSET_FOREACH_END() \
		} \
	} while (0)

/*** IR Register Allocation ***/
/* Flags for ctx->regs[][] (low bits are used for register number itself) */
#define IR_REG_SPILL_LOAD  (1<<6)
#define IR_REG_SPILL_STORE (1<<6)
#define IR_REG_NUM(r) \
	((r) == IR_REG_NONE ? IR_REG_NONE : ((r) & ~(IR_REG_SPILL_LOAD|IR_REG_SPILL_STORE)))

typedef struct _ir_tmp_reg {
	uint8_t num;
	uint8_t type;
	uint8_t start;
	uint8_t end;
} ir_tmp_reg;

typedef struct _ir_reg_alloc_data {
	int32_t stack_frame_size;
	int32_t unused_slot_4;
	int32_t unused_slot_2;
	int32_t unused_slot_1;
} ir_reg_alloc_data;

int32_t ir_allocate_spill_slot(ir_ctx *ctx, ir_type type, ir_reg_alloc_data *data);

/*** IR Target Interface ***/
typedef enum _ir_reg ir_reg;

bool ir_needs_vreg(ir_ctx *ctx, ir_ref ref);

/* Registers modified by the given instruction */
ir_regset ir_get_scratch_regset(ir_ctx *ctx, ir_ref ref, ir_live_pos *start, ir_live_pos *end);
ir_reg ir_uses_fixed_reg(ir_ctx *ctx, ir_ref ref, int op_num);
uint8_t ir_get_def_flags(ir_ctx *ctx, ir_ref ref);
uint8_t ir_get_use_flags(ir_ctx *ctx, ir_ref ref, int op_num);
int ir_get_temporary_regs(ir_ctx *ctx, ir_ref ref, ir_tmp_reg *tmp_regs);

#endif /* defined(IR_REGSET_64BIT) */


int ir_regs_number(void);
const char *ir_reg_name(int8_t reg, ir_type type);

#endif /* IR_PRIVATE_H */
