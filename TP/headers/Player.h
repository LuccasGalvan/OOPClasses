//
// Created by Luccas on 27/12/2024.
//

#ifndef PLAYER_H
#define PLAYER_H

class Player {
    int moedas;
    int totalMoedas;
    int caravanasBarbarasAbatidas;
    int totalCaravanas;

public:
    Player();

    [[nodiscard]] int getMoedas() const;
    [[nodiscard]] int getTotalMoedas() const;
    [[nodiscard]] int getCaravanasBarbarasAbatidas() const;
    [[nodiscard]] int getTotalCaravanas() const;

    void adicionarMoedas(int valor);
    void incrementarCaravanasBarbarasAbatidas();
    void incrementarTotalCaravanas();
    void decrementarTotalCaravanas();
    void mostrarEstatisticas() const;
};

extern Player player;


#endif //PLAYER_H
