#include "solver.hpp"
#include "processarJogo.hpp"  // Incluído para ProcessarJogo
#include "elo.hpp"            // Incluído para Elo
#include <queue>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>

Solucionador::Solucionador(Elo& instanciaElo, ProcessarJogo& instanciaProcessarJogo)
    : elo(instanciaElo), processarJogo(instanciaProcessarJogo) {
    }

std::vector<std::string> Solucionador::encontrarSolucao(const Elo::Grade& gradeInicial) {
    return buscaAEstrelaParalela(gradeInicial, 8); // Usa 6 threads por padrao
}

std::vector<std::string> Solucionador::buscaAEstrela(const Elo::Grade& gradeInicial) {
    std::priority_queue<No> conjuntoAberto;
    No noInicial{gradeInicial, {}, 0, calcularHeuristica(gradeInicial)};
    conjuntoAberto.push(noInicial);

    std::unordered_set<std::string> conjuntoFechado;
    int melhorPontuacao = INT_MAX;
    No melhorNo = noInicial;
    auto tempoUltimaMelhorPontuacao = std::chrono::steady_clock::now();
    int contadorTravado = 0;
    const int MAX_ITERACOES_SEM_MELHORIA = 2000000;
    const int MOVIMENTOS_EMBARALHAR = 40;
    std::mt19937 gerador(std::random_device{}());
    int iteracoes = 0;
    int iteracoesUltimaMelhoria = 0;

    while (!conjuntoAberto.empty()) {
        iteracoes++;
        No atual = conjuntoAberto.top();
        conjuntoAberto.pop();

        // Verifica se estamos presos em um mínimo local
        if (iteracoes - iteracoesUltimaMelhoria > MAX_ITERACOES_SEM_MELHORIA) {
            std::cout << "Preso em um mínimo local. Reiniciando com estado embaralhado..." << std::endl;
            
            // Use o estado atual em vez do melhor estado
            Elo::Grade estadoEmbaralhado = atual.estado;
            processarJogo.embaralharQuebraCabeca(estadoEmbaralhado, MOVIMENTOS_EMBARALHAR, elo);
            
            // Limpa as estruturas de busca
            while (!conjuntoAberto.empty()) conjuntoAberto.pop();
            conjuntoFechado.clear();
            
            // Comece do zero com o estado embaralhado, mantendo as acoes atuais
            No novoNoInicial{estadoEmbaralhado, atual.acoes, atual.custo, calcularHeuristica(estadoEmbaralhado)};
            conjuntoAberto.push(novoNoInicial);
            iteracoesUltimaMelhoria = iteracoes;
            continue;
        }

        if (atual.prioridade < melhorPontuacao) {
            melhorPontuacao = atual.prioridade;
            melhorNo = atual;
            iteracoesUltimaMelhoria = iteracoes;
            tempoUltimaMelhorPontuacao = std::chrono::steady_clock::now();
            std::cout << "\nNova melhor pontuacao encontrada na iteracao " << iteracoes 
                     << " (+" << (iteracoes - iteracoesUltimaMelhoria) 
                     << " iteracoes desde a última melhor): " << melhorPontuacao << std::endl;
            std::cout << "Estado atual:" << std::endl;
            elo.exibirGrade(atual.estado);
            std::cout << "Acoes ate agora: ";
            for (const auto& acao : atual.acoes) {
                std::cout << acao << " ";
            }
            std::cout << "\n" << std::endl;
        }

        std::ostringstream oss;
        for (const auto& linha : atual.estado) {
            for (const auto& peca : linha) {
                oss << peca << ",";
            }
        }
        std::string serializado = oss.str();

        if (conjuntoFechado.find(serializado) != conjuntoFechado.end()) continue;
        conjuntoFechado.insert(serializado);

        if (processarJogo.ehEstadoObjetivo(atual.estado)) {
            std::cout << "Estado objetivo alcancado após " << iteracoes << " iteracoes." << std::endl;
            return atual.acoes;
        }

        std::vector<std::string> acoesPossiveis = {"rsd", "rse", "rid", "rie", "mfc", "mfb"};
        for (const auto& acao : acoesPossiveis) {
            Elo::Grade novoEstado = atual.estado;
            elo.processarAcoes(novoEstado, {acao});
            std::vector<std::string> novasAcoes = atual.acoes;
            novasAcoes.push_back(acao);
            int novoCusto = atual.custo + 1;
            int novaPrioridade = novoCusto + calcularHeuristica(novoEstado);
            conjuntoAberto.push(No{novoEstado, novasAcoes, novoCusto, novaPrioridade});
        }

    }

    return {};
}

