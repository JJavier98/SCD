#include <iostream>
#include <cassert>
#include <iomanip>
#include <random>
#include <chrono> 
#include "HoareMonitor.hpp"
#include <vector>

using namespace std ;
using namespace HM ;



template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

class MbarberiaSU : public HoareMonitor{
  private:
  int num_clientes,
  		clienteActual,
	  	contador;
  CondVar activo,
          silla,
          sala_espera;
          
 std::vector<int> cola;
          

  public:
  MbarberiaSU(int clientes);
  void cortarPelo(int c);
  void siguienteCliente();
  void finCliente();

};


MbarberiaSU::MbarberiaSU(int clientes){
  
  num_clientes = clientes;
  contador = 0;

  activo = newCondVar();
  sala_espera = newCondVar();
  silla = newCondVar();

}


void MbarberiaSU::siguienteCliente(){	
   if(sala_espera.empty()){
	   contador++;
   		if(contador % 2 == 0) 
		     cout << "--------Me voy a dormir " << endl;
		else
			cout << "--------Me voy a afeitar yo mismo" << endl;
     activo.wait();
   }

   sala_espera.signal();

}


void MbarberiaSU::cortarPelo(int c){
  	cout << "Entra el cliente " << c << endl;

 	if(!silla.empty())
 	{
 		if(!activo.empty())
 		{
 			cout << "El cliente " << c << " despierta al barbero" << endl;
 			activo.signal();
 		}
 		cola.push_back(c);
 		//while(cola[0] != c)
	   	sala_espera.wait();
 	}
  	else if (!activo.empty())
  	{
  		cout << "El cliente " << c << " despierta al barbero" << endl;
    	activo.signal();
  	}
  	
  		clienteActual = c;
  		if(cola.size() != 0)
  		{  		
  			if(clienteActual != cola[0])
  				cout << "*****HA HABIDO UN FALLO****" << endl;
		
			cola.erase(cola.begin());
		}
	  	silla.wait();      
  
}


void MbarberiaSU::finCliente()
{
  	cout << "El cliente ha sido pelado" << endl;
  	silla.signal();
}


void CortarPeloACliente(){

	cout << "El barbero empieza a cortar " << endl;

	chrono::milliseconds duracion_corte( aleatorio<1000,3000>() );/*EXAMEN*/ // Hacemos que el tiempo que tarda el barbero en cortar el pelo sea altísimo respecto al tiempo que esperan los clientes

	this_thread::sleep_for( duracion_corte );
}


void EsperarFueraBarberia(int num){

  chrono::milliseconds duracion_crecimiento( aleatorio<10,200>() ); /*EXAMEN*/ // Hacemos que el tiempo que esperan los clientes sea altísimo respecto al tiempo que tarda el barbero en cortar el pelo

  cout << "El cliente nº "<< num << " espera a que le crezca de nuevo el pelo" << endl ;

  this_thread::sleep_for( duracion_crecimiento );

}


void Barbero(MRef<MbarberiaSU> Barberia){

  while(true){
    Barberia->siguienteCliente();
  
    CortarPeloACliente();

    Barberia->finCliente();
  }

}


void Cliente(MRef<MbarberiaSU> Barberia, int num){

  while(true){
   Barberia->cortarPelo(num);
 
   EsperarFueraBarberia(num);

  }

}


int main(){

	const int clientes = 10;

	MRef<MbarberiaSU> Barberia = Create<MbarberiaSU>( clientes );

	
	thread hebra_barbero ( Barbero, Barberia);
	thread hebra[clientes];
   	for( unsigned i = 0 ; i < clientes ; i++ )
          hebra[i] = thread( Cliente, Barberia, i );


	hebra_barbero.join() ;
	// esperar a que terminen las hebras (no pasa nunca)	
	for( unsigned i = 0 ; i < clientes ; i++ )
	 hebra[i].join();
}


























