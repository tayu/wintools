GCC	= ~/bin/mingw32/bin/i686-pc-mingw32-gcc -mwindows
cflags	= -Wall
lflags	=
TMP	= /tmp

efile	= trashbox.exe
ofiles	= main.o
cfiles	= main.c




all: $(efile)

$(efile): $(ofiles)
	$(GCC) $(lflags) $< -o $@

clean:
	rm $(efile) $(ofiles)

.c.o:
	$(GCC) -c $(cflags) $< -o $@

