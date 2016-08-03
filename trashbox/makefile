GCC	= /usr/bin/x86_64-w64-mingw32-gcc -mwindows
cflags	= -Wall -Werror
lflags	= -mwindows

cfiles	= main.c
ofiles	= $(cfiles:.c=.o)
efile	= trashbox.exe

outdir	= /media/sf_share/wintools/
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

.c.o:
	$(GCC) -c $(cflags) $< -o $@

