
all:

dep:Q:
	echo mkdep -r $INCLUDES $CFILES
	mkdep -r $INCLUDES $CFILES > mk.dep

< mk.dep