std::vector<std::string> Solucionador::buscaAEstrelaParalela(const Elo::Grade& gradeInicial, int numThreads) {
    std::vector<std::thread> threads;
    deveTerminar = false;
    melhorPontuacaoGlobal = INT_MAX;

    // Lanca threads de busca
    for (int i = 0; i < numThreads; i++) {
        threads.emplace_back(&Solucionador::threadBusca, this, gradeInicial, i);
    }

    // Aguarda todas as threads concluírem
    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "Melhor solucao encontrada pela thread " << melhorNoGlobal.idThread << std::endl;
    return melhorNoGlobal.acoes;
}

void Solucionador::threadBusca(const Elo::Grade& gradeInicial, int idThread) {
    std::priority_queue<No> conjuntoAberto;
    No noInicial{gradeInicial, {}, 0, calcularHeuristica(gradeInicial), idThread};
    conjuntoAberto.push(noInicial);

    std::unordered_set<std::string> conjuntoFechado;
    auto tempoUltimaMelhorPontuacao = std::chrono::steady_clock::now();
    int iteracoes = 0;
    int iteracoesUltimaMelhoria = 0;
    const int MAX_ITERACOES_SEM_MELHORIA = 800000; // Reduzido para múltiplas threads
    // Escolhe um número aleatório de movimentos de embaralhamento
    
    std::mt19937 gerador(std::random_device{}());
    std::uniform_int_distribution<int> dist(10, 50);
    const int MOVIMENTOS_EMBARALHAR = dist(gerador);

    while (!conjuntoAberto.empty() && !deveTerminar) {
        iteracoes++;
        No atual = conjuntoAberto.top();
        conjuntoAberto.pop();

        // Atualiza a melhor pontuacao com seguranca de thread
        if (atual.prioridade < melhorPontuacaoGlobal) {
            std::unique_lock<std::mutex> lock(mutexMelhorPontuacao);
            if (atual.prioridade < melhorPontuacaoGlobal) {
                melhorPontuacaoGlobal = atual.prioridade;
                melhorNoGlobal = atual;
                iteracoesUltimaMelhoria = iteracoes;
                
                std::cout << "\nThread " << idThread << " encontrou nova melhor pontuacao na iteracao " 
                         << iteracoes << ": " << melhorPontuacaoGlobal << std::endl;
                std::cout << "Acoes ate agora: ";
                for (const auto& acao : atual.acoes) {
                    std::cout << acao << " ";
                }
                std::cout << "\nEstado atual:" << std::endl;
                elo.exibirGrade(atual.estado);
                std::cout << std::endl;
            }
        }

        // Verifica se esta preso em um mínimo local
        if (iteracoes - iteracoesUltimaMelhoria > MAX_ITERACOES_SEM_MELHORIA) {
            std::cout << "Thread " << idThread << " presa em um mínimo local. Reiniciando..." << std::endl;
            
            // Use o estado atual em vez do melhor estado global
            Elo::Grade estadoEmbaralhado = atual.estado;
            processarJogo.embaralharQuebraCabeca(estadoEmbaralhado, MOVIMENTOS_EMBARALHAR, elo);
            
            while (!conjuntoAberto.empty()) conjuntoAberto.pop();
            conjuntoFechado.clear();
            
            // Comece do zero com o estado embaralhado, mantendo as acoes atuais
            No novoNoInicial{estadoEmbaralhado, atual.acoes, atual.custo, 
                            calcularHeuristica(estadoEmbaralhado), idThread};
            conjuntoAberto.push(novoNoInicial);
            iteracoesUltimaMelhoria = iteracoes;
            continue;
        }

        // Verifica se o estado objetivo foi alcancado
        if (processarJogo.ehEstadoObjetivo(atual.estado)) {
            std::unique_lock<std::mutex> lock(mutexMelhorPontuacao);
            melhorNoGlobal = atual;
            deveTerminar = true;
            std::cout << "Thread " << idThread << " encontrou a solucao!" << std::endl;
            return;
        }

        // Gera e processa estados vizinhos
        std::ostringstream oss;
        for (const auto& linha : atual.estado) {
            for (const auto& peca : linha) {
                oss << peca << ",";
            }
        }
        std::string serializado = oss.str();

        if (conjuntoFechado.find(serializado) != conjuntoFechado.end()) continue;
        conjuntoFechado.insert(serializado);

        std::vector<std::string> acoesPossiveis = {"rsd", "rse", "rid", "rie", "mfc", "mfb"};
        for (const auto& acao : acoesPossiveis) {
            Elo::Grade novoEstado = atual.estado;
            elo.processarAcoes(novoEstado, {acao});
            std::vector<std::string> novasAcoes = atual.acoes;
            novasAcoes.push_back(acao);
            int novoCusto = atual.custo + 1;
             int novaPrioridade = novoCusto + calcularHeuristicaSimples(novoEstado);
            //int novaPrioridade = novoCusto + calcularHeuristica(novoEstado);
            conjuntoAberto.push(No{novoEstado, novasAcoes, novoCusto, novaPrioridade, idThread});
        }
    }
}


