SHELL = /bin/sh
package = display-dhammapada
Package = $(package)
VERSION = 0.23

# ARCHITECTURE is for Linux packages only:
#ARCHITECTURE = i386
ARCHITECTURE=$(shell if test -x /usr/bin/dpkg; \
	then dpkg --print-architecture; else uname -m; fi \
	| sed -e 's/^.*i.86\>.*/i386/'; )

# flags for your C compiler:
OPT = -O2
WARN = -ansi -Wall -Wno-unused -Wno-parentheses
CFLAGS = $(OPT) $(WARN) $(DEFS) -I/usr/local/include
# if you get undefined reference to `iconv_open' add this:
# CFLAGS += -L/usr/local/lib -liconv
# Or you may get rid off locale support completely:
# CFLAGS += -DNO_LOCALE

# install files with this ownership:
owner=root
group=wheel

install_own = -o $(owner) -g $$Group
INSTALL_DATA = install -c    $(install_own) -m 664
INSTALL_MAN = $(INSTALL_DATA)
INSTALL_PROGRAM = install -c -s $(install_own) -m 775
INSTALL_SCRIPT =  install -c $(install_own) -m 775
MKDIR = install -d $(install_own) -m 2775
BIN      = display-dhammapada
BINshort = dp
CC      = cc
DP       = dhammapada-english-transl.txt
DPALT    = dhammapada-alternate.txt
DPPL     = dhammapada-polish-transl.txt
DEBDIST	= ../$(package)_$(VERSION)_$(ARCHITECTURE).changes \
	../$(package)_$(VERSION)_$(ARCHITECTURE).deb \
	../$(package)_$(VERSION).dsc \
	../$(package)_$(VERSION).tar.gz
DEBIAN	= debian/changelog debian/control debian/copyright debian/rules 
DOCS     = $(DP) $(DPALT) $(DPPL) $(Package)-$(VERSION).lsm changelog copyright INSTALL
EXAMPLES = examples/profile examples/xsession examples/amortips
MAN1	= $(Package).1
#GZIPDOCS = yes
SRCS    = $(BIN).c $(Package).spec.in Makefile INSTALL
PREFIX  = /usr/local
prefix  = ${PREFIX}
exec_prefix = $(prefix)
bindir  = $(exec_prefix)/bin
docsdir  = $(prefix)/share/doc/$(Package)
examplesdir = $(docsdir)/examples
mandir  = $(prefix)/man
datadir  = $(prefix)/share/$(Package)
# relative symlink from docsdir to datadir:
relDATADIR = ../../$(Package)
DEFS    = '-DDPPATHNAME="$(docsdir)/$(DP):$(datadir)/$(DP)"' '-DDPPATHNAME_ALT="$(docsdir)/$(DPALT):$(datadir)/$(DPALT)"' '-DDPPATHNAME_PL="$(docsdir)/$(DPPL):$(datadir)/$(DPPL)"' '-DVERSION="$(VERSION)"'

all:	$(BIN) $(MAN1)

$(BIN):

$(MAN1):	$(Package).1.in
	    sed -e's./usr/share/display-dhammapada.$(datadir).g;s./usr/share/doc/display-dhammapada.$(docsdir).g'	<$(Package).1.in >$(MAN1); \

clean:
	-rm  -R -f *.o *~ *.bak *.tmp.1 $(MAN1) 1 2 t tmp \#*\# errors* core* \
		a.out typescript debian/tmp debian/files* debian/substvars* \
		./stdin ./stdout $(BIN) $(BINshort) $(Package)-*.spec \
		FreeBSD_port/$(Package)/work

debdist $(DEBDIST):	$(DEBIAN) $(DOCS) $(EXAMPLES) $(MAN1) $(SRCS)
	test -r $$HOME/.pgp/secring.pgp
	dpkg-buildpackage

dist tar:	clean $(MAN1) $(Package)-$(VERSION).lsm $(Package)-$(VERSION).spec
	-rm -f *.o
	cd .. && tar -cvzf $(Package)-$(VERSION).tar.gz $(Package)-$(VERSION)/
	
# should not rm deb files
distclean: clean

disttest:	$(DEBDIST)
	cp $(addprefix ../$(package)_$(VERSION).,dsc tar.gz) /tmp/
	cd /tmp && tar -xvzf $(package)_$(VERSION).tar.gz
	cd /tmp/$(package)-$(VERSION) && \
			pmake install test uninstall
	make rpm
	rpm --install --force --nodeps /usr/src/redhat/RPMS/$(ARCHITECTURE)/$(Package)-$(VERSION)-0.$(ARCHITECTURE).rpm
	rpm -qisv $(Package)
	rm -R -f /tmp/$(package)-$(VERSION)
	cd /tmp && dpkg-source -x $(package)_$(VERSION).dsc
	cd /tmp/$(package)-$(VERSION) && \
		dpkg-buildpackage -B
	dpkg --contents /tmp/$(package)_$(VERSION)_$(ARCHITECTURE).deb
	-rpm --erase $(Package)
	dpkg --install  /tmp/$(package)_$(VERSION)_$(ARCHITECTURE).deb
	cd /tmp/$(package)-$(VERSION) && rm $(DEBDIST)
	rm -R -f /tmp/$(package)-$(VERSION)

