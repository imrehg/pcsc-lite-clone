# Process this file with automake to create Makefile.in.

EXTRA_DIST = ChangeLog.svn reconf

SUBDIRS = doc src utils m4

ChangeLog.svn:
	-svn2cl.sh --group-by-day --stdout --include-rev | perl -pe \
	's/  corcoran-guest/  David Corcoran/; \
	s/  corcoran/  David Corcoran/; \
	s/  aet-guest/  Antti Tapaninen/; \
	s/  rousseau/  Ludovic Rousseau/; \
	s/  giraud/  Jean-Luc Giraud/; \
	s+trunk/PCSC/++g;' > $@

