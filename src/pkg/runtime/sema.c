#include "runtime.h"

#line 22 "sema.cgo"
typedef struct Sema Sema; 
struct Sema 
{ 
uint32 *addr; 
G *g; 
Sema *prev; 
Sema *next; 
} ; 
#line 32 "sema.cgo"
static Sema *semfirst , *semlast; 
static Lock semlock; 
#line 35 "sema.cgo"
static void 
semqueue ( uint32 *addr , Sema *s ) 
{ 
s->addr = addr; 
s->g = nil; 
#line 41 "sema.cgo"
lock ( &semlock ) ; 
s->prev = semlast; 
s->next = nil; 
if ( semlast ) 
semlast->next = s; 
else 
semfirst = s; 
semlast = s; 
unlock ( &semlock ) ; 
} 
#line 52 "sema.cgo"
static void 
semdequeue ( Sema *s ) 
{ 
lock ( &semlock ) ; 
if ( s->next ) 
s->next->prev = s->prev; 
else 
semlast = s->prev; 
if ( s->prev ) 
s->prev->next = s->next; 
else 
semfirst = s->next; 
s->prev = nil; 
s->next = nil; 
unlock ( &semlock ) ; 
} 
#line 69 "sema.cgo"
static void 
semwakeup ( uint32 *addr ) 
{ 
Sema *s; 
#line 74 "sema.cgo"
lock ( &semlock ) ; 
for ( s=semfirst; s; s=s->next ) { 
if ( s->addr == addr && s->g ) { 
ready ( s->g ) ; 
s->g = nil; 
break; 
} 
} 
unlock ( &semlock ) ; 
} 
#line 89 "sema.cgo"
static void 
semsleep1 ( Sema *s ) 
{ 
lock ( &semlock ) ; 
s->g = g; 
unlock ( &semlock ) ; 
} 
#line 98 "sema.cgo"
static void 
semsleepundo1 ( Sema *s ) 
{ 
lock ( &semlock ) ; 
if ( s->g != nil ) { 
s->g = nil; 
} else { 
#line 109 "sema.cgo"
if ( g->readyonstop == 0 ) 
* ( int32* ) 0x555 = 555; 
g->readyonstop = 0; 
} 
unlock ( &semlock ) ; 
} 
#line 117 "sema.cgo"
static void 
semsleep2 ( Sema *s ) 
{ 
USED ( s ) ; 
g->status = Gwaiting; 
gosched ( ) ; 
} 
#line 125 "sema.cgo"
static int32 
cansemacquire ( uint32 *addr ) 
{ 
uint32 v; 
#line 130 "sema.cgo"
while ( ( v = *addr ) > 0 ) 
if ( cas ( addr , v , v-1 ) ) 
return 1; 
return 0; 
} 
#line 138 "sema.cgo"
void 
semacquire ( uint32 *addr ) 
{ 
Sema s; 
#line 144 "sema.cgo"
if ( cansemacquire ( addr ) ) 
return; 
#line 152 "sema.cgo"
semqueue ( addr , &s ) ; 
for ( ;; ) { 
semsleep1 ( &s ) ; 
if ( cansemacquire ( addr ) ) { 
semsleepundo1 ( &s ) ; 
break; 
} 
semsleep2 ( &s ) ; 
} 
semdequeue ( &s ) ; 
semwakeup ( addr ) ; 
} 
#line 165 "sema.cgo"
void 
semrelease ( uint32 *addr ) 
{ 
uint32 v; 
#line 170 "sema.cgo"
for ( ;; ) { 
v = *addr; 
if ( cas ( addr , v , v+1 ) ) 
break; 
} 
semwakeup ( addr ) ; 
} 
void
runtime·Semacquire(uint32* addr)
{
#line 178 "sema.cgo"

	semacquire(addr);
}
void
runtime·Semrelease(uint32* addr)
{
#line 182 "sema.cgo"

	semrelease(addr);
}
