#include "processarJogo.hpp"
#include <iostream>
#include <unordered_set>

ProcessarJogo::ProcessarJogo() {}

int ProcessarJogo::carregarArquivoXML(const std::string& nomeArquivo) {
    doc.LoadFile(nomeArquivo.c_str());
    return doc.ErrorID();
}

bool ProcessarJogo::ehPadraoValido(const std::vector<char>& alturas) {
    return alturas.size() == 4 && 
           alturas[0] == 's' && 
           alturas[1] == 'm' && 
           alturas[2] == 'm' && 
           alturas[3] == 'i';
}

Elo::Grade ProcessarJogo::obterEstadoInicial() {
    XMLElement* raiz = doc.RootElement();
    if (raiz) {
        XMLElement* estadoAtual = raiz->FirstChildElement("EstadoAtual");
        if (estadoAtual) {
            return lerEstadoInicial(estadoAtual);
        }
    }
    return Elo::Grade();
}

std::vector<std::string> ProcessarJogo::obterAcoes() {
    XMLElement* raiz = doc.RootElement();
    if (raiz) {
        XMLElement* acoes = raiz->FirstChildElement("Acoes");
        if (acoes) {
            return lerAcoes(acoes);
        }
    }
    return std::vector<std::string>();
}

/**
 * @brief Processa o jogo a partir de um documento XML, manipulando o estado do jogo
 * 
 * Esta função lê um documento XML que contém o estado inicial do jogo e uma lista de ações,
 * processa essas informações e atualiza o estado do jogo conforme necessário.
 * 
 * @param elo Referência para um objeto da classe Elo que contém a lógica do jogo
 * 
 * @details
 * O método realiza as seguintes operações:
 * - Acessa a raiz do documento XML
 * - Lê o estado inicial do jogo a partir do elemento "EstadoAtual"
 * - Exibe o estado inicial
 * - Processa a lista de ações contida no elemento "Acoes"
 * - Exibe o estado final após processar todas as ações
 * 
 * Se algum elemento necessário não for encontrado no XML, mensagens de erro apropriadas
 * são exibidas no console.
 */
void ProcessarJogo::processarJogo(Elo& elo) {
    XMLElement* raiz = doc.RootElement();
    if (raiz) {
        XMLElement* estadoAtual = raiz->FirstChildElement("EstadoAtual");
        XMLElement* acoes = raiz->FirstChildElement("Acoes");
        
        if (estadoAtual) {
            Elo::Grade grade = lerEstadoInicial(estadoAtual);
            
            std::cout << "Estado Inicial:" << std::endl;
            elo.exibirGrade(grade);
            
            if (acoes) {
                std::vector<std::string> listaAcoes = lerAcoes(acoes);
                elo.processarAcoes(grade, listaAcoes);
                
                std::cout << "\nEstado Final:" << std::endl;
                elo.exibirGrade(grade);
            } else {
                std::cout << "Elemento 'acoes' nao encontrado" << std::endl;
            }
        } else {
            std::cout << "Elemento 'EstadoAtual' nao encontrado" << std::endl;
        }
    }
}

/**
 * @brief Salva o estado atual do jogo e as ações executadas em um arquivo XML.
 * 
 * @param nomeArquivo O caminho e nome do arquivo XML onde os dados serão salvos
 * @param estadoInicial A grade (matriz) representando o estado atual do jogo
 * @param acoes Um vetor contendo todas as ações executadas até o momento
 * 
 * @details Esta função cria um documento XML com a seguinte estrutura:
 * - Elemento raiz "Jogo"
 *   - Elemento "EstadoAtual" contendo a matriz do jogo
 *     - Elementos "row" para cada linha da grade
 *       - Elementos "col" para cada coluna contendo os valores
 *   - Elemento "Acoes" (se houver ações)
 *     - Elementos "acao" para cada ação executada
 * 
 * O arquivo XML resultante é salvo no caminho especificado por nomeArquivo.
 */
void ProcessarJogo::salvarEstados(const std::string& nomeArquivo, const Elo::Grade& estadoInicial, const std::vector<std::string>& acoes) {
    XMLDocument newDoc;

    XMLElement* root = newDoc.NewElement("Jogo");
    newDoc.InsertFirstChild(root);

    XMLElement* estadoAtual = newDoc.NewElement("EstadoAtual");
    root->InsertEndChild(estadoAtual);

    for (const auto& linha : estadoInicial) {
        XMLElement* rowElement = newDoc.NewElement("row");
        for (const auto& col : linha) {
            XMLElement* colElement = newDoc.NewElement("col");
            colElement->SetText(col.c_str());
            rowElement->InsertEndChild(colElement);
        }
        estadoAtual->InsertEndChild(rowElement);
    }

    if (!acoes.empty()) {
        XMLElement* acoesElement = newDoc.NewElement("Acoes");
        root->InsertEndChild(acoesElement);

        for (const auto& acao : acoes) {
            XMLElement* acaoElement = newDoc.NewElement("acao");
            acaoElement->SetText(acao.c_str());
            acoesElement->InsertEndChild(acaoElement);
        }
    }

    newDoc.SaveFile(nomeArquivo.c_str());
}

Elo::Grade ProcessarJogo::lerEstadoInicial(XMLElement* estadoAtual) {
    Elo::Grade grade;
    for (XMLElement* row = estadoAtual->FirstChildElement("row"); row != nullptr; row = row->NextSiblingElement("row")) {
        std::vector<std::string> linha;
        for (XMLElement* col = row->FirstChildElement("col"); col != nullptr; col = col->NextSiblingElement("col")) {
            linha.push_back(col->GetText());
        }
        grade.push_back(linha);
    }
    return grade;
}

std::vector<std::string> ProcessarJogo::lerAcoes(XMLElement* acoesElement) {
    std::vector<std::string> acoes;
    for (XMLElement* acao = acoesElement->FirstChildElement("acao"); acao != nullptr; acao = acao->NextSiblingElement("acao")) {
        acoes.push_back(acao->GetText());
    }
    return acoes;
}

Elo::Grade ProcessarJogo::embaralharQuebraCabeca(const Elo::Grade& gradeInicial, int numMovimentos, Elo& elo) {
    Elo::Grade gradeEmbaralhada = gradeInicial;
    
    for(int i = 0; i < numMovimentos; i++) {
        elo.fazerMovimentoAleatorio(gradeEmbaralhada);
    }
    
    return gradeEmbaralhada;
}

bool ProcessarJogo::ehEstadoObjetivo(const Elo::Grade& grade) {
    std::unordered_set<std::string> columnColors;
    int whiteCol = -1;

    for (int col = 0; col < 4; col++) {
        std::string baseColor = grade[0][col].substr(0, 2);
        if (baseColor == "br") whiteCol = col;
        if (baseColor != "vz" && !columnColors.insert(baseColor).second) {
            return false;
        }

        for (int row = 1; row < 4; row++) {
            if (grade[row][col] != "vzo" && grade[row][col].substr(0, 2) != baseColor) {
                return false;
            }
        }
    }

    for (int col = 0; col < 4; col++) {
        std::vector<char> alturas;
        for (int row = 0; row < 4; row++) {
            if (grade[row][col] != "vzo") {
                alturas.push_back(grade[row][col][2]);
            }
        }

        if (col == whiteCol) {
            if (alturas.size() != 3) return false;
            if (alturas[0] != 's' || alturas[1] != 'm' || alturas[2] != 'i') return false;
        } else if (!ehPadraoValido(alturas)) {
            return false;
        }
    }

    return true;
}