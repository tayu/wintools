CC	= /usr/bin/x86_64-w64-mingw32-g++
cflags	= -Wall -Werror
lflags	= -mwindows

cfiles	= main.cpp
ofiles	= $(cfiles:.cpp=.o)
efile	= hardlink.exe

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

.cpp.o:
	$(CC) -c $(cflags) $< -o $@

