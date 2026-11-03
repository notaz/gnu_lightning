#include <lightning.h>
#include <assert.h>

int
main(int argc, char *argv[])
{
    jit_state_t		 *_jit, *entries;
    jit_node_t		 *first, *second, *jump;
    jit_pointer_t	  code;
    jit_word_t		(*entry)(void), (*function1)(void), (*function2)(void);
    jit_word_t		(*caller)(void);
    int			  leading_jump;

    init_jit(argv[0]);
    for (leading_jump = 0; leading_jump < 2; leading_jump++) {
	_jit = entries = jit_new_state();
	jump = leading_jump ? jit_jmpi() : NULL;
	first = jit_indirect();
	jit_prolog();
	jit_reti(11);
	jit_epilog();
	second = jit_indirect();
	jit_prolog();
	jit_reti(29);
	jit_epilog();
	if (jump)
	    jit_patch_at(jump, first);
	code = jit_emit();
	assert(code != NULL);
	entry = code;
	function1 = jit_address(first);
	function2 = jit_address(second);
	jit_clear_state();
	assert((*entry)() == 11);
	assert((*function1)() == 11);
	assert((*function2)() == 29);

	_jit = jit_new_state();
	jit_prolog();
	jit_prepare();
	jit_finishi(function2);
	jit_retval(JIT_R0);
	jit_addi(JIT_R0, JIT_R0, 7);
	jit_retr(JIT_R0);
	jit_epilog();
	caller = jit_emit();
	assert(caller != NULL);
	jit_clear_state();
	assert((*caller)() == 36);
	jit_destroy_state();
	_jit = entries;
	jit_destroy_state();
    }
    finish_jit();
    return (0);
}
