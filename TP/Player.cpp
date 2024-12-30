//
// Created by Luccas on 27/12/2024.
//

#include <iostream>
#include "headers/Player.h"

#include "headers/GameData.h"

Player player;

Player::Player() : moedas(gameData.getMoedasIniciais()), totalMoedas(gameData.getMoedasIniciais()),caravanasBarbarasAbatidas(0), totalCaravanas(0) {}

int Player::getMoedas() const { return moedas; }
int Player::getTotalMoedas() const { return totalMoedas; }
int Player::getTotalCaravanas() const { return totalCaravanas; }
int Player::getCaravanasBarbarasAbatidas() const { return caravanasBarbarasAbatidas; }

void Player::adicionarMoedas(const int valor) {
    moedas += valor;

    if(valor > 0)
        totalMoedas+= valor;
}
void Player::incrementarCaravanasBarbarasAbatidas() { caravanasBarbarasAbatidas++; }
void Player::incrementarTotalCaravanas() { totalCaravanas++; }
void Player::decrementarTotalCaravanas() { totalCaravanas--; }
void Player::mostrarEstatisticas() const {
    std::cout << "Moedas: " << moedas << std::endl;
    std::cout << "Total de moedas: " << totalMoedas << std::endl;
    std::cout << "Caravanas Barbaras Abatidas: " << caravanasBarbarasAbatidas << std::endl;
    std::cout << "Total de Caravanas: " << totalCaravanas << std::endl;
}