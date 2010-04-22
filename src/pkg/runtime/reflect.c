#include "runtime.h"
#include "type.h"

#line 8 "reflect.cgo"
static Type* 
gettype ( void *typ ) 
{ 
#line 15 "reflect.cgo"
return ( Type* ) ( ( void** ) typ - 2 ) ; 
} 
void
reflect·mapaccess(byte* map, byte* key, byte* val, bool pres)
{
#line 25 "reflect.cgo"

	mapaccess((Hmap*)map, key, val, &pres);
	FLUSH(&pres);
}
void
reflect·mapassign(byte* map, byte* key, byte* val)
{
#line 29 "reflect.cgo"

	mapassign((Hmap*)map, key, val);
}
void
reflect·maplen(byte* map, int32 len)
{
#line 33 "reflect.cgo"

	// length is first word of map
	len = *(uint32*)map;
	FLUSH(&len);
}
void
reflect·mapiterinit(byte* map, byte* it)
{
#line 38 "reflect.cgo"

	it = (byte*)mapiterinit((Hmap*)map);
	FLUSH(&it);
}
void
reflect·mapiternext(byte* it)
{
#line 42 "reflect.cgo"

	mapiternext((struct hash_iter*)it);
}
void
reflect·mapiterkey(byte* it, byte* key, bool ok)
{
#line 46 "reflect.cgo"

	ok = mapiterkey((struct hash_iter*)it, key);
	FLUSH(&ok);
}
void
reflect·makemap(byte* typ, byte* map)
{
#line 50 "reflect.cgo"

	MapType *t;

	t = (MapType*)gettype(typ);
	map = (byte*)makemap(t->key, t->elem, 0);
	FLUSH(&map);
}
void
reflect·makechan(byte* typ, uint32 size, byte* ch)
{
#line 61 "reflect.cgo"

	ChanType *t;

	// typ is a *runtime.ChanType, but the ChanType
	// defined in type.h includes an interface value header
	// in front of the raw ChanType.  the -2 below backs up
	// to the interface value header.
	t = (ChanType*)gettype(typ);
	ch = (byte*)makechan(t->elem, size);
	FLUSH(&ch);
}
void
reflect·chansend(byte* ch, byte* val, bool* pres)
{
#line 72 "reflect.cgo"

	chansend((Hchan*)ch, val, pres);
}
void
reflect·chanrecv(byte* ch, byte* val, bool* pres)
{
#line 76 "reflect.cgo"

	chanrecv((Hchan*)ch, val, pres);
}
void
reflect·chanclose(byte* ch)
{
#line 80 "reflect.cgo"

	chanclose((Hchan*)ch);
}
void
reflect·chanclosed(byte* ch, bool r)
{
#line 84 "reflect.cgo"

	r = chanclosed((Hchan*)ch);
	FLUSH(&r);
}
void
reflect·chanlen(byte* ch, int32 r)
{
#line 88 "reflect.cgo"

	r = chanlen((Hchan*)ch);
	FLUSH(&r);
}
void
reflect·chancap(byte* ch, int32 r)
{
#line 92 "reflect.cgo"

	r = chancap((Hchan*)ch);
	FLUSH(&r);
}
void
reflect·setiface(byte* typ, byte* x, byte* ret)
{
#line 101 "reflect.cgo"

	InterfaceType *t;

	t = (InterfaceType*)gettype(typ);
	if(t->mhdr.len == 0) {
		// already an empty interface
		*(Eface*)ret = *(Eface*)x;
		return;
	}
	if(((Eface*)x)->type == nil) {
		// can assign nil to any interface
		((Iface*)ret)->tab = nil;
		((Iface*)ret)->data = nil;
		return;
	}
	ifaceE2I((InterfaceType*)gettype(typ), *(Eface*)x, (Iface*)ret);
}
