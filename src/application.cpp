// Elo Maluco
// application.cpp
// Prof. Giovani Bernardes Vitor
// ECOI2207 - 2024

#include <stdlib.h>
#include <stdio.h>
#include "application.hpp"
#include "solver.hpp"          // Added include for Solver
#include "processarJogo.hpp"   // Added include for ProcessarJogo
#include "elo.hpp"             // Added include for Elo

Aplicacao::Aplicacao(int argc, char** argv) {
    inicializar();
}

Aplicacao::~Aplicacao() {
}

void Aplicacao::inicializar(void) {   
    tempo = 0;
}


int Aplicacao::executar() {   
    //std::string caminhoArquivo = "../../../data/EloMaluco_embaralhado_100.xml";
    //std::string caminhoArquivo = "../../../data/EloMaluco_embaralhado_50.xml";
    //std::string caminhoArquivo = "../../../data/EloMaluco_embaralhado_25.xml";
    std::string caminhoArquivo = "../../../data/EloMaluco_dificil.xml";

    //std::string caminhoArquivo = "../../../data/EloMaluco_embaralhado.xml";



    //carregarEEmbaralhar(100); // Embaralha o quebra-cabeca
    carregarEResolver(caminhoArquivo);
    carregarEExecutar(caminhoArquivo);
    return 0;
}

/**
 * @brief Carrega o elo resolvido, embaralha-o e salva o estado embaralhado
 * 
 * Este método realiza as seguintes operações:
 * 1. Carrega o elo resolvido a partir de um arquivo XML
 * 2. Embaralha o elo com o número especificado de movimentos
 * 3. Exibe os estados inicial (resolvido) e final (embaralhado)
 * 4. Salva o estado embaralhado em um novo arquivo XML
 * 
 * @param numMovimentos Número de movimentos aleatórios para embaralhar o elo
 * 
 * @note Os arquivos XML são buscados e salvos em caminhos relativos predefinidos:
 * - Origem: "../../../data/EloMaluco_resolvido.xml"
 * - Destino: "../../../data/EloMaluco_embaralhado.xml"
 * 
 * @warning Se houver erro ao carregar o arquivo original, a função retorna sem realizar
 * as demais operações
 */
void Aplicacao::carregarEEmbaralhar(int numMovimentos) {
    Elo elo;
    ProcessarJogo processarJogo;

    std::string arquivoOrigem = "../../../data/EloMaluco_resolvido.xml";
    std::string arquivoDestino = "../../../data/EloMaluco_embaralhado.xml";
    
    int codigoErro = processarJogo.carregarArquivoXML(arquivoOrigem);
    if (codigoErro) {
        std::cout << "Erro ao carregar arquivo resolvido..." << std::endl;
        return;
    }

    Elo::Grade gradeInicial = processarJogo.obterEstadoInicial();
    std::cout << "Estado Inicial (Resolvido):" << std::endl;
    elo.exibirGrade(gradeInicial);

    Elo::Grade gradeEmbaralhada = processarJogo.embaralharQuebraCabeca(gradeInicial, numMovimentos, elo);
    
    std::cout << "\nEstado Embaralhado:" << std::endl;
    elo.exibirGrade(gradeEmbaralhada);

    processarJogo.salvarEstados(arquivoDestino, gradeEmbaralhada);
    std::cout << "\nEstado embaralhado salvo em: " << arquivoDestino << std::endl;
}

/**
 * @brief Carrega um arquivo XML contendo um jogo e tenta encontrar uma solução.
 * 
 * Esta função realiza as seguintes operações:
 * 1. Carrega o jogo a partir de um arquivo XML
 * 2. Verifica se o estado inicial já é o estado objetivo
 * 3. Busca uma sequência de movimentos que leva ao estado objetivo
 * 4. Exibe a solução encontrada, se houver
 * 5. Salva os estados inicial e final no arquivo XML
 * 
 * @param caminhoArquivo String contendo o caminho do arquivo XML a ser processado
 * 
 * @details A função exibe mensagens no console informando:
 * - Se houve erro na leitura do arquivo
 * - O estado inicial do jogo
 * - A sequência de movimentos da solução (se encontrada)
 * - O estado final após aplicar a solução
 * - Se o estado final corresponde ao objetivo
 */
void Aplicacao::carregarEResolver(std::string caminhoArquivo) {
    Elo elo;
    ProcessarJogo processarJogo;
    Solucionador solucionador(elo, processarJogo);

    int codigoErro = processarJogo.carregarArquivoXML(caminhoArquivo);

    if (codigoErro) {
        std::cout << "Ocorreu um erro na leitura do arquivo..." << std::endl;
        return;
    }

    std::cout << "Arquivo XML carregado com sucesso..." << std::endl;

    Elo::Grade gradeAtual = processarJogo.obterEstadoInicial();
    Elo::Grade gradeInicial = gradeAtual;
    std::cout << "Estado Inicial:" << std::endl;
    elo.exibirGrade(gradeAtual);

    if (processarJogo.ehEstadoObjetivo(gradeAtual)) {
        std::cout << "O estado inicial ja e o estado objetivo." << std::endl;
        return;
    }

    std::vector<std::string> solucao = solucionador.encontrarSolucao(gradeAtual);

    if (!solucao.empty()) {
        std::cout << "\nSolucao encontrada! Sequência de movimentos:" << std::endl;
        for (const auto& movimento : solucao) {
            std::cout << movimento << " ";
        }
        std::cout << "\n\nEstado final após aplicar a solucao:" << std::endl;
        elo.processarAcoes(gradeAtual, solucao);
        elo.exibirGrade(gradeAtual);
        processarJogo.salvarEstados(caminhoArquivo, gradeInicial, solucao);
        
        if (processarJogo.ehEstadoObjetivo(gradeAtual)) {
            std::cout << "O estado final e o estado objetivo." << std::endl;
        } else {
            std::cout << "O estado final nao e o estado objetivo." << std::endl;
        }
    } else {
        std::cout << "\nNenhuma solucao encontrada!" << std::endl;
    }
}

void Aplicacao::carregarEExecutar(std::string caminhoArquivo) {
    Elo elo;
    ProcessarJogo processarJogo;

    int codigoErro = processarJogo.carregarArquivoXML(caminhoArquivo);

    if (codigoErro) {
        std::cout << "Ocorreu um erro na leitura do arquivo de solucao..." << std::endl;
        return;
    }

    std::cout << "Arquivo de solucao XML carregado com sucesso..." << std::endl;

    Elo::Grade gradeInicial = processarJogo.obterEstadoInicial();
    std::vector<std::string> movimentos = processarJogo.obterAcoes();

    std::cout << "Estado Inicial:" << std::endl;
    elo.exibirGrade(gradeInicial);

    std::cout << "\nAplicando solucao..." << std::endl;
    for (const auto& movimento : movimentos) {
        std::cout << movimento << " ";
    }

    std::cout << "\n\nEstado final após aplicar a solucao:" << std::endl;
    elo.processarAcoes(gradeInicial, movimentos);
    elo.exibirGrade(gradeInicial);
    
    if (processarJogo.ehEstadoObjetivo(gradeInicial)) {
        std::cout << "O estado final e o estado objetivo." << std::endl;
    } else {
        std::cout << "O estado final nao e o estado objetivo." << std::endl;
    }
}


