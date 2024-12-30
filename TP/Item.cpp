//
// Created by Luccas on 13/11/2024.
//

#include "headers/Item.h"
#include "headers/Player.h"
#include <random>
#include <iostream>

void CaixaDePandora::aplicarEfeito(Caravana& caravana) {
    const int novaTripulacao = caravana.getTripulacao() - static_cast<int>(caravana.getTripulacao() * 0.2);
    caravana.setTripulacao(novaTripulacao < 0 ? 0 : novaTripulacao);
    std::cout << "Caixa de Pandora: Caravana " << caravana.getId() << " perdeu 20% da tripulacao.\n";
}

void ArcaDoTesouro::aplicarEfeito(Caravana& caravana) {
    player.adicionarMoedas(static_cast<int>(player.getMoedas() * 0.1));
    std::cout << "Arca do Tesouro: Caravana " << caravana.getId() << " ganhou 10% a mais de moedas para o jogador.\n";
}

void Jaula::aplicarEfeito(Caravana& caravana) {
    int novaTripulacao = caravana.getTripulacao() + 10;
    if (novaTripulacao > caravana.getMaxTripulacao()) {
        novaTripulacao = caravana.getMaxTripulacao();
    }
    caravana.setTripulacao(novaTripulacao);
    std::cout << "Jaula: Caravana " << caravana.getId() << " ganhou prisioneiros, nova tripulacao: " << novaTripulacao << ".\n";
}

void Mina::aplicarEfeito(Caravana& caravana) {
    caravana.triggerIsDestroyed();
    std::cout << "Mina: Caravana " << caravana.getId() << " foi destruida.\n";
}

void Surpresa::aplicarEfeito(Caravana& caravana) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> carga(0, caravana.getMaxCarga());
    std::uniform_int_distribution<> agua(caravana.getAgua(), caravana.getMaxAgua());

    const int novaCarga = carga(gen);
    const int novaAgua = agua(gen) - caravana.getMaxAgua();

    caravana.carregarCarga(novaCarga);
    caravana.setAgua(novaAgua);

    std::cout << "Surpresa: Caravana " << caravana.getId() << " encontrou uma caixa de suprimentos!\n";
    std::cout << "Nova carga: " << novaCarga << ", nova agua: " << novaAgua << ".\n";
}