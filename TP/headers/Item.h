//
// Created by Luccas on 13/11/2024.
//

#ifndef ITEM_H
#define ITEM_H


#include "Caravana.h"

class Caravana;

enum class TipoItem {
    CaixaDePandora,
    ArcaDoTesouro,
    Jaula,
    Mina,
    Surpresa
};

class Item {
protected:
    int posX;
    int posY;
    int duracao;
    TipoItem tipo;

public:
    Item(const int _x,const int _y, const int _duracao, const TipoItem _tipo)
        : posX(_x), posY(_y), duracao(_duracao), tipo(_tipo) {}

    virtual ~Item() = default;

    [[nodiscard]] int getPosX() const { return posX; }
    [[nodiscard]] int getPosY() const { return posY; }
    [[nodiscard]] int getDuracao() const { return duracao; }
    [[nodiscard]] TipoItem getTipo() const { return tipo; }

    void reduzirDuracao() { if (duracao > 0) --duracao; }

    virtual void aplicarEfeito(Caravana& caravana) = 0;
};

class CaixaDePandora final : public Item {
public:
    CaixaDePandora(const int x, const int y, const int duracao)
        : Item(x, y, duracao, TipoItem::CaixaDePandora) {}

    void aplicarEfeito(Caravana& caravana) override;
};

class ArcaDoTesouro final : public Item {
public:
    ArcaDoTesouro(const int x, const int y, const int duracao)
        : Item(x, y, duracao, TipoItem::ArcaDoTesouro) {}

    void aplicarEfeito(Caravana& caravana) override;
};

class Jaula final : public Item {
public:
    Jaula(const int x, const int y, const int duracao)
        : Item(x, y, duracao, TipoItem::Jaula) {}

    void aplicarEfeito(Caravana& caravana) override;
};

class Mina final : public Item {
public:
    Mina(const int x, const int y, const int duracao)
        : Item(x, y, duracao, TipoItem::Mina) {}

    void aplicarEfeito(Caravana& caravana) override;
};

class Surpresa final : public Item {
public:
    Surpresa(const int x, const int y, const int duracao)
        : Item(x, y, duracao, TipoItem::Surpresa) {}

    void aplicarEfeito(Caravana& caravana) override;
};

#endif // ITEM_H
