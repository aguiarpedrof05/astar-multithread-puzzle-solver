#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "elo.hpp"            
#include "processarJogo.hpp"  
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <mutex>
#include <condition_variable>

// Define uma estrutura para os nós usados no A*
struct No {
    Elo::Grade estado;
    std::vector<std::string> acoes;
    int custo;
    int prioridade;
    int idThread;  // ID da thread que encontrou esta solucao

    bool operator<(const No& outro) const {
        return prioridade > outro.prioridade;
    }
};

class Solucionador {
public:
    Solucionador(Elo& instanciaElo, ProcessarJogo& instanciaProcessarJogo); 
    std::vector<std::string> encontrarSolucao(const Elo::Grade& gradeInicial);
    std::vector<std::string> buscaAEstrela(const Elo::Grade& gradeInicial);
    std::vector<std::string> buscaAEstrelaParalela(const Elo::Grade& gradeInicial, int numThreads);

private:
    Elo& elo;                      
    ProcessarJogo& processarJogo;  

    // Variaveis membro adicionais
    std::vector<int> posicao; 
    int posVaziaX;              
    int posVaziaY;              
    int modo;               
    std::vector<int> sequencia;   

    std::unordered_map<std::string, int> mapaCusto;

    // Metodos utilitarios
    void exibir();
    void exibirBotao();
    void inicializarTabuleiro();
    int calcularHeuristicaSimples(const Elo::Grade& grade);
    int calcularHeuristica(const Elo::Grade& grade);

    // Membros relacionados a threads para busca paralela
    std::mutex mutexMelhorPontuacao;
    std::condition_variable cv;
    bool deveTerminar;
    
    void threadBusca(const Elo::Grade& gradeInicial, int idThread);
    No melhorNoGlobal;
    int melhorPontuacaoGlobal;
};

#endif // SOLVER_HPP