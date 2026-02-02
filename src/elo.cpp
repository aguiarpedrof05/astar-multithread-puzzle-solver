#include "elo.hpp"
#include <iostream>
#include <random>
#include <chrono>

Elo::Elo() {}

void Elo::rotacionarSuperiorDireita(Grade& grade) {
    // Rotaciona a primeira linha para a direita
    string temp = grade[0][3];
    for (int i = 3; i > 0; --i) {
        grade[0][i] = grade[0][i - 1];
    }
    grade[0][0] = temp;
}

void Elo::rotacionarSuperiorEsquerda(Grade& grade) {
    // Rotaciona a primeira linha para a esquerda
    string temp = grade[0][0];
    for (int i = 0; i < 3; ++i) {
        grade[0][i] = grade[0][i + 1];
    }
    grade[0][3] = temp;
}

void Elo::rotacionarInferiorDireita(Grade& grade) {
    // Rotaciona a última linha para a direita
    string temp = grade[3][3];
    for (int i = 3; i > 0; --i) {
        grade[3][i] = grade[3][i - 1];
    }
    grade[3][0] = temp;
}

void Elo::rotacionarInferiorEsquerda(Grade& grade) {
    // Rotaciona a última linha para a esquerda
    string temp = grade[3][0];
    for (int i = 0; i < 3; ++i) {
        grade[3][i] = grade[3][i + 1];
    }
    grade[3][3] = temp;
}

void Elo::moverFaceCima(Grade& grade) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (grade[i][j] == "vazio" || grade[i][j] == "vzo") {
                if (i + 1 < 4) {
                    swap(grade[i][j], grade[i + 1][j]);
                }
                return;
            }
        }
    }
}

void Elo::moverFaceBaixo(Grade& grade) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (grade[i][j] == "vazio" || grade[i][j] == "vzo") {
                if (i - 1 >= 0) {
                    swap(grade[i][j], grade[i - 1][j]);
                }
                return;
            }
        }
    }
}

void Elo::processarAcoes(Grade& grade, const vector<string>& acoes) {
    for (const string& acao : acoes) {
        if (acao == "rsd") {
            rotacionarSuperiorDireita(grade);
        } else if (acao == "rse") {
            rotacionarSuperiorEsquerda(grade);
        } else if (acao == "rid") {
            rotacionarInferiorDireita(grade);
        } else if (acao == "rie") {
            rotacionarInferiorEsquerda(grade);
        } else if (acao == "mfc") {
            moverFaceCima(grade);
        } else if (acao == "mfb") {
            moverFaceBaixo(grade);
        }
    }
}

void Elo::fazerMovimentoAleatorio(Grade& grade) {
    vector<string> possibleMoves = {"rsd", "rse", "rid", "rie", "mfc", "mfb"};
    
    // Create random number generator
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    mt19937 generator(seed);
    uniform_int_distribution<int> distribution(0, possibleMoves.size() - 1);
    
    // Select and execute random move
    string randomMove = possibleMoves[distribution(generator)];
    processarAcoes(grade, {randomMove});
}


void Elo::exibirGrade(const Grade& grade) {
    for (const auto& linha : grade) {
        for (const auto& col : linha) {
            cout << col << "\t";
        }
        cout << endl;
    }
}
