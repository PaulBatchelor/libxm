#include <stdlib.h>
#include <soundpipe.h>
#include <sporth.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "xm.h"

#define DEBUG(...) do {	  \
		fprintf(stderr, __VA_ARGS__); \
		fflush(stderr); \
	} while(0)

#define DEBUG_ERR(...) do {	  \
		perror(__VA_ARGS__); \
		fflush(stderr); \
	} while(0)

#define FATAL(...) do {	  \
		fprintf(stderr, __VA_ARGS__); \
		fflush(stderr); \
		exit(1); \
	} while(0)

#define FATAL_ERR(...) do {	  \
		perror(__VA_ARGS__); \
		fflush(stderr); \
		exit(1); \
	} while(0)

/* user data. */
typedef struct {
    xm_context_t *ctx;
    sp_ftbl *chan;
    SPFLOAT *tick;
    SPFLOAT *bpm;
} sporth_xm_data;

static void create_context_from_file(xm_context_t** ctx, uint32_t rate, const char* filename) {
	int xmfiledes;
	off_t size;

	xmfiledes = open(filename, O_RDONLY);
	if(xmfiledes == -1) {
		DEBUG_ERR("Could not open input file");
		*ctx = NULL;
		return;
	}

	size = lseek(xmfiledes, 0, SEEK_END);
	if(size == -1) {
		close(xmfiledes);
		DEBUG_ERR("lseek() failed");
		*ctx = NULL;
		return;
	}

	/* NB: using a VLA here was a bad idea, as the size of the
	 * module file has no upper bound, whereas the stack has a
	 * very finite (and usually small) size. Using mmap bypasses
	 * the issue (at the cost of portability…). */
	char* data = mmap(NULL, size, PROT_READ, MAP_SHARED, xmfiledes, (off_t)0);
	if(data == MAP_FAILED)
		FATAL_ERR("mmap() failed");

	switch(xm_create_context_safe(ctx, data, size, rate)) {
		
	case 0:
		break;

	case 1:
		DEBUG("could not create context: module is not sane\n");
		*ctx = NULL;
		break;

	case 2:
		FATAL("could not create context: malloc failed\n");
		break;
		
	default:
		FATAL("could not create context: unknown error\n");
		break;
		
	}
	
	munmap(data, size);
	close(xmfiledes);
}
static int sporth_xm(plumber_data *pd, sporth_stack *stack, void **ud)
{
    sporth_xm_data *xm_d;
    const char *tick;
    const char *chan;
    const char *filename;
    const char *bpmvar;
    uint16_t tempo;
    uint16_t bpm;

    switch(pd->mode) {
        case PLUMBER_CREATE:
            if(sporth_check_args(stack, "ssss") != SPORTH_OK) {
                fprintf(stderr,"XM: not enough args\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            filename = sporth_stack_pop_string(stack);
            chan = sporth_stack_pop_string(stack);
            bpmvar = sporth_stack_pop_string(stack);
            tick = sporth_stack_pop_string(stack);
            xm_d = malloc(sizeof(sporth_xm_data));
            create_context_from_file(&xm_d->ctx, pd->sp->sr, filename);
            *ud = xm_d;
            if(xm_d->ctx == NULL) {
                stack->error++;
                return PLUMBER_NOTOK;
            }
            sp_ftbl_create(pd->sp, 
                    &xm_d->chan, 
                    xm_get_number_of_channels(xm_d->ctx));
            plumber_ftmap_add(pd, chan, xm_d->chan);
            plumber_create_var(pd, tick, &xm_d->tick);
            plumber_create_var(pd, bpmvar, &xm_d->bpm);
            break;
        case PLUMBER_INIT:
            sporth_stack_pop_string(stack);
            sporth_stack_pop_string(stack);
            sporth_stack_pop_string(stack);
            sporth_stack_pop_string(stack);
            break;

        case PLUMBER_COMPUTE:
            xm_d = *ud;
            if(xm_get_loop_count(xm_d->ctx) == 0) {
                xm_generate_slice(xm_d->ctx, 
                        xm_d->chan->tbl, 
                        xm_d->tick);
                xm_get_playing_speed(xm_d->ctx, &bpm, &tempo);
                *xm_d->bpm = bpm;
            }
            break;

        case PLUMBER_DESTROY:
            xm_d = *ud;
            xm_free_context(xm_d->ctx);
            free(xm_d);
            break;
    }
    return PLUMBER_OK;
}

plumber_dyn_func sporth_return_ugen()
{
    return sporth_xm;
}
