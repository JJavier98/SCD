// -----------------------------------------------------------------------------
//
// Sistemas concurrentes y Distribuidos.
// Práctica 3. Implementación de algoritmos distribuidos con MPI
//
// Archivo: prodcons2.cpp
// Implementación del problema del productor-consumidor con
// un proceso intermedio que gestiona un buffer finito y recibe peticiones
// en orden arbitrario
// (versión con un único productor y un único consumidor)
//
// Historial:
// Actualizado a C++11 en Septiembre de 2017
// -----------------------------------------------------------------------------

#include <iostream>
#include <thread> // this_thread::sleep_for
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include <mpi.h>

using namespace std;
using namespace std::this_thread ;
using namespace std::chrono ;

const int
//   id_productor          = 0 ,
//   id_buffer             = 1 ,
//   id_consumidor         = 2 ,
   np                    = 4 ,            // nº de procesos productores
   nc                    = 5 ,            // nº de procesos consumidores
   num_procesos_esperado = nc + np + 1 ,  // +1 por el proceso del buffer (con id = np)
   id_buffer             = np,
   num_items             = 10,            // debe ser multiplo de nc y np
   tam_vector            = 10,
   etiq_prod             = 0,
   //etiq_prod_impar       = 1,
   etiq_cons_par         = 2,
   etiq_cons_impar       = 3,
   etiq_buffer           = 4;

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
// ---------------------------------------------------------------------
// ptoducir produce los numeros en secuencia (1,2,3,....)
// y lleva espera aleatorio
int producir(int num_orden)
{
   static int contador = num_orden * (num_items / np) ;
   sleep_for( milliseconds( aleatorio<10,100>()) );
   contador++ ;
   cout << "Productor " << num_orden << " ha producido valor " << contador << endl << flush;
   return contador ;
}
// ---------------------------------------------------------------------

void funcion_productor(int num_orden)
{
   for ( unsigned int i= 0 ; i < num_items/np ; i++ )
   {
      // producir valor
      int valor_prod = producir(num_orden);
      // enviar valor
      cout << "Productor " << num_orden << " va a enviar valor " << valor_prod << endl << flush;
      MPI_Ssend( &valor_prod, 1, MPI_INT, id_buffer, etiq_prod, MPI_COMM_WORLD );
   }
}
// ---------------------------------------------------------------------

void consumir( int valor_cons, int num_orden )
{
   // espera bloqueada
   sleep_for( milliseconds( aleatorio<110,200>()) );
   cout << "Consumidor " << num_orden << " ha consumido valor " << valor_cons << endl << flush ;
}
// ---------------------------------------------------------------------

void funcion_consumidor(int num_orden)
{
    int peticion = num_orden;
    int valor_rec = -1 ;
    MPI_Status  estado ;

    int tag = num_orden %2 == 0? etiq_cons_par : etiq_cons_impar;

    for( unsigned int i=0 ; i < num_items/nc; i++ )
    {
        MPI_Ssend( &peticion,  1, MPI_INT, id_buffer, tag, MPI_COMM_WORLD);
        MPI_Recv ( &valor_rec, 1, MPI_INT, id_buffer, tag, MPI_COMM_WORLD,&estado );
        cout << "Consumidor " << num_orden << " ha recibido valor " << valor_rec << endl << flush ;
        consumir( valor_rec, num_orden );
    }
}
// ---------------------------------------------------------------------

