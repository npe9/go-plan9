#include "runtime.h"
#include "malloc.h"
#include "defs.h"

#line 13 "malloc.cgo"
MHeap mheap; 
MStats mstats; 
#line 19 "malloc.cgo"
void* 
mallocgc ( uintptr size , uint32 refflag , int32 dogc ) 
{ 
int32 sizeclass; 
MCache *c; 
uintptr npages; 
MSpan *s; 
void *v; 
uint32 *ref; 
#line 29 "malloc.cgo"
if ( m->mallocing ) 
throw ( "malloc/free - deadlock" ) ; 
m->mallocing = 1; 
#line 33 "malloc.cgo"
if ( size == 0 ) 
size = 1; 
#line 36 "malloc.cgo"
mstats.nmalloc++; 
if ( size <= MaxSmallSize ) { 
#line 39 "malloc.cgo"
sizeclass = SizeToClass ( size ) ; 
size = class_to_size[sizeclass]; 
c = m->mcache; 
v = MCache_Alloc ( c , sizeclass , size ) ; 
if ( v == nil ) 
throw ( "out of memory" ) ; 
mstats.alloc += size; 
} else { 
#line 50 "malloc.cgo"
npages = size >> PageShift; 
if ( ( size & PageMask ) != 0 ) 
npages++; 
s = MHeap_Alloc ( &mheap , npages , 0 ) ; 
if ( s == nil ) 
throw ( "out of memory" ) ; 
mstats.alloc += npages<<PageShift; 
v = ( void* ) ( s->start << PageShift ) ; 
} 
#line 61 "malloc.cgo"
if ( !mlookup ( v , nil , nil , &ref ) ) { 
printf ( "malloc %D; mlookup failed\n" , ( uint64 ) size ) ; 
throw ( "malloc mlookup" ) ; 
} 
*ref = RefNone | refflag; 
#line 67 "malloc.cgo"
m->mallocing = 0; 
#line 69 "malloc.cgo"
if ( dogc && mstats.inuse_pages > mstats.next_gc ) 
gc ( 0 ) ; 
return v; 
} 
#line 74 "malloc.cgo"
void* 
malloc ( uintptr size ) 
{ 
return mallocgc ( size , 0 , 0 ) ; 
} 
#line 81 "malloc.cgo"
void 
free ( void *v ) 
{ 
int32 sizeclass , size; 
uintptr page , tmp; 
MSpan *s; 
MCache *c; 
uint32 *ref; 
#line 90 "malloc.cgo"
if ( v == nil ) 
return; 
#line 93 "malloc.cgo"
if ( m->mallocing ) 
throw ( "malloc/free - deadlock" ) ; 
m->mallocing = 1; 
#line 97 "malloc.cgo"
if ( !mlookup ( v , nil , nil , &ref ) ) 
throw ( "free mlookup" ) ; 
*ref = RefFree; 
#line 102 "malloc.cgo"
page = ( uintptr ) v >> PageShift; 
sizeclass = MHeapMapCache_GET ( &mheap.mapcache , page , tmp ) ; 
if ( sizeclass == 0 ) { 
#line 106 "malloc.cgo"
s = MHeap_Lookup ( &mheap , page ) ; 
if ( s == nil ) 
throw ( "free - invalid pointer" ) ; 
sizeclass = s->sizeclass; 
if ( sizeclass == 0 ) { 
#line 112 "malloc.cgo"
mstats.alloc -= s->npages<<PageShift; 
runtime_memclr ( v , s->npages<<PageShift ) ; 
MHeap_Free ( &mheap , s ) ; 
goto out; 
} 
MHeapMapCache_SET ( &mheap.mapcache , page , sizeclass ) ; 
} 
#line 121 "malloc.cgo"
c = m->mcache; 
size = class_to_size[sizeclass]; 
runtime_memclr ( v , size ) ; 
mstats.alloc -= size; 
MCache_Free ( c , v , sizeclass , size ) ; 
#line 127 "malloc.cgo"
out: 
m->mallocing = 0; 
} 
#line 131 "malloc.cgo"
int32 
mlookup ( void *v , byte **base , uintptr *size , uint32 **ref ) 
{ 
uintptr n , nobj , i; 
byte *p; 
MSpan *s; 
#line 138 "malloc.cgo"
mstats.nlookup++; 
s = MHeap_LookupMaybe ( &mheap , ( uintptr ) v>>PageShift ) ; 
if ( s == nil ) { 
if ( base ) 
*base = nil; 
if ( size ) 
*size = 0; 
if ( ref ) 
*ref = 0; 
return 0; 
} 
#line 150 "malloc.cgo"
p = ( byte* ) ( ( uintptr ) s->start<<PageShift ) ; 
if ( s->sizeclass == 0 ) { 
#line 153 "malloc.cgo"
if ( base ) 
*base = p; 
if ( size ) 
*size = s->npages<<PageShift; 
if ( ref ) 
*ref = &s->gcref0; 
return 1; 
} 
#line 162 "malloc.cgo"
if ( ( byte* ) v >= ( byte* ) s->gcref ) { 
#line 165 "malloc.cgo"
return 0; 
} 
#line 168 "malloc.cgo"
n = class_to_size[s->sizeclass]; 
i = ( ( byte* ) v - p ) /n; 
if ( base ) 
*base = p + i*n; 
if ( size ) 
*size = n; 
nobj = ( s->npages << PageShift ) / ( n + RefcountOverhead ) ; 
if ( ( byte* ) s->gcref < p || ( byte* ) ( s->gcref+nobj ) > p+ ( s->npages<<PageShift ) ) { 
printf ( "odd span state=%d span=%p base=%p sizeclass=%d n=%D size=%D npages=%D\n" , 
s->state , s , p , s->sizeclass , ( uint64 ) nobj , ( uint64 ) n , ( uint64 ) s->npages ) ; 
printf ( "s->base sizeclass %d v=%p base=%p gcref=%p blocksize=%D nobj=%D size=%D end=%p end=%p\n" , 
s->sizeclass , v , p , s->gcref , ( uint64 ) s->npages<<PageShift , 
( uint64 ) nobj , ( uint64 ) n , s->gcref + nobj , p+ ( s->npages<<PageShift ) ) ; 
throw ( "bad gcref" ) ; 
} 
if ( ref ) 
*ref = &s->gcref[i]; 
#line 186 "malloc.cgo"
return 1; 
} 
#line 189 "malloc.cgo"
MCache* 
allocmcache ( void ) 
{ 
return FixAlloc_Alloc ( &mheap.cachealloc ) ; 
} 
#line 195 "malloc.cgo"
void 
mallocinit ( void ) 
{ 
InitSizes ( ) ; 
MHeap_Init ( &mheap , SysAlloc ) ; 
m->mcache = allocmcache ( ) ; 
#line 203 "malloc.cgo"
free ( malloc ( 1 ) ) ; 
} 
#line 206 "malloc.cgo"
void* 
SysAlloc ( uintptr n ) 
{ 
void *p; 
#line 211 "malloc.cgo"
mstats.sys += n; 
p = runtime_mmap ( nil , n , PROT_READ|PROT_WRITE|PROT_EXEC , MAP_ANON|MAP_PRIVATE , -1 , 0 ) ; 
if ( p < ( void* ) 4096 ) { 
if ( p == ( void* ) EACCES ) { 
printf ( "mmap: access denied\n" ) ; 
printf ( "If you're running SELinux , enable execmem for this process.\n" ) ; 
} else { 
printf ( "mmap: errno=%p\n" , p ) ; 
} 
exit ( 2 ) ; 
} 
return p; 
} 
#line 225 "malloc.cgo"
void 
SysUnused ( void *v , uintptr n ) 
{ 
USED ( v ) ; 
USED ( n ) ; 
#line 231 "malloc.cgo"
} 
#line 233 "malloc.cgo"
void 
SysFree ( void *v , uintptr n ) 
{ 
USED ( v ) ; 
USED ( n ) ; 
#line 239 "malloc.cgo"
} 
#line 243 "malloc.cgo"
void* 
mal ( uint32 n ) 
{ 
return mallocgc ( n , 0 , 1 ) ; 
} 
#line 256 "malloc.cgo"
struct { 
Lock; 
FixAlloc; 
} stacks; 
#line 261 "malloc.cgo"
void* 
stackalloc ( uint32 n ) 
{ 
void *v; 
uint32 *ref; 
#line 267 "malloc.cgo"
if ( m->mallocing || m->gcing ) { 
lock ( &stacks ) ; 
if ( stacks.size == 0 ) 
FixAlloc_Init ( &stacks , n , SysAlloc , nil , nil ) ; 
if ( stacks.size != n ) { 
printf ( "stackalloc: in malloc , size=%D want %d" , ( uint64 ) stacks.size , n ) ; 
throw ( "stackalloc" ) ; 
} 
v = FixAlloc_Alloc ( &stacks ) ; 
unlock ( &stacks ) ; 
return v; 
} 
v = malloc ( n ) ; 
if ( !mlookup ( v , nil , nil , &ref ) ) 
throw ( "stackalloc mlookup" ) ; 
*ref = RefStack; 
return v; 
} 
#line 286 "malloc.cgo"
void 
stackfree ( void *v ) 
{ 
if ( m->mallocing || m->gcing ) { 
lock ( &stacks ) ; 
FixAlloc_Free ( &stacks , v ) ; 
unlock ( &stacks ) ; 
return; 
} 
free ( v ) ; 
} 
void
malloc·Alloc(uintptr n, byte* p)
{
#line 298 "malloc.cgo"

	p = malloc(n);
	FLUSH(&p);
}
void
malloc·Free(byte* p)
{
#line 302 "malloc.cgo"

	free(p);
}
void
malloc·Lookup(byte* p, byte* base, uintptr size)
{
#line 306 "malloc.cgo"

	mlookup(p, &base, &size, nil);
	FLUSH(&base);
	FLUSH(&size);
}
void
malloc·GetStats(MStats* s)
{
#line 310 "malloc.cgo"

	s = &mstats;
	FLUSH(&s);
}
void
malloc·GC()
{
#line 314 "malloc.cgo"

	gc(1);
}
