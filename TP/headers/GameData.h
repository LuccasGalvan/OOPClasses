//
// Created by Luccas on 27/12/2024.
//

#ifndef GAMEDATA_H
#define GAMEDATA_H



class GameData {
    int moedasIniciais;
    int instantesEntreNovosItens;
    int duracaoItem;
    int maxItens;
    int precoVendaMercadoria;
    int precoCompraMercadoria;
    int precoCaravana;
    int instantesEntreNovosBarbaros;
    int duracaoBarbaros;

public:
    GameData() : moedasIniciais(0), instantesEntreNovosItens(0), duracaoItem(0), maxItens(0),
                 precoVendaMercadoria(0), precoCompraMercadoria(0), precoCaravana(0),
                 instantesEntreNovosBarbaros(0), duracaoBarbaros(0) {}

    [[nodiscard]] int getMoedasIniciais() const { return moedasIniciais; }
    [[nodiscard]] int getInstantesEntreNovosItens() const { return instantesEntreNovosItens; }
    [[nodiscard]] int getDuracaoItem() const { return duracaoItem; }
    [[nodiscard]] int getMaxItens() const { return maxItens; }
    [[nodiscard]] int getPrecoVendaMercadoria() const { return precoVendaMercadoria; }
    [[nodiscard]] int getPrecoCompraMercadoria() const { return precoCompraMercadoria; }
    [[nodiscard]] int getPrecoCaravana() const { return precoCaravana; }
    [[nodiscard]] int getInstantesEntreNovosBarbaros() const { return instantesEntreNovosBarbaros; }
    [[nodiscard]] int getDuracaoBarbaros() const { return duracaoBarbaros; }

    void setMoedasIniciais(const int valor) { moedasIniciais = valor; }
    void setInstantesEntreNovosItens(const int valor) { instantesEntreNovosItens = valor; }
    void setDuracaoItem(const int valor) { duracaoItem = valor; }
    void setMaxItens(const int valor) { maxItens = valor; }
    void setPrecoVendaMercadoria(const int valor) { precoVendaMercadoria = valor; }
    void setPrecoCompraMercadoria(const int valor) { precoCompraMercadoria = valor; }
    void setPrecoCaravana(const int valor) { precoCaravana = valor; }
    void setInstantesEntreNovosBarbaros(const int valor) { instantesEntreNovosBarbaros = valor; }
    void setDuracaoBarbaros(const int valor) { duracaoBarbaros = valor; }
};

extern GameData gameData;


#endif //GAMEDATA_H
