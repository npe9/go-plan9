#include "runtime.h"
#include "defs.h"
#include "os.h"

void
dumpregs(Ureg *r)
{
	printf("ax     %x\n", r->ax);
	printf("bx     %x\n", r->bx);
	printf("cx     %x\n", r->cx);
	printf("dx     %x\n", r->dx);
	printf("di     %x\n", r->di);
	printf("si     %x\n", r->si);
	printf("bp     %x\n", r->bp);
	printf("sp     %x\n", r->sp);
	printf("pc     %x\n", r->pc);
	printf("flags  %x\n", r->flags);
	printf("cs      %x\n", r->cs);
	printf("fs      %x\n", r->fs);
	printf("gs      %x\n", r->gs);
}

#define	NDFLT	1	/* terminate after note */
extern void noted(int32);
extern int8 *strstr(int8*,int8*);
extern int32 notify(void (*f)(void*, int8*));

void
notehandler(void *v, int8 *note)
{
	Ureg *ureg;

	if(strstr(note, "sys: breakpoint") == 0 &&
		strstr(note, "sys: bad address") == 0 &&
  		strstr(note, "sys: odd address") == 0 &&
  		strstr(note, "sys: bad sys call") == 0 &&
  		strstr(note, "sys: odd stack") == 0 &&
  		strstr(note, "sys: fp: fptrap") == 0 &&
  		strstr(note, "sys: trap: trap") == 0)
		noted(NDFLT);
	if(panicking)	// traceback already printed
		exits("panicking in note handler");
	panicking = 1;

	ureg = v;

	printf("note %s\n", note);
	printf("Faulting address: %p\n", ureg->usp);
	printf("PC=%X\n", ureg->pc);
	printf("\n");

	if(gotraceback()){
		traceback((void*)ureg->pc, (void*)ureg->sp, m->curg);
		tracebackothers(m->curg);
		dumpregs(ureg);
	}
	breakpoint();
	exits(note);
}

/* sic */
void
initsig(void)
{
	notify(notehandler);
}
