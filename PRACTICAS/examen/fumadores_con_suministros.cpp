#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;

Semaphore 	fumadores[2] = {0,0},
          	nadie_fuma = 1,
            sin_suministros = 1,
            con_suministros = 0;



//**********************************************************************
/*
  Suponiendo que el programa es:
  2 estanqueros, cada uno con sus 3 fumadores diferentes
  Los estanqueros esperan a que el fumador acabe de fumar 
  para poner otro ingrediente, no a que lo coja.
*/
//----------------------------------------------------------------------

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

void funcion_hebra_productora()
{
  while(true)
  {
    sin_suministros.sem_wait();

    chrono::milliseconds duracion_producir( aleatorio<20,200>() );
    this_thread::sleep_for( duracion_producir );
    cout << "La cadena de producción ha preparado 10 ingredientes aleatorios" << endl;

    con_suministros.sem_signal();
  }
}

int ingrediente_aleat()
{
   	return aleatorio<0,2>();
}

//----------------------------------------------------------------------
// función que ejecuta la hebra del estanquero

void funcion_hebra_estanquero()
{
	while( true )
	{
    con_suministros.sem_wait();

    for(int i = 0 ; i < 10 ; ++i)
    {    
      int ingrediente = ingrediente_aleat();
      nadie_fuma.sem_wait();
      cout << "Estanquero produce ingrediente: " << ingrediente << endl;
      fumadores[ingrediente].sem_signal();
    }

    sin_suministros.sem_signal();
  }

}

//-------------------------------------------------------------------------
// Función que simula la acción de fumar, como un retardo aleatoria de la hebra

void fumar( int num_fumador )
{

   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_fumar( aleatorio<20,200>() );

   // informa de que comienza a fumar

    cout << "Fumador " << num_fumador << "  :"
          << " empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl;

   // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
   this_thread::sleep_for( duracion_fumar );

   // informa de que ha terminado de fumar

    cout << "Fumador " << num_fumador << "  : termina de fumar, comienza espera de ingrediente." << endl;

}

//----------------------------------------------------------------------
// función que ejecuta la hebra del fumador
void  funcion_hebra_fumador( int num_fumador )
{
   while( true )
   {
   	fumadores[num_fumador].sem_wait();
   	cout << "Retirado ingrediente: " << num_fumador << endl;
    
    nadie_fuma.sem_signal();
    fumar( num_fumador );
    
   }
}

//----------------------------------------------------------------------

int main()
{
   cout << "--------------------------" << endl
        << "Problema de los Fumadores." << endl
        << "--------------------------" << endl
        << flush ;

   	thread hebras_productoras[2];
   	thread hebras_consumidoras[2];

    for(int i = 0 ; i < 2 ; ++i)
    {
      hebras_consumidoras[i] = thread (funcion_hebra_fumador, i);
    }

    hebras_productoras[1] = thread (funcion_hebra_productora);
    hebras_productoras[0] = thread (funcion_hebra_estanquero);

    for(int i = 0 ; i < 2 ; ++i)
    {
      hebras_productoras[i].join();
      hebras_consumidoras[i].join();
    }
   	

	cout << "FIN" << endl;
}
