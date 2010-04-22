#include "runtime.h"

#line 7 "string.cgo"
String emptystring; 
#line 9 "string.cgo"
int32 
findnull ( byte *s ) 
{ 
int32 l; 
#line 14 "string.cgo"
if ( s == nil ) 
return 0; 
for ( l=0; s[l]!=0; l++ ) 
; 
return l; 
} 
#line 21 "string.cgo"
int32 maxstring; 
#line 23 "string.cgo"
String 
gostringsize ( int32 l ) 
{ 
String s; 
#line 28 "string.cgo"
if ( l == 0 ) 
return emptystring; 
s.str = mal ( l+1 ) ; 
s.len = l; 
if ( l > maxstring ) 
maxstring = l; 
return s; 
} 
#line 37 "string.cgo"
String 
gostring ( byte *str ) 
{ 
int32 l; 
String s; 
#line 43 "string.cgo"
l = findnull ( str ) ; 
s = gostringsize ( l ) ; 
mcpy ( s.str , str , l ) ; 
return s; 
} 
void
runtime·catstring(String s1, String s2, String s3)
{
#line 49 "string.cgo"

	if(s1.len == 0) {
		s3 = s2;
		goto out;
	}
	if(s2.len == 0) {
		s3 = s1;
		goto out;
	}

	s3 = gostringsize(s1.len + s2.len);
	mcpy(s3.str, s1.str, s1.len);
	mcpy(s3.str+s1.len, s2.str, s2.len);
out:
	FLUSH(&s3);
}

#line 65 "string.cgo"
static void 
prbounds ( int8* s , int32 a , int32 b , int32 c ) 
{ 
prints ( s ) ; 
prints ( " " ) ; 
runtime·printint ( a ) ; 
prints ( "<" ) ; 
runtime·printint ( b ) ; 
prints ( ">" ) ; 
runtime·printint ( c ) ; 
prints ( "\n" ) ; 
throw ( "string bounds" ) ; 
} 
#line 79 "string.cgo"
uint32 
cmpstring ( String s1 , String s2 ) 
{ 
uint32 i , l; 
byte c1 , c2; 
#line 85 "string.cgo"
l = s1.len; 
if ( s2.len < l ) 
l = s2.len; 
for ( i=0; i<l; i++ ) { 
c1 = s1.str[i]; 
c2 = s2.str[i]; 
if ( c1 < c2 ) 
return -1; 
if ( c1 > c2 ) 
return +1; 
} 
if ( s1.len < s2.len ) 
return -1; 
if ( s1.len > s2.len ) 
return +1; 
return 0; 
} 
void
runtime·cmpstring(String s1, String s2, int32 v)
{
#line 103 "string.cgo"

	v = cmpstring(s1, s2);
	FLUSH(&v);
}

#line 107 "string.cgo"
int32 
strcmp ( byte *s1 , byte *s2 ) 
{ 
uint32 i; 
byte c1 , c2; 
#line 113 "string.cgo"
for ( i=0;; i++ ) { 
c1 = s1[i]; 
c2 = s2[i]; 
if ( c1 < c2 ) 
return -1; 
if ( c1 > c2 ) 
return +1; 
if ( c1 == 0 ) 
return 0; 
} 
} 
void
runtime·slicestring(String si, int32 lindex, int32 hindex, String so)
{
#line 125 "string.cgo"

	int32 l;

	if(lindex < 0 || lindex > si.len ||
	   hindex < lindex || hindex > si.len) {
		runtime·printpc(&si);
		prints(" ");
		prbounds("slice", lindex, si.len, hindex);
	}

	l = hindex-lindex;
	so.str = si.str + lindex;
	so.len = l;

//	alternate to create a new string
//	so = gostringsize(l);
//	mcpy(so.str, si.str+lindex, l);
	FLUSH(&so);
}
void
runtime·slicestring1(String si, int32 lindex, String so)
{
#line 144 "string.cgo"

	int32 l;

	if(lindex < 0 || lindex > si.len) {
		runtime·printpc(&si);
		prints(" ");
		prbounds("slice", lindex, si.len, si.len);
	}

	l = si.len-lindex;
	so.str = si.str + lindex;
	so.len = l;

//	alternate to create a new string
//	so = gostringsize(l);
//	mcpy(so.str, si.str+lindex, l);
	FLUSH(&so);
}
void
runtime·indexstring(String s, int32 i, byte b)
{
#line 162 "string.cgo"

	if(i < 0 || i >= s.len) {
		runtime·printpc(&s);
		prints(" ");
		prbounds("index", 0, i, s.len);
	}

	b = s.str[i];
	FLUSH(&b);
}
void
runtime·intstring(int64 v, String s)
{
#line 172 "string.cgo"

	s = gostringsize(8);
	s.len = runetochar(s.str, v);
	FLUSH(&s);
}
void
runtime·slicebytetostring(Slice b, String s)
{
#line 177 "string.cgo"

	s = gostringsize(b.len);
	mcpy(s.str, b.array, s.len);
	FLUSH(&s);
}
void
runtime·sliceinttostring(Slice b, String s)
{
#line 183 "string.cgo"

	int32 siz1, siz2, i;
	int32 *a;
	byte dum[8];

	a = (int32*)b.array;
	siz1 = 0;
	for(i=0; i<b.len; i++) {
		siz1 += runetochar(dum, a[i]);
	}

	s = gostringsize(siz1+4);
	siz2 = 0;
	for(i=0; i<b.len; i++) {
		// check for race
		if(siz2 >= siz1)
			break;
		siz2 += runetochar(s.str+siz2, a[i]);
	}
	s.len = siz2;
	FLUSH(&s);
}

#line 205 "string.cgo"
enum 
{ 
Runeself = 0x80 , 
} ; 
void
runtime·stringiter(String s, int32 k, int32 retk)
{
#line 210 "string.cgo"

	int32 l;

	if(k >= s.len) {
		// retk=0 is end of iteration
		retk = 0;
		goto out;
	}

	l = s.str[k];
	if(l < Runeself) {
		retk = k+1;
		goto out;
	}

	// multi-char rune
	retk = k + charntorune(&l, s.str+k, s.len-k);

out:
	FLUSH(&retk);
}
void
runtime·stringiter2(String s, int32 k, int32 retk, int32 retv)
{
#line 231 "string.cgo"

	if(k >= s.len) {
		// retk=0 is end of iteration
		retk = 0;
		retv = 0;
		goto out;
	}

	retv = s.str[k];
	if(retv < Runeself) {
		retk = k+1;
		goto out;
	}

	// multi-char rune
	retk = k + charntorune(&retv, s.str+k, s.len-k);

out:
	FLUSH(&retk);
	FLUSH(&retv);
}
