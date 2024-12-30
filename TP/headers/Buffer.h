//
// Created by Luccas on 13/11/2024.
//

#ifndef BUFFER_H
#define BUFFER_H

#include <string>
#include <vector>
#include "mapManager.h"

class Buffer {
    char *buffer;
    int linhas;
    int colunas;
    int cursorX, cursorY;

    void alocarBuffer();

    void limparBuffer() const;

    [[nodiscard]] int getIndex(int x, int y) const;

public:
    Buffer();

    ~Buffer();

    [[nodiscard]] char getCharAt(int x, int y) const;

    [[nodiscard]] std::pair<int, int> getCaravanaByChar(char c) const;

    [[nodiscard]] std::vector<std::pair<int, int> > getAllCaravans() const;

    [[nodiscard]] std::vector<std::pair<int, int> > getAllCities() const;

    [[nodiscard]] std::vector<std::pair<int, int> > getAllItems() const;

    [[nodiscard]] std::vector<std::pair<int, int> > getAllMountains() const;

    [[nodiscard]] int getLinhas() const;

    [[nodiscard]] int getColunas() const;

    void moverCursor(int x, int y);

    static int ajustarCoordenada(int coord, int limite);

    void imprimirChar(char c) const;

    void imprimirInt(int c) const;

    void transcreverParaConsole() const;

    void limpar();

    bool carregarMapa(const std::string &nomeFicheiro);

    void moverAlgo(int x, int y, int newX, int newY) const;

    bool operator==(const Buffer &other) const {
        if (linhas != other.linhas || colunas != other.colunas) {
            return false;
        }
        for (int i = 0; i < linhas * colunas; ++i) {
            if (buffer[i] != other.buffer[i]) {
                return false;
            }
        }
        return true;
    }

    Buffer &operator=(const Buffer &other) {
        if (this == &other) {
            return *this;
        }

        linhas = other.linhas;
        colunas = other.colunas;

        buffer = new char[linhas * colunas];

        for (int i = 0; i < linhas * colunas; ++i) {
            buffer[i] = other.buffer[i];
        }

        return *this;
    }
};

#endif //BUFFER_H
