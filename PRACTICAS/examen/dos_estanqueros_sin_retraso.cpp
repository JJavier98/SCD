#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;

Semaphore 	estanquero_ingrediente[2][3] = {0,0,0,0,0,0},
          	nadie_fuma = 1;



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

int producir_ingrediente()
{
	chrono::milliseconds duracion_producir( aleatorio<20,200>() );
	this_thread::sleep_for( duracion_producir );
   	return aleatorio<0,2>();
}

//----------------------------------------------------------------------
// función que ejecuta la hebra del estanquero

void funcion_hebra_estanquero( int num_estanquero )
{
	while( true )
	{
	    int ingrediente = producir_ingrediente();
      cout << "NADIE FUMA: " << nadie_fuma << "       ";
	    nadie_fuma.sem_wait();
      cout << "NADIE FUMA: " << nadie_fuma << "       ";
	    cout << "Estanquero número: " << num_estanquero << " produce ingrediente: " << ingrediente << endl;
	    estanquero_ingrediente[num_estanquero][ingrediente].sem_signal();
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
void  funcion_hebra_fumador( int num_estanquero, int num_fumador )
{
   while( true )
   {
   	estanquero_ingrediente[num_estanquero][num_fumador].sem_wait();
   	cout << "Retirado ingrediente: " << num_fumador << " del estanquero " << num_estanquero << endl;

    /*
      Suponemos que el segundo estanquero
      no repone el ingrediente hasta que 
      no termina de fumar la persona a la
      que ha abastecido.
      En cambio el primer estanquero 
      reanuda la producción en cuanto le 
      retiran el ingrediente.
    */

    if (num_estanquero == 0)
    {
      nadie_fuma.sem_signal();
      fumar( num_fumador );
    }
    else
    {
      fumar( num_fumador );
      nadie_fuma.sem_signal();
    }
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
   	thread hebras_consumidoras[2][3];

    for(int i = 0 ; i < 2 ; ++i)
    {
      hebras_productoras[i] = thread (funcion_hebra_estanquero, i);
      for(int j = 0 ; j < 3 ; ++j)
      {
      	hebras_consumidoras[i][j] = thread (funcion_hebra_fumador, i, j);
      }
    }
   	
    for(int i = 0 ; i < 2 ; ++i)
    {
      hebras_productoras[i].join();
      for(int j = 0 ; j < 3 ; ++j)
      {
      	hebras_consumidoras[i][j].join();
      }
    }
   	

	cout << "FIN" << endl;
}
