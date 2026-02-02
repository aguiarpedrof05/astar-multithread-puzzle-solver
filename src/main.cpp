// Elo Maluco
// main.cpp
// Prof. Giovani Bernardes Vitor
// ECOI2207- 2024

#include <iostream>
#include <thread>

//#include "wraps.hpp"
#include "application.hpp"

     
// Programa Principal 
int main(int argc, char** argv)
{
     
     int time=0;

     Aplicacao app(argc,argv);
     
     app.executar();
     

     return 0;
}
