.SUFFIXES:
.PHONY: pc
.PRECIOUS: %_exe

compilador:=g++

pc_lifo: prodcons-plantilla-lifo_exe
	./$<

pc_fifo: prodcons-plantilla-fifo_exe
	./$<

fu: fumadores-plantilla_exe
	./$<

exa: examen_exe
	./$<

dos_estanqueros: dos-estanqueros_exe
	./$<

%_exe: %.cpp
		$(compilador) -std=c++11 -I. -o $@ $< Semaphore.cpp -lpthread

clean:
	rm -f *_exe
