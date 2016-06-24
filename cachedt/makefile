# Desktop menu

GCC	= ~/bin/mingw32/i686-pc-mingw32/bin/g++
RC	=
cflags	= -mwindows -Wall
efile	= cachedt.exe
ofiles	= main.o
rfiles	=
rflags	=
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