install:	all $(DOCS) $(MAN1)
	set -o errexit; set -x; \
		for group in $(group) wheel staff bin root; \
		do if install -d -g $$group ./tstdir; \
		   then \
			Group=$$group; export Group; \
			rmdir ./tstdir; \
			break; \
		   fi; \
		done; \
	test -e $(bindir) || $(MKDIR)	$(bindir); \
	$(INSTALL_PROGRAM)	$(BIN)		$(bindir); \
	$(INSTALL_SCRIPT)	xdp		$(bindir); \
	if [ $(BINshort)_ != _ ]; \
	then \
		ln -f -s	$(BIN)		$(bindir)/$(BINshort) \
		|| ln -f	$(BIN)		$(bindir)/$(BINshort); \
	fi; \
	test -e $(datadir) || $(MKDIR)	$(datadir); \
	$(INSTALL_DATA)		$(DP)		$(datadir); \
	$(INSTALL_DATA)		$(DPALT)	$(datadir); \
	$(INSTALL_DATA)		$(DPPL)		$(datadir); \
	if [ _$$NODOCS = _ ]; then \
	    $(INSTALL_MAN)		$(MAN1)	$(mandir)/man1; \
	    test -e $(docsdir) || $(MKDIR)	$(docsdir); \
	    for f in $(DOCS); do \
		    test -h $$f || $(INSTALL_MAN) $$f	$(docsdir); \
	    done; \
	    ln -s -f copyright $(docsdir)/COPYING \
	    || ln -f copyright $(docsdir)/COPYING; \
	    ln -s -f INSTALL $(docsdir)/README \
	    || ln -f INSTALL $(docsdir)/README; \
	    test -e $(examplesdir) || $(MKDIR)	$(examplesdir); \
	    $(INSTALL_SCRIPT)	$(EXAMPLES)	$(examplesdir); \
	    if [ _$(GZIPDOCS) = _yes -a _$(NOMANCOMPRESS) = _ ]; then \
		    gzip --best --force --verbose $(docsdir)/changelog $(mandir)/man1/$(MAN1); \
		    suff=.gz; \
	    else \
		    suff=; \
	    fi; \
	    ln -f -s $(MAN1)$$suff $(mandir)/man1/xdp.1$$suff \
	    || ln -f $(MAN1)$$suff $(mandir)/man1/xdp.1$$suff; \
	    if [ $(BINshort)_ != _ ]; then \
		ln -f -s $(MAN1)$$suff $(mandir)/man1/$(BINshort).1$$suff \
		|| ln -f $(MAN1)$$suff $(mandir)/man1/$(BINshort).1$$suff; \
	    fi; \
	    if [ $(datadir) != $(docsdir) ]; \
	    then \
		    ln -f -s $(relDATADIR)/$(DP) $(docsdir) || ln -f $(relDATADIR)/$(DP) $(docsdir); \
		    ln -f -s $(relDATADIR)/$(DPALT) $(docsdir) || ln -f $(relDATADIR)/$(DPALT) $(docsdir); \
		    ln -f -s $(relDATADIR)/$(DPPL) $(docsdir) || ln -f $(relDATADIR)/$(DPPL) $(docsdir); \
	    fi; \
	fi

$(Package)-$(VERSION).lsm lsm:	$(Package).lsm.in
	-rm -f $(package)*.lsm $(Package)*.lsm
	[ $(VERSION)x != x ]
	sed -e 's/$${date}/'`LC_ALL=C date +%d%b%y |tr a-z A-Z`'/g; \
		s/$${version}/$(VERSION)/g'  $(Package).lsm.in >$(Package)-$(VERSION).lsm
	touch lsm

port:
	sed -n -e 's/^Description:[[:space:]]*//p' $(Package).lsm.in \
		> FreeBSD_port/$(Package)/pkg/COMMENT
	{ sed -n -e '/^Description:/,/^[^[:space:]]/!d; /^[[:space:]]/p' \
		$(Package).lsm.in; echo; \
		echo '- Ricardas Cepas <rch@WriteMe.Com>'; } \
		> FreeBSD_port/$(Package)/pkg/DESCR
	cd FreeBSD_port/$(Package)/ # && make makesum
	cd FreeBSD_port/$(Package)/work/$(Package)-$(VERSION) && make \
		&& make distclean \
		&& make group=wheel prefix=/tmp/$(Package) install
	{ find /tmp/$(Package) -type f -or -type l \
          |sed -e 's!^'/tmp/$(Package)/'!!'; \
	  echo @dirrm share/doc/display-dhammapada/examples; \
	  echo @dirrm share/doc/display-dhammapada; \
	  echo @dirrm share/display-dhammapada; \
	} > FreeBSD_port/$(Package)/pkg/PLIST



