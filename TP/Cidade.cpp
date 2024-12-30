//
// Created by Luccas on 13/11/2024.
//

#include "headers/Cidade.h"
#include "headers/Player.h"
#include <iostream>
#include <algorithm>

Cidade::Cidade(const char nome, const int x, const int y, const int mercadorias, const int tripulantes, const int precoC, const int precoV, const int precoCar)
        : nome(nome), posX(x), posY(y), mercadoriasDisponiveis(mercadorias),
        tripulantesDisponiveis(tripulantes), precoCompra(precoC), precoVenda(precoV),
        precoCaravana(precoCar) {
    caravanasDisponiveis.push_back(std::make_shared<CComercial>(posX, posY));
    caravanasDisponiveis.push_back(std::make_shared<CMilitar>(posX, posY));
    caravanasDisponiveis.push_back(std::make_shared<CSecreta>(posX, posY));
}

char Cidade::getNome() const {
    return nome;
}

int Cidade::getPosX() const {
    return posX;
}

int Cidade::getPosY() const {
    return posY;
}

int Cidade::getMercadoriasDisponiveis() const {
    return mercadoriasDisponiveis;
}

int Cidade::getTripulantesDisponiveis() const {
    return tripulantesDisponiveis;
}

bool Cidade::aceitaCaravana(const std::shared_ptr<Caravana>& caravana, Buffer &buffer) {
    if (!caravana) {
        std::cerr << "Erro: Caravana inválida.\n";
        return false;
    }

    auto it = std::ranges::find(caravanas, caravana);
    if (it != caravanas.end()) {
        std::cerr << "Erro: Caravana já está na cidade " << nome << ".\n";
        return false;
    }

    caravanas.push_back(caravana);
    caravana->abastecerAgua();

    std::cout << "Caravana " << caravana->getId() << " entrou na cidade " << nome
              << " na posicao (" << caravana->getPosX() << ", " << caravana->getPosY() << ").\n";

    return true;
}

bool Cidade::sairCaravana(const std::shared_ptr<Caravana>& caravana, Buffer &buffer) {
    auto it = std::ranges::find(caravanas, caravana);
    if (it != caravanas.end()) {
        caravanas.erase(it);

        std::cout << "Caravana " << caravana->getId() << " saiu da cidade " << nome << ".\n";

        const int limColunas = buffer.getColunas();
        const int limLinhas = buffer.getLinhas();
        int novoX = caravana->getPosX();
        int novoY = caravana->getPosY();

        std::vector<std::pair<int, int>> adjPositions = {
            {novoX + 1, novoY}, {novoX - 1, novoY},
            {novoX, novoY + 1}, {novoX, novoY - 1},
            {novoX + 1, novoY + 1}, {novoX - 1, novoY - 1},
            {novoX + 1, novoY - 1}, {novoX - 1, novoY + 1}
        };

        for (const auto &[adjX, adjY] : adjPositions) {
            const int adjXAdj = Buffer::ajustarCoordenada(adjX, limColunas);
            const int adjYAdj = Buffer::ajustarCoordenada(adjY, limLinhas);

            if (buffer.getCharAt(adjXAdj, adjYAdj) == '.') {
                caravana->mover(adjXAdj, adjYAdj);

                buffer.moverCursor(caravana->getPosX(), caravana->getPosY());
                buffer.imprimirInt(caravana->getId());

                std::cout << "Caravana " << caravana->getId() << " moveu-se para (" << adjXAdj << ", " << adjYAdj << ").\n";
                return true;
            }
        }

        std::cerr << "Erro: Nenhum espaço livre encontrado para mover a caravana.\n";
        return false;
    }
    std::cerr << "Erro: Caravana não encontrada na cidade " << nome << ".\n";
    return false;
}


void Cidade::listarCaravanas() const {
    std::cout << "Caravanas na cidade " << nome << ":\n";
    if (caravanas.empty()) {
        std::cout << "Nenhuma caravana presente.\n";
        return;
    }
    for (const auto& caravana : caravanas) {
        caravana->imprimirDetalhes();
    }
}

