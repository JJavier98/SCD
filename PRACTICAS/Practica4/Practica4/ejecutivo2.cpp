// -----------------------------------------------------------------------------
//
// Sistemas concurrentes y Distribuidos.
// Práctica 4. Implementación de sistemas de tiempo real.
//
// Archivo: ejecutivo2.cpp
// Implementación del segundo ejemplo de ejecutivo cíclico:
//
//   Datos de las tareas:
//   ---------------
//   Ta.    T     C
//   ---------------
//   A     500   100
//   B     500   150
//   C    1000   200
//   D    2000   240
//  -------------
//
//  Planificación (con Ts == 500 ms)
//  *-------*-------*-------*------*
//  | A C B | A B D | A C B | A B  |
//  *-------*-------*-------*------*
//
//
// Historial:
// Creado en Diciembre de 2017
// -----------------------------------------------------------------------------

#include <string>
#include <iostream> // cout, cerr
#include <thread>
#include <chrono>   // utilidades de tiempo
#include <ratio>    // std::ratio_divide

using namespace std ;
using namespace std::chrono ;
using namespace std::this_thread ;

// tipo para duraciones en segundos y milisegundos, en coma flotante:
typedef duration<float,ratio<1,1>>    seconds_f ;
typedef duration<float,ratio<1,1000>> milliseconds_f ;

// -----------------------------------------------------------------------------
// tarea genérica: duerme durante un intervalo de tiempo (de determinada duración)

void Tarea( const std::string & nombre, milliseconds tcomputo )
{
   cout << "   Comienza tarea " << nombre << " (C == " << tcomputo.count() << " ms.) ... " ;
   sleep_for( tcomputo );
   cout << "fin." << endl ;
}

// -----------------------------------------------------------------------------
// tareas concretas del problema:
                                                //Error +250

void TareaA() { Tarea( "A", milliseconds(100) );  }//+n
void TareaB() { Tarea( "B", milliseconds(150) );  }//+n
void TareaC() { Tarea( "C", milliseconds(200) );  }//+n
void TareaD() { Tarea( "D", milliseconds(240) );  }//+n

// -----------------------------------------------------------------------------
// implementación del ejecutivo cíclico:

int main( int argc, char *argv[] )
{
   // Ts = duración del ciclo secundario
   const milliseconds Ts( 500 );

   steady_clock::duration tiempo_ciclo;

   // ini_sec = instante de inicio de la iteración actual del ciclo secundario
   time_point<steady_clock> ini_sec = steady_clock::now();

   while( true ) // ciclo principal
   {
      cout << endl
           << "---------------------------------------" << endl
           << "Comienza iteración del ciclo principal." << endl ;

      for( int i = 1 ; i <= 4 ; i++ ) // ciclo secundario (4 iteraciones)
      {
         cout << endl << "Comienza iteración " << i << " del ciclo secundario." << endl ;

         switch( i )
         {
            case 1 : TareaA(); TareaC(); TareaB();
                     tiempo_ciclo = milliseconds(100+200+150);
                     break ;
            case 2 : TareaA(); TareaB(); TareaD();
                     tiempo_ciclo = milliseconds(100+150+240);
                     break ;
            case 3 : TareaA(); TareaC(); TareaB();
                     tiempo_ciclo = milliseconds(100+200+150);
                     break ;
            case 4 : TareaA(); TareaB();
                     tiempo_ciclo = milliseconds(100+150);
                     break ;
         }


         // fin_sec = instante final de la iteración actual del ciclo secundario
         time_point<steady_clock> fin_sec = steady_clock::now();
         // diferencia = duración real que ha tenido el ciclo secundario
         steady_clock::duration diferencia = fin_sec - ini_sec;

         /*_______________INFORMACIÓN EN PANTALLA_______________*/

         // Imprime la duración del ciclo secundario
         cout << "      *Duración ciclo secundario: " << milliseconds_f(Ts).count() << endl;
         // Imprime la duración ideal de la ejecucion de las tareas del ciclo secundario
         cout << "      *Duración ideal: " << milliseconds_f(tiempo_ciclo).count() << endl;
         // Imprime la duración real de la ejecucion de las tareas del ciclo secundario
         cout << "      *Duración real: " << milliseconds_f( diferencia ).count() << endl;
         // Imprime la diferencia entre la duración real e ideal de la ejecucion de las tareas del ciclo secundario
         cout << "      *Retraso: " << (milliseconds_f(diferencia)-milliseconds_f(tiempo_ciclo)).count() << endl;

         /*_______________CONDICIONES DE ABORCIÓN_______________*/

         // Si la diferencia sobrepasa el tiempo máximo de ciclo (250ms) abortamos
         /*
         if( milliseconds_f( diferencia ) >= Ts )
         {
            cout << "Has sobrepasado el tiempo de ciclo (250ms). Hay que abortar el programa." << endl;
            return 1;
         }
         */

         // Si la diferencia sobrepasa el tiempo máximo de ciclo más un retardo de 20ms (270ms) abortamos
         /*
         if( (milliseconds_f(diferencia) - milliseconds_f(20) ) >= Ts)
         {
            cout << "Has sobrepasado en +20ms el tiempo de ciclo (250ms +20). Hay que abortar el programa." << endl;
            return 1;
         }
         */

         // Si la diferencia sobrepasa el tiempo ideal de ejecucion de las tareas asociadas al ciclo actual en 20ms abortamos
         /*
         if( (milliseconds_f(diferencia) - milliseconds_f(20) ) >= tiempo_ciclo)
         {
            cout << "Has sobrepasado en +20ms el tiempo real de ejecución de las tareas. Hay que abortar el programa." << endl;
            return 1;
         }
         */

         // calcular el siguiente instante de inicio del ciclo secundario
         ini_sec += Ts ;

         // esperar hasta el inicio de la siguiente iteración del ciclo secundario
         sleep_until( ini_sec );
      }
   }
}
