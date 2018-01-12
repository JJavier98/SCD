.SUFFIXES:
.PHONY: x1, x2, clean

compilador:=g++
opcionesc:= -std=c++11 -pthread -Wfatal-errors -I.
hmonsrcs:= HoareMonitor.hpp HoareMonitor.cpp 

x1: monitor_em
	./$<

x2: barrera1_sc
	./$<

x3: barrera2_sc
	./$<

x4: barrera2_su
	./$<

x5: prodcons1_sc
	./$<

x6: prodcons2_sc
	./$<

x7: prodcons2_su
	./$<

x8: fumadoresm_su
	./$<

x9: barbero
	./$<


barbero: barbero.cpp
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp

barbero_sabe: barbero_sabe.cpp
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp

fumadoresm_su: fumadoresm_su.cpp
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp

fumadores_monitor_su: fumadores_monitor_su.cpp
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp

fumadoresm_su_con_limite: fumadoresm_su_con_limite.cpp
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp

fumadoresm_su_con_limite_y_varios_estanqueros: fumadoresm_su_con_limite_y_varios_estanqueros.cpp
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp

fumadoresm_su_con_limite_y_varios_estanqueros_2i: fumadoresm_su_con_limite_y_varios_estanqueros_2i.cpp
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp

fumadoresm_su_con_limite_y_varios_estanqueros_y_reparto_equitativo: fumadoresm_su_con_limite_y_varios_estanqueros_y_reparto_equitativo.cpp
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp

monitor_em: monitor_em.cpp
	$(compilador) $(opcionesc)  -o $@ $<

barrera1_sc: barrera1_sc.cpp
	$(compilador) $(opcionesc)  -o $@ $<

barrera2_sc: barrera2_sc.cpp
	$(compilador) $(opcionesc)  -o $@ $<

barrera2_su: barrera2_su.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp

prodcons1_sc: prodcons1_sc.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $<

prodcons2_sc: prodcons2_sc.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $<

prodcons2_su: prodcons2_su.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $<  HoareMonitor.cpp

clean:
	rm -f monitor_em barrera1_sc barrera2_sc barrera2_su prodcons1_sc prodcons2_sc prodcons2_su
