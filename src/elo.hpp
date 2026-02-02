#ifndef ELO_HPP
#define ELO_HPP

#include <vector>
#include <string>

using namespace std;

class Elo {
public:
    typedef vector<vector<string>> Grade;
    
    Elo();
    void processarAcoes(Grade& grade, const vector<string>& acoes);
    void exibirGrade(const Grade& grade);
    void fazerMovimentoAleatorio(Grade& grade);

private:
    void rotacionarSuperiorDireita(Grade& grade);
    void rotacionarSuperiorEsquerda(Grade& grade);
    void rotacionarInferiorDireita(Grade& grade);
    void rotacionarInferiorEsquerda(Grade& grade);
    void moverFaceCima(Grade& grade);
    void moverFaceBaixo(Grade& grade);
};

namespace std {
}

#endif