void funcion_buffer()
{
   int        buffer[tam_vector],      // buffer con celdas ocupadas y vacías
              valor,                   // valor recibido o enviado
              primera_libre       = 0, // índice de primera celda libre
              primera_ocupada     = 0, // índice de primera celda ocupada
              num_celdas_ocupadas = 0, // número de celdas ocupadas
              tag_emisor_aceptable ,    // etiqueta de emisor aceptable
              marcador = 0,
              flag;

   MPI_Status estado ;                 // metadatos del mensaje recibido

   for( unsigned int i=0 ; i < num_items*2 ; i++ )
   {
      // 1. determinar si puede enviar solo prod., solo cons, o todos

        if ( num_celdas_ocupadas == 0 )                 // si buffer vacío
            tag_emisor_aceptable = etiq_prod ;          // $~~~$ solo prod.
        else if ( num_celdas_ocupadas == tam_vector )   // si buffer lleno
        {
            if(marcador%2 == 0)
                tag_emisor_aceptable = etiq_cons_par;   // $~~~$ solo cons.
            else
                tag_emisor_aceptable = etiq_cons_impar;
        }
        else                                            // si no vacío ni lleno
        {
            MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &estado);
            if(estado.MPI_TAG != etiq_prod)
            {
                if(marcador%2 == 0)
                    tag_emisor_aceptable = etiq_cons_par;   // $~~~$ solo cons.
                else
                    tag_emisor_aceptable = etiq_cons_impar;
            }
            else
                tag_emisor_aceptable = etiq_prod;
        }

      // 2. recibir un mensaje del emisor o emisores aceptables

        MPI_Recv( &valor, 1, MPI_INT, MPI_ANY_SOURCE, tag_emisor_aceptable, MPI_COMM_WORLD, &estado );

      // 3. procesar el mensaje recibido

      switch( estado.MPI_TAG ) // leer emisor del mensaje en metadatos
      {
        case etiq_prod: // si ha sido el productor: insertar en buffer

            buffer[primera_libre] = valor ;
            primera_libre = (primera_libre+1) % tam_vector ;
            num_celdas_ocupadas++ ;
            cout << "Buffer ha recibido valor " << valor << endl ;
            break;

        case etiq_cons_par: // si ha sido el consumidor par: extraer y enviarle

            valor = buffer[primera_ocupada] ;
            primera_ocupada = (primera_ocupada+1) % tam_vector ;
            num_celdas_ocupadas-- ;
            cout << "Buffer va a enviar valor " << valor << " a un consumidor PAR." << endl ;
            MPI_Ssend( &valor, 1, MPI_INT, estado.MPI_SOURCE, etiq_cons_par, MPI_COMM_WORLD);
            marcador++;
            break;

        case etiq_cons_impar: // si ha sido el consumidor par: extraer y enviarle

            valor = buffer[primera_ocupada] ;
            primera_ocupada = (primera_ocupada+1) % tam_vector ;
            num_celdas_ocupadas-- ;
            cout << "Buffer va a enviar valor " << valor << " a un consumidor IMPAR" << endl ;
            MPI_Ssend( &valor, 1, MPI_INT, estado.MPI_SOURCE, etiq_cons_impar, MPI_COMM_WORLD);
            marcador++;
            break;
      }
   }
}

// ---------------------------------------------------------------------

int main( int argc, char *argv[] )
{
   int id_propio, num_procesos_actual, num_orden_prod, num_orden_cons;

   // inicializar MPI, leer identif. de proceso y número de procesos
   MPI_Init( &argc, &argv );
   MPI_Comm_rank( MPI_COMM_WORLD, &id_propio );
   MPI_Comm_size( MPI_COMM_WORLD, &num_procesos_actual );

   if ( num_procesos_esperado == num_procesos_actual )
   {
      // ejecutar la operación apropiada a 'id_propio'
      if ( id_propio < np )
      {
        num_orden_prod = id_propio;
        funcion_productor(num_orden_prod);
      }
      else if ( id_propio == np )
      {
        funcion_buffer();
      }
      else
      {
        num_orden_cons = id_propio - np - 1;
        funcion_consumidor(num_orden_cons);
      }
   }
   else
   {
      if ( id_propio == 0 ) // solo el primero escribe error, indep. del rol
      { cout << "el número de procesos esperados es:    " << num_procesos_esperado << endl
             << "el número de procesos en ejecución es: " << num_procesos_actual << endl
             << "(programa abortado)" << endl ;
      }
   }

   // al terminar el proceso, finalizar MPI
   MPI_Finalize( );
   return 0;
}
