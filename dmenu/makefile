# Desktop menu

GCC	= ~/bin/mingw32/bin/i686-pc-mingw32-g++
RC	= ~/bin/mingw32/bin/i686-pc-mingw32-windres
cflags	= -mwindows -Wall
efile	= dmenu.exe
ofiles	= main.o
rfiles	= $(ofiles:.o=.res)
rflags	= -O coff
lflags	= -mwindows -Wall -Wl



all: $(efile)



$(efile): $(ofiles) $(rfiles)
	$(GCC) $(lflags) -o $@ $+


$(rfiles): main.rc
	$(RC) $(rflags) -o $@ $+


release:
	rm $(ofiles) $(rfiles) *~

clean: release
	rm $(efile)

.cc.o:
	$(GCC) $(cflags) -c -o $@ $+

.rc.res:
	$(RC) $(rflags) -o $@ $+