realclean: distclean
	-rm $(DEBDIST) *.lsm
	

$(Package)-$(VERSION).spec:	$(Package).spec.in
	-rm -f $(Package)*.spec
	[ $(VERSION)x != x ]
	Summary="`sed -n -e 's/^Description:[[:space:]]*//p' \
						$(Package).lsm.in`" \
	&& Copying="`sed -n -e 's/^Copying-policy:[[:space:]]*//p' \
						$(Package).lsm.in`" \
	&& sed -e 's/$${version}/$(VERSION)/g; s/$${summary}/'"$${Summary}"'/g; \
		 s/$${copying}/'"$${Copying}"/g \
		 $(Package).spec.in >tmp
	sed -n -e '1,/^%description/p' tmp >$@
	sed -n -e '/^Description:/,/^[^[:space:]]/!d; /^[[:space:]]/p' \
		$(Package).lsm.in >>$@
	sed -n -e '1,/^%description/!p' tmp >>$@
	rm -f tmp

rpm:		tar $(Package)-$(VERSION).spec
		cp ../$(Package)-$(VERSION).tar.gz /usr/src/redhat/SOURCES
		rpm --clean -v -ba $(Package)-$(VERSION).spec

rpmi:
		rpm -vv -bi --short-circuit $(Package)-$(VERSION).spec

rpmsign:
	test -r $$HOME/.pgp/secring.pgp
	rpm -vv --resign \
		/usr/src/redhat/SRPMS/$(Package)-$(VERSION)-*.src.rpm \
		/usr/src/redhat/RPMS/$(ARCHITECTURE)/$(Package)-$(VERSION)-*.$(ARCHITECTURE).rpm

rpmdist:	rpm rpmsign

test check:
	@echo Testing installation...
	$(bindir)/$(BIN)
	-$(bindir)/$(BINshort)
	$(bindir)/xdp
	@echo -n "Press <enter> ... "
	@sh -c 'read a'

uninstall:
	rm -f $(bindir)/$(BIN) $(bindir)/xdp $(bindir)/$(BINshort) $(datadir)/$(DP) $(datadir)/$(DPALT) $(datadir)/$(DPPL) \
		$(mandir)/man1/$(MAN1) $(mandir)/man1/xdp.1 \
		$(mandir)/man1/$(MAN1).gz $(mandir)/man1/xdp.1.gz
	if [ $(BINshort)_ != _ ]; then \
		rm -f $(mandir)/man1/$(BINshort).1 $(mandir)/man1/$(BINshort).1.gz; \
	fi
	-cd $(docsdir) \
		&& for f in $(DOCS); do rm -f $$f $$f.gz; done; rm -f COPYING README $(Package)-*.lsm
	-cd $(examplesdir) \
		&& for f in $(EXAMPLES); \
			do rm -f $${f##*/} $${f##*/}.gz; done
	-rmdir $(examplesdir) 
	-rmdir $(datadir) $(docsdir)

upload: upload_main #dist debdist ../$(Package)-$(VERSION).tar.gz $(Package)-$(VERSION).lsm
	[ $(VERSION)x != x ]
	[ "`rpm --checksig \
		/usr/src/redhat/SRPMS/$(Package)-$(VERSION)-*.src.rpm \
		/usr/src/redhat/RPMS/$(ARCHITECTURE)/$(Package)-$(VERSION)-*.$(ARCHITECTURE).rpm \
	| grep -c 'pgp md5 OK$$'`" = 2 ]
	set -x; xPWD=`pwd` && echo -e open -r -d 10 incoming.redhat.com\\n \
	   cd /libc6 \\n \
	   mput /usr/src/redhat/SRPMS/$(Package)-$(VERSION)-*.src.rpm \
	    /usr/src/redhat/RPMS/$(ARCHITECTURE)/$(Package)-$(VERSION)-*.rpm\\n \
	   dir $(Package)-$(VERSION)-0.src.rpm $(Package)-$(VERSION)-0.i386.rpm\\n \
	   redir \>$$xPWD/../$(Package)-$(VERSION).ftp.redhat.com\\n \
	| ncftp
#	dupload --to erlangen ../$(package)_$(VERSION)_$(ARCHITECTURE).changes
	echo -ne \\a
	sleep 1s
	echo -ne \\a

UPHOST=metalab.unc.edu
upload_main:
	set -x; xPWD=`pwd` && echo -e open -r -d 10 $(UPHOST)\\n \
	   cd /incoming/linux\\n \
	   lcd $$xPWD\\n \
	   mput $(Package)-$(VERSION).lsm\\n \
	   lcd ..\\n \
	   mput $(Package)-$(VERSION).tar.gz \\n \
	   dir $(Package)*\\n \
	   redir $(Package)* \>$(Package)-$(VERSION).$(UPHOST)\\n \
	| ncftp

# vi: set noexpandtab:

