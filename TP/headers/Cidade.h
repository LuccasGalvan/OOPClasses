//
// Created by Luccas on 13/11/2024.
//

#ifndef CIDADE_H
#define CIDADE_H

#include <string>
#include <vector>
#include "Caravana.h"

//para impedir os erros de dependencias circulares
class Caravana;
class Player;
class Buffer;
enum class TipoCaravana;

class Cidade {
    char nome;
    int posX, posY;
    std::vector<std::shared_ptr<Caravana>> caravanas;
    std::vector<std::shared_ptr<Caravana>> caravanasDisponiveis;
    int mercadoriasDisponiveis;
    int tripulantesDisponiveis;
    int precoCompra;
    int precoVenda;
    int precoCaravana;

public:
    Cidade(char nome, int x, int y, int mercadoriasDisponiveis, int tripulantesDisponiveis, int precoCompra, int precoVenda, int precoCaravana);

    [[nodiscard]] char getNome() const;
    [[nodiscard]] int getPosX() const;
    [[nodiscard]] int getPosY() const;
    [[nodiscard]] int getMercadoriasDisponiveis() const;
    [[nodiscard]] int getTripulantesDisponiveis() const;

    bool aceitaCaravana(const std::shared_ptr<Caravana>& caravana, Buffer &buffer);
    bool sairCaravana(const std::shared_ptr<Caravana>& caravana, Buffer &buffer);
    void listarCaravanas() const;
    int comprarMercadoria(int quantidade);
    void venderMercadoria(Caravana* caravana) const;
    int contratarTripulantes(int quantidade);
    std::shared_ptr<Caravana> comprarCaravana(TipoCaravana tipo, Buffer &buffer);

    void exibir() const;
};

#endif // CIDADE_H

