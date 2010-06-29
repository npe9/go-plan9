#include <u.h>
#include <libc.h>
#include <bio.h>

// Compile .go file, import data from .6 file, and generate C string version.

void
main(int argc, char **argv)
{
	char *name;
	char buf[1024], initfunc[1024], *s, *p, *q;
	Biobuf *bin;

	if(argc != 2) {
		fprint(2, "usage: mkbuiltin1 sys\n");
		fprint(2, "in file $1.6 s/PACKAGE/$1/\n");
		exits("usage");
	}

	name = argv[1];
	snprint(initfunc, sizeof(initfunc), "init_%s_function", name);

	snprint(buf, sizeof(buf), "%s.%s", name, getenv("O"));
	bin = Bopen(buf, OREAD);
	if(bin == nil)
		sysfatal("open: %r");
   
	// look for $$ that introduces imports
	for(; s = Brdstr(bin, '\n', 1); free(s))
		if(strstr(buf, "$$"))
			goto begin;
	sysfatal("did not find beginning of imports");

begin:
	print("char *%simport =\n", name);
	for(free(s); s = Brdstr(bin, '\n', 1); free(s)){
		if(strstr(buf, "$$")){
			free(s);
			goto end;
		}
		for(p=buf; *p==' ' || *p == '\t'; p++)
			;

		// cut out decl of init_$1_function - it doesn't exist
		if(strstr(buf, initfunc))
			continue;

		// sys.go claims to be in package PACKAGE to avoid
		// conflicts during "6g sys.go".  rename PACKAGE to $2.
		print("\t\"");
		while(q = strstr(p, "PACKAGE")) {
			*q = 0;
			print("%s", p);	// up to the substitution
			print("%s", name);	// the sub name
			p = q+7;		// continue with rest
		}

		print("%s\\n\"\n", p);
	}
	sysfatal("did not find end of imports\n");

end:
	print("\t\"$$\\n\";\n");
	exits("");
}
