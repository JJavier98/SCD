#include <iostream>
#include <random>
#include "HoareMonitor.hpp"
//Falta poner las condiciones de las colas.
using namespace std;
using namespace HM;

const int FUMADORES = 27;
const int fumadores_ingrediente_0 = 20;
const int fumadores_ingrediente_1 = 5;
const int fumadores_ingrediente_2 = 2;
const int INGREDIENTES = 3;
const int ESTANQUEROS = 1;
const int A_PRODUCIR = 100;
//**********************************************************************
// plantilla de función para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilación)
//----------------------------------------------------------------------

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

// Función que simula la acción de producir, como un retardo aleatoria de la hebra

int ProducirIngrediente( )
{
	// Produce ingrediente aleatorio
	int producido = rand() % INGREDIENTES;
   // calcular milisegundos aleatorios de duración de la acción de producir)
   chrono::milliseconds duracion_producir( aleatorio<20,25>() );

 	// informa de que comienza a producir

    cout << "Se empieza a producir (" << duracion_producir.count() << " milisegundos)" << endl;
   // espera bloqueada un tiempo igual a 'duracion_producir' milisegundos
   this_thread::sleep_for( duracion_producir );

   // informa de que ha terminado de producirlo
   cout << "Producido ingrediente " << producido << endl;
	return producido;

}

//-------------------------------------------------------------------------
// Función que simula la acción de fumar, como un retardo aleatoria de la hebra

void fumar( int num_fumador )
{

   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_fumar( aleatorio<20,25>() );

   // informa de que comienza a fumar

    cout << "Fumador " << num_fumador << "  :"
          << " empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl;

   // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
   this_thread::sleep_for( duracion_fumar );

   // informa de que ha terminado de fumar

    cout << "Fumador " << num_fumador << "  : termina de fumar, comienza espera de ingrediente." << endl;

}

class Estanco : public HoareMonitor{
private:
	CondVar cola_fumador[INGREDIENTES];
	CondVar cola_estanqueros;
	int mostrador;
	int producidos;
	int consumidos; 
	bool fin_produccion;
public:
	Estanco ();
	bool ObtenerIngrediente( int num_fumador, int ingrediente);
	void PonerIngrediente(int num_estanquero, int ingrediente);
	void EsperaRecogerIngrediente();
	int getProducidos();
	int getConsumidos();
	bool getFinProduccion();
	void llamada_final();
	bool acabar();
	
};

Estanco::Estanco (){
	for (int i = 0; i < INGREDIENTES; ++i)
		cola_fumador[i] = newCondVar();
	cola_estanqueros = newCondVar();
	mostrador = -1;
	producidos = 0;
	consumidos = 0;
	fin_produccion = false;
}

bool Estanco::ObtenerIngrediente (int num_fumador, int ingrediente){
	if ( mostrador != ingrediente)
		cola_fumador[ingrediente].wait();
	if (consumidos < A_PRODUCIR && mostrador==ingrediente){
		mostrador = -1;
		cout << "El fumador " << num_fumador << " recoge el ingrediente " << ingrediente << endl;
		consumidos++;
		if (!cola_estanqueros.empty())
			cola_estanqueros.signal();
		return true;
	}
	else{
		if (!cola_fumador[ingrediente].empty())
			cola_fumador[ingrediente].signal();
		return false;	
	}
}

void Estanco::PonerIngrediente (int num_estanquero, int ingrediente){
	mostrador = ingrediente;
	cout << "El estanquero " << num_estanquero << " pone el ingrediente " << ingrediente << " en el mostrador." << endl;
	cola_fumador[ingrediente].signal();
}

void Estanco::EsperaRecogerIngrediente(){
	if (mostrador!=-1)
		cola_estanqueros.wait();
}

int Estanco::getProducidos(){
	return producidos;
}

int Estanco::getConsumidos(){
	return consumidos;
}
bool Estanco::getFinProduccion(){
	if (fin_produccion)
		return true;
	else 
		return false;
}
bool Estanco::acabar(){
	if (producidos == A_PRODUCIR){
		fin_produccion = true;	
	}
	else 
		producidos++;
	return fin_produccion;
}

void Estanco::llamada_final(){
	cout << "Se hace llamada final." << endl;////S
	for (int i = 0; i < INGREDIENTES; ++i)
		if (!cola_fumador[i].empty())
			cola_fumador[i].signal();
	if (!cola_estanqueros.empty())
		cola_estanqueros.signal();
		
}

void funcion_hebra_fumador(MRef <Estanco> monitor, int ingrediente, int num_fumador){
	while (monitor->getConsumidos() < A_PRODUCIR){
		if (monitor->ObtenerIngrediente (num_fumador,ingrediente))
			fumar(num_fumador);
	}
	monitor->llamada_final();
	cout << "El fumador " << num_fumador << " se va a casa." << endl;
}

void funcion_hebra_estanquero(MRef <Estanco> monitor, int num_estanquero){
	int ingrediente;
	while (!monitor->acabar()){
		ingrediente = ProducirIngrediente();
		monitor->PonerIngrediente (num_estanquero, ingrediente);
		monitor->EsperaRecogerIngrediente();	
	}
	monitor->llamada_final();
	cout << "El estanquero " << num_estanquero << " acaba su labor." << endl;
}

int main()
{
	cout << "--------------------------------------------------------" << endl
        << "Problema de mcuhos fumadores. 3 ingredientes y 3 estanqueros que producen 100. Monitor SU." << endl
        << "--------------------------------------------------------" << endl
        << flush ;
	
	MRef <Estanco> monitor = Create <Estanco> ();
	thread hebras_estanqueros[ESTANQUEROS];
	thread hebras_fumadores[FUMADORES];

	for (int i=0; i<ESTANQUEROS; i++)
		hebras_estanqueros[i] = thread (funcion_hebra_estanquero,monitor,i);

	int i = 0;
	while(i<fumadores_ingrediente_0){
		hebras_fumadores[i] = thread (funcion_hebra_fumador,monitor,0,i);
		i++;
	}
	while(i<fumadores_ingrediente_1+fumadores_ingrediente_0){
		hebras_fumadores[i] = thread (funcion_hebra_fumador,monitor,1,i);
		i++;
	}
	while(i<fumadores_ingrediente_1+fumadores_ingrediente_0+fumadores_ingrediente_2){
		hebras_fumadores[i] = thread (funcion_hebra_fumador,monitor,2,i);
		i++;
	}

	for (int j=0; j<FUMADORES; j++)
		hebras_fumadores[j].join();
	for (int j=0; j<ESTANQUEROS; j++)
		hebras_estanqueros[j].join();

	cout << "FIN" << endl;
	
}
