GCC	= /usr/bin/x86_64-w64-mingw32-g++
cflags	= -Wall -Werror
lflags	= -mwindows

cfiles	= main.cc
ofiles	= $(cfiles:.cc=.o)
efile	= cachedt.exe

outdir	= /media/sf_share/wintools
outfile	= $(outdir)/$(efile)


all: $(outfile)

$(outfile): $(efile)
	cp $< $@

$(efile): $(ofiles)
	$(GCC) $(lflags) $< -o $@

clean:
	rm $(ofiles)

dist-clean: clean
	rm $(outfile) $(efile)

.cc.o:
	$(GCC) -c $(cflags) $< -o $@

