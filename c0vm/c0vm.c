#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#include "lib/xalloc.h"
#include "lib/stack.h"
#include "lib/contracts.h"
#include "lib/c0v_stack.h"
#include "lib/c0vm.h"
#include "lib/c0vm_c0ffi.h"
#include "lib/c0vm_abort.h"

/* call stack frames */
typedef struct frame_info frame;
struct frame_info {
  c0v_stack_t S; /* Operand stack of C0 values */
  ubyte *P;      /* Function body */
  size_t pc;     /* Program counter */
  c0_value *V;   /* The local variables */
};

int execute(struct bc0_file *bc0) {
  REQUIRES(bc0 != NULL);

  /* Variables */
  c0v_stack_t S = c0v_stack_new(); /* Operand stack of C0 values */
  ubyte *P = bc0->function_pool->code;      /* The array of bytes that make up the current function */
  size_t pc = 0;     /* Your current location within the current byte array P */
  c0_value *V = xcalloc(bc0->function_pool->num_vars, sizeof(c0_value));   /* The local variables (you won't need this till Task 2) */
  (void) V;

  /* The call stack, a generic stack that should contain pointers to frames */
  /* You won't need this until you implement functions. */
  gstack_t callStack = stack_new(); 
  (void) callStack;

	/* my_free , first-class function used by stack_free */
	void my_free(stack_elem data) {
		REQUIRES (data != NULL);
		frame * f = (frame *) data;
		c0v_stack_free(f->S);
		free(f->V);
		free(f);
	}

  while (true) {
    
#ifdef DEBUG
    /* You can add extra debugging information here */
    fprintf(stderr, "Opcode %x -- Stack size: %zu -- PC: %zu\n",
            P[pc], c0v_stack_size(S), pc);
#endif
    
    switch (P[pc]) {
      
    /* Additional stack operation: */

    case POP: {
      pc++;
      c0v_pop(S);
      break;
    }

    case DUP: {
      pc++;
      c0_value v = c0v_pop(S);
      c0v_push(S,v);
      c0v_push(S,v);
      break;
    }
      
    case SWAP: {
		pc++;
		c0_value hi = c0v_pop(S);
		c0_value lo = c0v_pop(S);
		c0v_push(S, hi);
		c0v_push(S, lo);
		break;
	}


    /* Returning from a function.
     * This currently has a memory leak! It will need to be revised
     * when you write INVOKESTATIC. */

    case RETURN: {
		c0_value retval = c0v_pop(S);
      //int retval = val2int(c0v_pop(S));
		if (stack_empty(callStack)) {	// main frame
			int intRetval = val2int(retval);
			assert(c0v_stack_empty(S));
			free(V);	// free local variable array
			c0v_stack_free(S);	// free operand stack
			stack_free(callStack, &my_free);	// TODO: my_free()
#ifdef DEBUG
      fprintf(stderr, "Returning %d from execute()\n", intRetval);
#endif
      // Free everything before returning from the execute function!
			return intRetval;
		}
		else {
			frame * callingFrame = (frame *)pop(callStack);	// pop calling frame;
			c0v_stack_free(S);	// free operand stack of current frame
			
			// restore calling frame
			S = callingFrame->S;
			V = callingFrame->V;
			pc = callingFrame->pc;
			P = callingFrame->P;
			
			c0v_push(S, retval);	// push return value to calling frame's operand stack
			free(callingFrame);		// free the poped frame
		}
		break;
	}


    /* Arithmetic and Logical operations */

    case IADD: {
		pc++;
		int32_t i = val2int(c0v_pop(S));
		int32_t j = val2int(c0v_pop(S));
		c0v_push(S, int2val(i+j));
		break;
	}
      
    case ISUB: {
		pc++;
		int32_t i = val2int(c0v_pop(S));
		int32_t j = val2int(c0v_pop(S));
		c0v_push(S, int2val(j-i));
		break;
	}
      
    case IMUL: {
		pc++;
		int32_t i = val2int(c0v_pop(S));
		int32_t j = val2int(c0v_pop(S));
		c0v_push(S, int2val(j*i));
		break;
	}
      
    case IDIV: {
		pc++;
		int32_t i = val2int(c0v_pop(S));
		int32_t j = val2int(c0v_pop(S));
		if (i == 0) {
			c0_arith_error("divide by zero!");
		}
		if (j == INT_MIN && i == -1) {
			c0_arith_error("illegal integer division: INT_MIN / -1");
		}
		c0v_push(S, int2val(j/i));
		break;
	}
      
    case IREM: {
		pc++;
		int32_t i = val2int(c0v_pop(S));
		int32_t j = val2int(c0v_pop(S));
		if (i == 0) {
			c0_arith_error("modulo by zero!");
		}
		if (j == INT_MIN && i == -1) {
			c0_arith_error("illegal modulo: INT_MIN % -1");
		}
		c0v_push(S, int2val(j%i));
		break;
	}
      
    case IAND: {
		pc++;
		int32_t i = val2int(c0v_pop(S));
		int32_t j = val2int(c0v_pop(S));
		c0v_push(S, int2val(j&i));
		break;
	}
      
    case IOR: {
		pc++;
		int32_t i = val2int(c0v_pop(S));
		int32_t j = val2int(c0v_pop(S));
		c0v_push(S, int2val(j|i));
		break;
	}
      
    case IXOR: {
		pc++;
		int32_t i = val2int(c0v_pop(S));
		int32_t j = val2int(c0v_pop(S));
		c0v_push(S, int2val(j^i));
		break;
	}
      
    case ISHL: {
		pc++;
		int32_t i = val2int(c0v_pop(S));
		int32_t j = val2int(c0v_pop(S));
		if (i < 0 || i >= 32) {
			c0_arith_error("Invalid left shit");
		}
		c0v_push(S, int2val(j<<i));
		break;
	}
      
    case ISHR: {
		pc++;
		int32_t i = val2int(c0v_pop(S));
		int32_t j = val2int(c0v_pop(S));
		if (i < 0 || i >= 32) {
			c0_arith_error("Invalid right shit");
		}
		c0v_push(S, int2val(j>>i));
		break;
	}
      
      
    /* Pushing constants */

    case BIPUSH: {
		ubyte b = P[pc+1];
		c0_value c= int2val((int32_t)(int8_t)b);
		c0v_push(S,c);
		pc += 2;
		break;
	}

    case ILDC: {
		uint16_t i = P[pc+1];
		uint16_t j = P[pc+2];
		c0_value c = int2val((int32_t)(bc0->int_pool[i<<8 | j]));
		c0v_push(S, c);
		pc += 3;
		break;
	}
      
    case ALDC: {
		uint16_t i = P[pc+1];
		uint16_t j = P[pc+2];
		c0_value c = ptr2val((void *)&(bc0->string_pool[i<<8 | j]));
		c0v_push(S, c);
		pc += 3;
		break;
	}

    case ACONST_NULL: {
		pc++;
		c0v_push(S, ptr2val((void*)NULL));
		break;
	}


    /* Operations on local variables */

    case VLOAD: {
		c0_value load = V[P[pc+1]];
		c0v_push(S, load);
		pc += 2;
		break;
	}
      
    case VSTORE: {
		c0_value store = c0v_pop(S);
		V[P[pc+1]] = store;
		pc += 2;
		break;
	}
      
      
    /* Control flow operations */

    case NOP: {
		pc++;
		break;
	}
      
    case IF_CMPEQ: {
		int16_t i = P[pc+1];
		int16_t j = P[pc+2];
		c0_value v1 = c0v_pop(S);
		c0_value v2 = c0v_pop(S);
		if (val_equal(v1, v2)) {
			pc += (int16_t)(i<<8|j);
		}
		else {
			pc += 3;
		}
		break;
	}
      
    case IF_CMPNE: {
		int16_t i = P[pc+1];
		int16_t j = P[pc+2];
		c0_value v1 = c0v_pop(S);
		c0_value v2 = c0v_pop(S);
		if (!val_equal(v1, v2)) {
			pc += (int16_t)(i<<8|j);
		}
		else {
			pc += 3;
		}
		break;
	}
      
    case IF_ICMPLT: {
		int16_t i = P[pc+1];
		int16_t j = P[pc+2];
		int32_t v1 = val2int(c0v_pop(S));
		int32_t v2 = val2int(c0v_pop(S));
		if (v2 < v1) {
			pc += (int16_t)(i<<8|j);
		}
		else {
			pc += 3;
		}
		break;
		
	}
      
    case IF_ICMPGE: {
		int16_t i = P[pc+1];
		int16_t j = P[pc+2];
		int32_t v1 = val2int(c0v_pop(S));
		int32_t v2 = val2int(c0v_pop(S));
		if (v2 >= v1) {
			pc += (int16_t)(i<<8|j);
		}
		else {
			pc += 3;
		}
		break;
	}
      
    case IF_ICMPGT: {
		int16_t i = P[pc+1];
		int16_t j = P[pc+2];
		int32_t v1 = val2int(c0v_pop(S));
		int32_t v2 = val2int(c0v_pop(S));
		if (v2 > v1) {
			pc += (int16_t)(i<<8|j);
		}
		else {
			pc += 3;
		}
		break;
	}
      
    case IF_ICMPLE: {
		int16_t i = P[pc+1];
		int16_t j = P[pc+2];
		int32_t v1 = val2int(c0v_pop(S));
		int32_t v2 = val2int(c0v_pop(S));
		if (v2 <= v1) {
			pc += (int16_t)(i<<8|j);
		}
		else {
			pc += 3;
		}
		break;
	}
      
    case GOTO: {
		int16_t i = P[pc+1];
		int16_t j = P[pc+2];
		pc += (int16_t)(i<<8|j);
		break;
	}
      
    case ATHROW: {
		pc++;
		char* err = (char*)val2ptr(c0v_pop(S));
		c0_user_error(err);
		break;
	}

    case ASSERT: {
		pc++;
		char* err = (char*)val2ptr(c0v_pop(S));
		int32_t v = val2int(c0v_pop(S));
		if (v == 0) {
			c0_assertion_failure(err);
		}
		break;
	}


    /* Function call operations: */

    case INVOKESTATIC: {
		uint16_t c1 = P[pc+1];
		uint16_t c2 = P[pc+2];
		
		uint16_t index = c1<<8|c2;
		uint16_t num_args = bc0->function_pool[index].num_args;
		uint16_t num_vars = bc0->function_pool[index].num_vars;
		c0_value *newV = xcalloc(num_vars, sizeof(c0_value));		// create V array for new frame
		/* pop arguments from calling frame and store at new V array */
		for (uint16_t i = num_args - 1; i >= 0; i--) {
			newV[i] = c0v_pop(S);
		}
		/* store calling frame at global callstack */
		frame * callingFrame = xmalloc(sizeof(frame));
		callingFrame->S = S;
		callingFrame->P = P;
		callingFrame->pc = pc + 3;	// return address
		callingFrame->V = V;
		push(callStack, (stack_elem)callingFrame);
		
		/* set up new frame*/
		S = c0v_stack_new();	// create a new operand stack
		P = bc0->function_pool[index].code;
		V = newV;
		pc = 0;
		
		break;
		
	}
      /* native function is called using system stack */
    case INVOKENATIVE: {
		uint16_t c1 = P[pc+1];
		uint16_t c2 = P[pc+1];
		uint16_t index = c1<<8|c2;
		uint16_t num_args = bc0->native_pool[index].num_args;
		uint16_t fn_index = bc0->native_pool[index].function_table_index;
		c0_value *newV = xcalloc(num_args, sizeof(c0_value));
		for (uint16_t i = num_args -1; i >=0; i--) {
			newV[i] = c0v_pop(S);
		}
		c0_value ret = (native_function_table[fn_index])(newV);
		free(newV);
		c0v_push(S, ret);
		pc += 3;
		break;
	}


    /* Memory allocation operations: */

    case NEW: {
		ubyte size = P[pc+1];
		c0_value addr = ptr2val(xmalloc(size));
		c0v_push(S, addr);
		pc += 2;
		break;
	}
      
    case NEWARRAY: {
		ubyte size = P[pc+1];
		int32_t num = val2int(c0v_pop(S));
		if (num < 0) {
			c0_memory_error(" invalid array size! ");
		}
		/* create a new c0_array_header */
		c0_array *arrHeader = xmalloc(sizeof(c0_array));
		arrHeader->count = num;
		arrHeader->elt_size = size; 
		arrHeader->elems = xcalloc(num, size);
		
		c0v_push(S, ptr2val(arrHeader));
		pc += 2;
		break;
	}
      
    case ARRAYLENGTH: {
		c0_array *arrHeader = (c0_array *)val2ptr(c0v_pop(S));
		if (arrHeader == NULL) {
			c0_memory_error("invalid access to null pointer!");
		}
		int length = arrHeader->count;
		c0v_push(S, int2val(length));
		pc++;
		break;
	}


    /* Memory access operations: */

    case AADDF: {
		ubyte offset = P[pc+1];
		char *base = (char *)val2ptr(c0v_pop(S));
		c0_value addr = ptr2val(base + offset);
		c0v_push(S, addr);
		pc += 2;
		break;
	}
      
    case AADDS: {
		int32_t offset = val2int(c0v_pop(S));
		c0_array *arrHeader = (c0_array *)val2ptr(c0v_pop(S));
		if (arrHeader == NULL) {
			c0_memory_error("invalid access to null pointer!");
		}
		int elt_size = arrHeader->elt_size;
		int length = arrHeader->count;
		char *base = arrHeader->elems;
		if (offset < 0 || offset >= length) {
			c0_memory_error("array outbound error!");
		}
		c0_value addr = ptr2val(base+offset*elt_size);
		c0v_push(S, addr);
		pc++;
		break;
	}
      
    case IMLOAD: {
		int32_t *addr = (int32_t *)val2ptr(c0v_pop(S));
		if (addr == NULL) {
			c0_memory_error("invalid access to null pointer!");
		}
		c0v_push(S, int2val(*addr));
		pc++;
		break;
	}
      
    case IMSTORE: {
		int32_t data = val2int(c0v_pop(S));
		int32_t *addr = (int32_t *)val2ptr(c0v_pop(S));
		if (addr == NULL) {
			c0_memory_error("invalid access to null pointer!");
		}
		*addr = data;
		pc++;
		break;
	}
      
    case AMLOAD: {
		void **addr = val2ptr(c0v_pop(S));
		if (addr == NULL) {
			c0_memory_error("invalid access to null pointer!");
		}
		c0v_push(S, ptr2val(*addr));
		pc++;
		break;
	}
      
    case AMSTORE: {
		void *data = val2ptr(c0v_pop(S));
		void **addr = val2ptr(c0v_pop(S));
		if (addr == NULL) {
			c0_memory_error("invalid access to null pointer!");
		}
		*addr = data;
		pc++;
		break;
	}
      
    case CMLOAD: {
		char *addr = (char *)val2ptr(c0v_pop(S));
		if (addr == NULL) {
			c0_memory_error("invalid access to null pointer!");
		}
		int32_t data = (int32_t)(*addr);
		c0v_push(S, int2val(data));
		pc++;
		break;
	}
      
    case CMSTORE: {
		int32_t data = val2int(c0v_pop(S));
		char *addr = (char *)val2ptr(c0v_pop(S));
		if (addr == NULL) {
			c0_memory_error("invalid access to null pointer!");
		}
		*addr = (char)(data & 0x17);
		pc++;
		break;
	}
      
    default:
      fprintf(stderr, "invalid opcode: 0x%02x\n", P[pc]);
      abort();
    }
  }
  
  /* cannot get here from infinite loop */
  assert(false);
}

