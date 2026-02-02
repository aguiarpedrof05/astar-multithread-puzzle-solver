// Elo Maluco
// application.hpp
// Prof. Giovani Bernardes Vitor
// ECOI2207 - 2024

#include <iostream>
#include <vector>
#include <list>
#include <string>

#include "tinyxml2.h"
#include "elo.hpp"
#include "processarJogo.hpp"
#include "solver.hpp"


#ifndef APPLICATION_HPP
#define APPLICATION_HPP

using namespace std;
using namespace tinyxml2;

class Aplicacao {
public:
    Aplicacao(int argc, char** argv);
    ~Aplicacao();
    
    void inicializar(void);
    void desenhar();
    int executar();
    
    void carregarEEmbaralhar(int numMovimentos);
    void carregarEResolver(std::string caminhoArquivo);
    void carregarEExecutar(std::string caminhoArquivo);

private:
    int tempo;
};

#endif // APPLICATION_HPP