int Solucionador::calcularHeuristicaSimples(const Elo::Grade& grade) {
    const std::vector<std::vector<std::string>> estadoObjetivo = {
        {"vms", "ams", "vds", "brs"},
        {"vmm", "amm", "vdm", "brm"},
        {"vmm", "amm", "vdm", "vzo"},
        {"vmi", "ami", "vdi", "bri"}
    };

    int distanciaTotal = 0;

    // Calcula a distância de Manhattan para cada peca
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (grade[i][j] != "vzo") {
                // Encontra a posicao correta da peca
                for (int gi = 0; gi < 4; gi++) {
                    for (int gj = 0; gj < 4; gj++) {
                        if (estadoObjetivo[gi][gj] == grade[i][j]) {
                            distanciaTotal += std::abs(i - gi) + std::abs(j - gj);
                        }
                    }
                }
            }
        }
    }
    return distanciaTotal;
}
int Solucionador::calcularHeuristica(const Elo::Grade& grade) {
    int pontuacao = 0;
    
    // Verifica cada coluna da esquerda para a direita
    for (int col = 0; col < 4; col++) {
        std::string corBase = "";
        bool colunaCompleta = true;
        std::vector<char> alturas;
        
        // Obtem a cor base da primeira celula nao vazia
        for (int row = 0; row < 4; row++) {
            if (grade[row][col] != "vzo") {
                corBase = grade[row][col].substr(0, 2);
                break;
            }
        }
        
        // Se nao encontrou cor base, a coluna esta vazia
        if (corBase.empty()) {
            pontuacao += 20; // Penaliza colunas vazias
            continue;
        }

        // Verifica se as pecas nesta coluna correspondem ao padrao de cor base e altura
        for (int row = 0; row < 4; row++) {
            if (grade[row][col] != "vzo") {
                if (grade[row][col].substr(0, 2) != corBase) {
                    colunaCompleta = false;
                    pontuacao += 2; // Penaliza cor errada
                }
                alturas.push_back(grade[row][col][2]);
            }
        }

        // Verifica padrao de altura
        if (corBase == "br") { // Coluna branca deve ter 3 pecas
            if (alturas.size() != 3) {
                pontuacao += 1;
            } else if (!(alturas[0] == 's' && alturas[1] == 'm' && alturas[2] == 'i')) {
                pontuacao += 2;
            }
        } else { // Outras colunas devem ter 4 pecas na ordem s-m-m-i
            if (!processarJogo.ehPadraoValido(alturas)) {
                pontuacao += 2;
            }
        }

        // Da bônus para colunas completas
        if (colunaCompleta) {
            // pontuacao -= 5;
        }

        // Penaliza mais fortemente pecas incorretas em colunas ja resolvidas
        if (col > 0 && !colunaCompleta) {
            pontuacao += col * 1;
        }
    }

    // Verifica posicoes incorretas de pecas em relacao as colunas ja resolvidas
    for (int row = 0; row < 4; row++) {
        bool encontrouVazio = false;
        for (int col = 0; col < 4; col++) {
            if (grade[row][col] == "vzo") {
                encontrouVazio = true;
            } else if (encontrouVazio && grade[row][col] != "vzo") {
                pontuacao += 1; // Penaliza espacos entre pecas
            }
        }
    }

    return pontuacao;
}
// ...existing code...