int Cidade::comprarMercadoria(int quantidade) {
    const int custo = quantidade * precoCompra;

    if (player.getMoedas() < custo) {
        std::cout << "Moedas insuficientes para comprar " << quantidade << " mercadorias.\n";
        return 0;
    }

    if (quantidade > mercadoriasDisponiveis) {
        quantidade = mercadoriasDisponiveis;
    }

    mercadoriasDisponiveis -= quantidade;
    player.adicionarMoedas(-custo);
    std::cout << "Compradas " << quantidade << " mercadorias por " << custo << " moedas.\n";
    return quantidade;
}

void Cidade::venderMercadoria(Caravana *caravana) const {
    const int quantidade = caravana->getCarga();
    const int ganho = quantidade * precoVenda;
    caravana->descarregarCarga();
    player.adicionarMoedas(ganho);
    std::cout << "Vendidas " << quantidade << " mercadorias por " << ganho << " moedas.\n";
}

int Cidade::contratarTripulantes(int quantidade) {
    const int custo = quantidade;
    if (player.getMoedas() < custo) {
        std::cout << "Moedas insuficientes para contratar " << quantidade << " tripulantes.\n";
        return 0;
    }

    if (quantidade > tripulantesDisponiveis) {
        quantidade = tripulantesDisponiveis;
    }

    tripulantesDisponiveis -= quantidade;
    player.adicionarMoedas(-custo);

    std::cout << "Contratados " << quantidade << " tripulantes por " << custo << " moedas.\n";
    return quantidade;
}

std::shared_ptr<Caravana> Cidade::comprarCaravana(const TipoCaravana tipo, Buffer &buffer) {
    if (player.getMoedas() < precoCaravana) {
        std::cout << "Moedas insuficientes para comprar uma caravana.\n";
        return nullptr;
    }

    for (auto it = caravanasDisponiveis.begin(); it != caravanasDisponiveis.end(); ++it) {
        if ((*it)->getTipo() == tipo) {
            std::shared_ptr<Caravana> novaCaravana = *it;

            int offset = 0;
            int maxOffset = std::max(buffer.getLinhas(), buffer.getColunas());
            int novoX = posX;
            int novoY = posY;

            bool posicaoEncontrada = false;
            while (offset < maxOffset) {
                novoX = Buffer::ajustarCoordenada(posX + offset, buffer.getColunas());
                novoY = Buffer::ajustarCoordenada(posY + offset, buffer.getLinhas());

                if (buffer.getCharAt(novoX, novoY) == '.') {
                    posicaoEncontrada = true;
                    break;
                }
                ++offset;
            }

            if (!posicaoEncontrada) {
                std::cerr << "Erro: Não há espaço disponível para posicionar a caravana.\n";
                return nullptr;
            }

            novaCaravana->mover(novoX, novoY);
            buffer.moverCursor(novoX, novoY);

            buffer.imprimirInt(novaCaravana->getId());
            player.adicionarMoedas(-precoCaravana);
            player.incrementarTotalCaravanas();

            caravanasDisponiveis.erase(it);

            switch (tipo) {
                case TipoCaravana::Comercial:
                    std::cout << "Caravana Comercial comprada por " << precoCaravana
                              << " moedas e posicionada em (" << novoX << ", " << novoY << ").\n";
                    break;
                case TipoCaravana::Militar:
                    std::cout << "Caravana Militar comprada por " << precoCaravana
                              << " moedas e posicionada em (" << novoX << ", " << novoY << ").\n";
                    break;
                case TipoCaravana::Secreta:
                    std::cout << "Caravana Fantasma comprada por " << precoCaravana
                              << " moedas e posicionada em (" << novoX << ", " << novoY << ").\n";
                    break;
                default:
                    break;
            }

            return novaCaravana;
        }
    }

    std::cout << "Nenhuma caravana do tipo especificado disponível para compra.\n";
    return nullptr;
}

void Cidade::exibir() const {
    std::cout << "Cidade " << nome << ":\n";
    std::cout << "Posicao: (" << posX << ", " << posY << ")\n";
    std::cout << "Mercadorias disponiveis: " << mercadoriasDisponiveis << "\n";
    std::cout << "Tripulantes disponiveis: " << tripulantesDisponiveis << "\n";
    listarCaravanas();
}