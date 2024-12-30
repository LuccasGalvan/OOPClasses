//
// Created by Luccas on 22/11/2024.
//

#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <memory>
#include <map>

#include "Buffer.h"
#include "Caravana.h"
#include "Cidade.h"
#include "Item.h"

//para impedir os erros de dependencias circulares
class Caravana;
class Item;
class Cidade;
class Buffer;
enum class TipoCaravana;

class mapManager {
    std::vector<std::shared_ptr<Caravana>> caravanas;
    std::vector<std::shared_ptr<Cidade>> cidades;
    std::vector<std::shared_ptr<Item>> itens;
    std::map<std::string, Buffer> bufferCopies;
    int instanteAtual = 0;
public:
    mapManager() = default;

    void avancarInstante(int n, Buffer &buffer);

    bool spawnDefaultCaravans(const Buffer& buffer);
    bool spawnDefaultCities(const Buffer& buffer);
    bool spawnDefaultEntities(const Buffer& buffer);

    void spawnarCaravanaBarbara(Buffer& buffer);
    void spawnarItem(Buffer& buffer);

    void adicionarCaravana(std::shared_ptr<Caravana> caravana);
    void adicionarCidade(std::shared_ptr<Cidade> cidade);
    void adicionarItem(std::shared_ptr<Item> item);

    bool autoSpawnarCB(Buffer& buffer, int l, int c);
    bool comprarCaravana(char cidadeNome, TipoCaravana tipo, Buffer& buffer);
    bool comprarMercadoria(int id, int quantidade);
    bool comprarTripulantes(int id, int quantidade);
    bool venderMercadoria(int id);
    bool automatizarCaravana(int id, bool movAut);
    bool removerCaravanaDaCidade(int id, Buffer& buffer);

    void moverCaravana(Buffer& buffer, int quantX, int quantY, int id);
    void atualizarCaravanas(Buffer &buffer);
    static void exibirEstado(const Buffer& buffer) ;
    void removerCaravanasDestruidas(Buffer& buffer);
    void decrementaTempoItens(Buffer &buffer);
    void resetaMovCaravanas();
    void removeItem(int x, int y);
    void criarTempestade(int centroX, int centroY, int raio, const Buffer &buffer);
    void battleTime(Buffer& buffer);

    bool saves(const std::string &name, const Buffer &buffer);
    bool loads(const std::string &name, Buffer &buffer);
    bool dels(const std::string &name);

    void reset();

    bool checkGameOver(Buffer& buffer);

    [[nodiscard]] bool lists() const;
    [[nodiscard]] bool descricaoCaravana(int id) const;
    [[nodiscard]] bool listarCaravanasCidade(char nomeCidade) const;
    [[nodiscard]] const std::vector<std::shared_ptr<Caravana>>& getCaravanas() const { return caravanas; }
    [[nodiscard]] const std::vector<std::shared_ptr<Cidade>>& getCidades() const { return cidades; }
    [[nodiscard]] std::shared_ptr<Item> getItemAt(int x, int y) const;
    [[nodiscard]] std::shared_ptr<Cidade> getCityAt(int x, int y) const;
    [[nodiscard]] std::shared_ptr<Caravana> getCaravanaById(int id) const;
    [[nodiscard]] int getCaravanaIDByChar(const Buffer& buffer, char c) const;
    [[nodiscard]] std::vector<std::pair<int, int>> getCaravanasPorTipo(TipoCaravana tipo) const;
};



#endif //MAPMANAGER_H
