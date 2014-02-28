# sleleton

GCC	= ~/bin/mingw32/bin/i686-pc-mingw32-gcc
RC	= ~/bin/mingw32/bin/i686-pc-mingw32-windres
cflags	= -mwindows -Wall
efile	= bup.exe
ofiles	= main.o
rfiles	= $(ofiles:.o=.res)
rflags	= -O coff
lflags	= -mwindows



all: $(efile)



$(efile): $(ofiles)
	$(GCC) $(lflags) -o $@ $?


$(rfiles): main.rc
	$(RC) $(rflags) -o $@ $<


clean:
	rm $(efile) $(ofiles) $(rfiles)


.cpp.o:
	$(GCC) $(cflags) -c -o $@ $<

.rc.res:
	$(RC) $(rflags) -o $@ $<

