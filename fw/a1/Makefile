# 5vcomp comes from: https://github.com/peterzieba/5Vpld
# See instructions on github for how to install
# WinCUPL in wine, and how to add the latest fitters
CUPL=./5vcomp

IKSI   = Iksi/iksi_240418.jed
NARNIA = Narnia/narnia_240422.jed
NESSI  = Nessi/nessi_wip.jed

DEPS	= $(CUPL) Makefile
OPTS	= m3jn

all		: iksi narnia nessi
iksi	: $(DEPS) $(IKSI)
narnia 	: $(DEPS) $(NARNIA)
nessi  	: $(DESP) $(NESSI)

.PHONY: clean
clean:
	rm -f $(IKSI) $(NARNIA) $(NESSI)
	rm -f Nessi/*.abs Nessi/*.doc Nessi/*.fit Nessi/*.io
	rm -f Nessi/*.lst Nessi/*.pdf Nessi/*.pin Nessi/*.pla
	rm -f Nessi/*.sim Nessi/*.tt2 Nessi/*.tt3 Nessi/*.log

%.jed : %.pld
	CUPL_OPTIONS=$(OPTS) $(CUPL)	$<
