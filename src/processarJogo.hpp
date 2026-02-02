#ifndef PROCESSARJOGO_HPP
#define PROCESSARJOGO_HPP

#include "elo.hpp"
#include <string>
#include <vector>
#include "tinyxml2.h"
using namespace tinyxml2;

class ProcessarJogo {
public:
    ProcessarJogo();
    int carregarArquivoXML(const std::string& nomeArquivo);
    bool ehPadraoValido(const std::vector<char>& alturas);
    Elo::Grade obterEstadoInicial();
    std::vector<std::string> obterAcoes();
    void processarJogo(Elo& elo);
    void salvarEstados(const std::string& nomeArquivo, const Elo::Grade& estadoInicial, const std::vector<std::string>& acoes = {});
    Elo::Grade lerEstadoInicial(XMLElement* estadoAtual);
    std::vector<std::string> lerAcoes(XMLElement* elementoAcoes);
    bool ehEstadoObjetivo(const Elo::Grade& grade);
    Elo::Grade embaralharQuebraCabeca(const Elo::Grade& gradeInicial, int numMovimentos, Elo& elo);
private:
    XMLDocument doc;
};

#endif // PROCESSARJOGO_HPP