CC	= /usr/bin/x86_64-w64-mingw32-gcc
cflags	= -Wall -Werror
lflags	= -mwindows

cfiles	= main.c
ofiles	= $(cfiles:.c=.o)
efile	= hello.exe

outdir	= /media/sf_share/wintools/
outfile	= $(outdir)/$(efile)


all: $(outfile)

$(outfile): $(efile)
	cp $< $@

$(efile): $(ofiles)
	$(CC) $(lflags) $< -o $@

clean:
	rm $(ofiles)

dist-clean: clean
	rm $(outfile) $(efile)

.c.o:
	$(CC) -c $(cflags) $< -o $@
