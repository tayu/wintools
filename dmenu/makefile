# Desktop menu

GCC	= /usr/bin/x86_64-w64-mingw32-g++
RC	= /usr/bin/x86_64-w64-mingw32-windres
cflags	= -Wall -Werror
lflags	= -mwindows -static-libstdc++ -static-libgcc
rflags	= -O coff

cfiles	= main.cc
ofiles	= $(cfiles:.cc=.o)
efile	= dmenu.exe
rfiles	= $(cfiles:.cc=.res)

outdir	= /media/sf_share/wintools/
outfile	= $(outdir)/$(efile)


all: $(outfile)

$(outfile): $(efile)
	cp $< $@

$(efile): $(ofiles) $(rfiles)
	$(GCC) $(lflags) -o $@ $+

$(rfiles): main.rc
	$(RC) $(rflags) -o $@ $+

clean:
	rm $(ofiles) $(rfiles)

dist-clean: clean
	rm $(outfile) $(efile)

.cc.o:
	$(GCC) -c $(cflags) $< -o $@

.rc.res:
	$(RC) $(rflags) -o $@ $+
