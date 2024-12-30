//
// Created by Luccas on 13/11/2024.
//

#include <sstream>
#include <fstream>
#include <iostream>
#include "headers/Buffer.h"
#include "headers/GameData.h"

Buffer::Buffer() : buffer(nullptr), linhas(0), colunas(0), cursorX(0), cursorY(0) {
}

Buffer::~Buffer() {
    delete[] buffer;
}

char Buffer::getCharAt(const int x, const int y) const {
    return buffer[y * colunas + x];
}

std::vector<std::pair<int, int> > Buffer::getAllCaravans() const {
    std::vector<std::pair<int, int> > result;

    for (int i = 0; i < linhas * colunas; i++) {
        if (isdigit(buffer[i]) || buffer[i] == '!') {
            int pos_y = i / colunas;
            int pos_x = i % colunas;

            result.emplace_back(pos_x, pos_y);
        }
    }
    return result;
}

std::vector<std::pair<int, int> > Buffer::getAllItems() const {
    std::vector<std::pair<int, int> > result;

    for (int i = 0; i < linhas * colunas; i++) {
        if (buffer[i] == '&') {
            int pos_y = i / colunas;
            int pos_x = i % colunas;

            result.emplace_back(pos_x, pos_y);
        }
    }
    return result;
}

std::vector<std::pair<int, int> > Buffer::getAllCities() const {
    std::vector<std::pair<int, int> > result;

    for (int i = 0; i < linhas * colunas; i++) {
        if (std::islower(buffer[i])) {
            int pos_y = i / colunas;
            int pos_x = i % colunas;

            result.emplace_back(pos_x, pos_y);
        }
    }
    return result;
}

std::pair<int, int> Buffer::getCaravanaByChar(const char c) const {
    for (int i = 0; i < linhas * colunas; i++) {
        if (buffer[i] == c) {
            int pos_y = i / colunas;
            int pos_x = i % colunas;

            return {pos_x, pos_y};
        }
    }
    return {-1, -1};
}

std::vector<std::pair<int, int> > Buffer::getAllMountains() const {
    std::vector<std::pair<int, int> > result;

    for (int i = 0; i < linhas * colunas; i++) {
        if (buffer[i] == '+') {
            int pos_y = i / colunas;
            int pos_x = i % colunas;

            result.emplace_back(pos_x, pos_y);
        }
    }
    return result;
}

int Buffer::getLinhas() const {
    return linhas;
}

int Buffer::getColunas() const {
    return colunas;
}

int Buffer::ajustarCoordenada(const int coord, const int limite) {
    if (coord < 0) {
        return (coord + limite) % limite;
    }
    return coord % limite;
}

void Buffer::alocarBuffer() {
    delete[] buffer;
    buffer = new char[linhas * colunas];
    limparBuffer();
}

void Buffer::limparBuffer() const {
    for (int i = 0; i < linhas * colunas; ++i) {
        buffer[i] = ' ';
    }
}

void Buffer::moverCursor(const int x, const int y) {
    if (x >= 0 && x < colunas && y >= 0 && y < linhas) {
        cursorX = x;
        cursorY = y;
    }
}

void Buffer::imprimirChar(const char c) const {
    if (cursorX >= 0 && cursorX < colunas && cursorY >= 0 && cursorY < linhas) {
        buffer[getIndex(cursorX, cursorY)] = c;
    }
}

void Buffer::imprimirInt(const int c) const {
    if (cursorX >= 0 && cursorX < colunas && cursorY >= 0 && cursorY < linhas) {
        const std::string aux = std::to_string(c);
        buffer[getIndex(cursorX, cursorY)] = aux[0];
    }
}

int Buffer::getIndex(const int x, const int y) const {
    return y * colunas + x; // Converte coordenadas 2D para 1D
}

void Buffer::transcreverParaConsole() const {
    for (int y = 0; y < linhas; ++y) {
        for (int x = 0; x < colunas; ++x) {
            std::cout << buffer[getIndex(x, y)];
        }
        std::cout << '\n';
    }
}

void Buffer::limpar() {
    limparBuffer();
    cursorX = cursorY = 0;
}

bool Buffer::carregarMapa(const std::string &nomeFicheiro) {
    std::ifstream arquivo(nomeFicheiro);
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o ficheiro: " << nomeFicheiro << std::endl;
        return false;
    }

    arquivo >> linhas >> colunas;
    arquivo.ignore();

    alocarBuffer();

    std::string linha;
    int linhaAtual = 0;
    while (std::getline(arquivo, linha) && linhaAtual < linhas) {
        for (int x = 0; x < linha.size() && x < colunas; ++x) {
            buffer[getIndex(x, linhaAtual)] = linha[x];
        }
        linhaAtual++;
    }

    while (std::getline(arquivo, linha)) {
        std::istringstream iss(linha);
        std::string chave;
        int valor;
        if (iss >> chave >> valor) {
            if (chave == "moedas") ::gameData.setMoedasIniciais(valor);
            else if (chave == "instantes_entre_novos_itens") gameData.setInstantesEntreNovosItens(valor);
            else if (chave == "duração_item") gameData.setDuracaoItem(valor);
            else if (chave == "max_itens") gameData.setMaxItens(valor);
            else if (chave == "preço_venda_mercadoria") gameData.setPrecoVendaMercadoria(valor);
            else if (chave == "preço_compra_mercadoria") gameData.setPrecoCompraMercadoria(valor);
            else if (chave == "preço_caravana") gameData.setPrecoCaravana(valor);
            else if (chave == "instantes_entre_novos_barbaros") gameData.setInstantesEntreNovosBarbaros(valor);
            else if (chave == "duração_barbaros") gameData.setDuracaoBarbaros(valor);
        }
    }

    arquivo.close();
    return true;
}

void Buffer::moverAlgo(const int x, const int y, const int newX, const int newY) const {
    const char aux = getCharAt(x, y);
    const char aux2 = getCharAt(newX, newY);

    if (aux2 != '.' || !isdigit(aux2) || !isalpha(aux2)) {
        buffer[getIndex(x, y)] = '.';
    } else {
        buffer[getIndex(x, y)] = aux2;
    }

    if (newX >= 0 && newX < colunas && newY >= 0 && newY < linhas) {
        buffer[getIndex(newX, newY)] = aux;
    }
}
