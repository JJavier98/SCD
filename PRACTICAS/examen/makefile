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

dos_estanqueros_retraso: dos_estanqueros_retraso_exe
	./$<

dos_estanqueros_sin_retraso: dos_estanqueros_sin_retraso_exe
	./$<

fumadores_con_suministros: fumadores_con_suministros_exe
	./$<

%_exe: %.cpp
		$(compilador) -std=c++11 -I. -o $@ $< Semaphore.cpp -lpthread

clean:
	rm -f *_exe
