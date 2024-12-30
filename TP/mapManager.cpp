//
// Created by Luccas on 22/11/2024.
//

#include <algorithm>
#include <set>
#include <iostream>
#include "headers/mapManager.h"

#include <random>

#include "headers/Caravana.h"
#include "headers/Player.h"
#include "headers/GameData.h"

void mapManager::adicionarCaravana(std::shared_ptr<Caravana> caravana) {
    caravanas.push_back(std::move(caravana));
}

void mapManager::adicionarCidade(std::shared_ptr<Cidade> cidade) {
    cidades.push_back(std::move(cidade));
}

void mapManager::adicionarItem(std::shared_ptr<Item> item) {
    itens.push_back(std::move(item));
}

std::vector<std::pair<int, int> > mapManager::getCaravanasPorTipo(const TipoCaravana tipo) const {
    std::vector<std::pair<int, int> > coordenadas;

    for (const auto &caravana: caravanas) {
        if (caravana->getTipo() == tipo) {
            coordenadas.emplace_back(caravana->getPosX(), caravana->getPosY());
        }
    }

    return coordenadas;
}

std::shared_ptr<Item> mapManager::getItemAt(const int x, const int y) const {
    for (const auto &item: itens) {
        if (item->getPosX() == x && item->getPosY() == y) {
            return item;
        }
    }
    return nullptr;
}

std::shared_ptr<Cidade> mapManager::getCityAt(const int x, const int y) const {
    for (const auto &cidade: cidades) {
        if (cidade->getPosX() == x && cidade->getPosY() == y) {
            return cidade;
        }
    }
    return nullptr;
}

std::shared_ptr<Caravana> mapManager::getCaravanaById(const int id) const {
    for (const auto &caravana: caravanas) {
        if (caravana->getId() == id) {
            return caravana;
        }
    }
    return nullptr;
}

int mapManager::getCaravanaIDByChar(const Buffer &buffer, const char c) const {
    auto [fst, snd] = buffer.getCaravanaByChar(c);
    if (fst != -1 && snd != -1) {
        for (const auto &caravana: caravanas) {
            if (caravana->getPosX() == fst && caravana->getPosY() == snd) {
                return caravana->getId();
            }
        }
    }

    return -1;
}


bool mapManager::spawnDefaultCaravans(const Buffer &buffer) {
    const auto caravanasCoords = buffer.getAllCaravans();

    if (caravanasCoords.empty())
        return false;

    for (const auto &[x, y]: caravanasCoords) {
        if (const char tipo = buffer.getCharAt(x, y); isdigit(tipo)) {
            auto novaCaravana = std::make_shared<CComercial>(x, y);
            adicionarCaravana(novaCaravana);
            std::cout << "Caravana Comercial inicializada em (" << x << ", " << y << ")\n";
            player.incrementarTotalCaravanas();
        } else if (tipo == '!') {
            auto caravanaBarbara = std::make_shared<CBarbara>(x, y, gameData.getDuracaoBarbaros());
            adicionarCaravana(caravanaBarbara);
            std::cout << "Caravana Barbara inicializada em (" << x << ", " << y << ")\n";
        }
    }
    return true;
}

bool mapManager::spawnDefaultCities(const Buffer &buffer) {
    const auto cidadesCoords = buffer.getAllCities();

    if (cidadesCoords.empty())
        return false;

    for (const auto &[x, y]: cidadesCoords) {
        char nome = buffer.getCharAt(x, y);

        const auto novaCidade = std::make_shared<Cidade>
        (nome, x, y, 50, 100, gameData.getPrecoCompraMercadoria(),
         gameData.getPrecoVendaMercadoria(), gameData.getPrecoCaravana());
        adicionarCidade(novaCidade);
        std::cout << "Cidade " << nome << " inicializada em (" << x << ", " << y << ")\n";
    }
    return true;
}

bool mapManager::spawnDefaultEntities(const Buffer &buffer) {
    if (spawnDefaultCaravans(buffer) && spawnDefaultCities(buffer)) {
        if (player.getMoedas() != gameData.getMoedasIniciais()) {
            player.adicionarMoedas(-player.getMoedas());
            player.adicionarMoedas(gameData.getMoedasIniciais());
        }
        return true;
    }
    return false;
}

void mapManager::removerCaravanasDestruidas(Buffer &buffer) {
    for (auto it = caravanas.begin(); it != caravanas.end();) {
        const TipoCaravana tipo = (*it)->getTipo();
        int deathTimerMax = 7;

        if (tipo == TipoCaravana::Comercial)
            deathTimerMax = 5;

        if (tipo == TipoCaravana::Barbara)
            deathTimerMax = 0;

        if (tipo == TipoCaravana::Secreta)
            deathTimerMax = 0;

        if ((*it)->getDeathTimer() > deathTimerMax || (*it)->getIsDestroyed()) {
            buffer.moverCursor((*it)->getPosX(), (*it)->getPosY());
            buffer.imprimirChar('.');
            std::cout << "Removendo caravana " << (*it)->getId() << " na posicao ("
                    << (*it)->getPosX() << ", " << (*it)->getPosY() << ").\n";

            it = caravanas.erase(it);
        } else {
            ++it;
        }
    }
}

bool mapManager::removerCaravanaDaCidade(const int id, Buffer &buffer) {
    for (const auto &caravana: caravanas) {
        if (caravana->getId() == id) {
            for (const auto &cidade: cidades) {
                if (cidade->getPosX() == caravana->getPosX() && cidade->getPosY() == caravana->getPosY()) {
                    cidade->sairCaravana(caravana, buffer);
                    return true;
                }
            }
        }
    }
    std::cerr << "Caravana nao está em uma cidade.\n";
    return false;
}


void mapManager::moverCaravana(Buffer &buffer, const int quantX, const int quantY, const int id) {
    for (const auto &caravana: caravanas) {
        if (caravana->getId() == id) {
            if (caravana->getTipo() == TipoCaravana::Barbara) {
                std::cerr << "Voce nao tem controle sobre os barbaros. Arrgh!\n";
                return;
            }

            if (caravana->getTipo() == TipoCaravana::Comercial && caravana->getContadorMovs() >= 2) {
                std::cerr << "Voce ja mexeu demais com essa caravana neste turno.\n";
                return;
            }

            if (caravana->getTipo() == TipoCaravana::Militar && caravana->getContadorMovs() >= 3) {
                std::cerr << "Voce ja mexeu demais com essa caravana neste turno.\n";
                return;
            }

            const int novaX = caravana->getPosX() + quantX;
            const int novaY = caravana->getPosY() + quantY;
            const char charAtXY = buffer.getCharAt(novaX, novaY);

            if (charAtXY == '&') {
                const auto item = getItemAt(novaX, novaY);
                item->aplicarEfeito(*caravana);
                std::cout << "Efeito de item aplicado na Caravana " << caravana->getId() << std::endl;
                removeItem(novaX, novaY);
            }

            if (isalpha(charAtXY)) {
                const auto cidade = getCityAt(novaX, novaY);
                if (cidade->aceitaCaravana(caravana, buffer)) {
                    buffer.moverCursor(caravana->getPosX(), caravana->getPosY());
                    buffer.imprimirChar(cidade->getNome());
                }
            }

            buffer.moverAlgo(caravana->getPosX(), caravana->getPosY(), novaX, novaY);
            caravana->mover(novaX, novaY);
        }
    }
}

void mapManager::atualizarCaravanas(Buffer &buffer) {
    for (const auto &caravana: caravanas) {
        if (caravana->getMovAutonomo() && caravana->getTripulacao() > 0) {
            std::vector<std::pair<int, int> > alvos;
            const auto itemCoords = buffer.getAllItems();
            const auto tipo = caravana->getTipo();

            if (tipo == TipoCaravana::Comercial) {
                if (caravana->getTripulacao() > 0) {
                    alvos = getCaravanasPorTipo(TipoCaravana::Militar);
                    alvos.insert(alvos.end(), itemCoords.begin(), itemCoords.end());
                    const int auxX = caravana->getPosX();
                    const int auxY = caravana->getPosY();
                    caravana->comportamentoAutonomo(alvos, buffer, *this);
                    buffer.moverAlgo(auxX, auxY, caravana->getPosX(), caravana->getPosY());
                }
            }

            if (tipo == TipoCaravana::Militar) {
                if (caravana->getTripulacao() > 0) {
                    alvos = getCaravanasPorTipo(TipoCaravana::Barbara);
                    alvos.insert(alvos.end(), itemCoords.begin(), itemCoords.end());
                    const int auxX = caravana->getPosX();
                    const int auxY = caravana->getPosY();
                    caravana->comportamentoAutonomo(alvos, buffer, *this);
                    buffer.moverAlgo(auxX, auxY, caravana->getPosX(), caravana->getPosY());
                }
            }

            if (tipo == TipoCaravana::Secreta) {
                if (caravana->getTripulacao() > 0) {
                    alvos = getCaravanasPorTipo(TipoCaravana::Barbara);
                    alvos.insert(alvos.end(), itemCoords.begin(), itemCoords.end());
                    const int auxX = caravana->getPosX();
                    const int auxY = caravana->getPosY();
                    caravana->comportamentoAutonomo(alvos, buffer, *this);
                    buffer.moverAlgo(auxX, auxY, caravana->getPosX(), caravana->getPosY());
                }
            }

            if (tipo == TipoCaravana::Barbara) {
                if (caravana->getTripulacao() > 0) {
                    alvos = getCaravanasPorTipo(TipoCaravana::Comercial);
                    std::vector<std::pair<int, int> > alvosAux = getCaravanasPorTipo(TipoCaravana::Militar);
                    alvos.insert(alvos.end(), itemCoords.begin(), itemCoords.end());
                    alvos.insert(alvos.end(), alvosAux.begin(), alvosAux.end());
                    const int auxX = caravana->getPosX();
                    const int auxY = caravana->getPosY();
                    caravana->comportamentoAutonomo(alvos, buffer, *this);
                    buffer.moverAlgo(auxX, auxY, caravana->getPosX(), caravana->getPosY());
                }
            }
        }
        if (caravana->getTripulacao() <= 0) {
            caravana->increaseDeathTimer();
        }
        caravana->consumirAgua();
        caravana->imprimirDetalhes();
    }
    removerCaravanasDestruidas(buffer);
}

bool mapManager::listarCaravanasCidade(const char nomeCidade) const {
    for (const auto &cidade: cidades) {
        if (cidade->getNome() == nomeCidade) {
            cidade->exibir();
            return true;
        }
    }
    std::cout << "Nenhuma cidade com este nome foi encontrada." << std::endl;
    return false;
}

bool mapManager::descricaoCaravana(const int id) const {
    for (const auto &caravana: caravanas) {
        if (caravana->getId() == id) {
            caravana->imprimirDetalhes();
            return true;
        }
    }

    std::cout << "Nenhuma caravana com este ID foi encontrada." << std::endl;
    return false;
}

bool mapManager::comprarMercadoria(const int id, const int quantidade) {
    //eu sei que isso é a coisa mais feia do mundo, mas eu não quero fazer uma flag pra cada caravana mostrar em qual cidade se encontra
    for (const auto &caravana: caravanas) {
        if (caravana->getId() == id) {
            const int auxX = caravana->getPosX();
            const int auxY = caravana->getPosY();

            for (const auto &cidade: cidades) {
                if (cidade->getPosX() == auxX && cidade->getPosY() == auxY) {
                    if (cidade->getMercadoriasDisponiveis() > 0) {
                        caravana->carregarCarga(cidade->comprarMercadoria(quantidade));
                        return true;
                    }
                }
            }
            std::cout << "Esta caravana não se encontra em uma cidade." << std::endl;
            return false;
        }
    }

    std::cout << "Nenhuma caravana com este ID foi encontrada." << std::endl;
    return false;
}

bool mapManager::comprarTripulantes(const int id, const int quantidade) {
    //eu sei que isso é a coisa mais feia do mundo, mas eu não quero fazer uma flag pra cada caravana mostrar em qual cidade se encontra
    for (const auto &caravana: caravanas) {
        if (caravana->getId() == id) {
            const int auxX = caravana->getPosX();
            const int auxY = caravana->getPosY();

            for (const auto &cidade: cidades) {
                if (cidade->getPosX() == auxX && cidade->getPosY() == auxY) {
                    if (cidade->getTripulantesDisponiveis() > 0 && caravana->getTripulacao() + quantidade < caravana->
                        getMaxTripulacao()) {
                        caravana->setTripulacao(caravana->getTripulacao() + cidade->contratarTripulantes(quantidade));
                        return true;
                    }
                    std::cout <<
                            "Cidade nao possui tripulantes o suficiente ou a quantidade excede a capacidade maxima da caravana."
                            << std::endl;
                    return false;
                }
            }
            std::cout << "Esta caravana nao se encontra em uma cidade." << std::endl;
            return false;
        }
    }

    std::cout << "Nenhuma caravana com este ID foi encontrada." << std::endl;
    return false;
}

bool mapManager::venderMercadoria(const int id) {
    //eu sei que isso é a coisa mais feia do mundo, mas eu não quero fazer uma flag pra cada caravana mostrar em qual cidade se encontra
    for (const auto &caravana: caravanas) {
        if (caravana->getId() == id) {
            const int auxX = caravana->getPosX();
            const int auxY = caravana->getPosY();

            for (const auto &cidade: cidades) {
                if (cidade->getPosX() == auxX && cidade->getPosY() == auxY) {
                    cidade->venderMercadoria(caravana.get());
                    caravana->descarregarCarga();
                    return true;
                }
            }
            std::cout << "Esta caravana não se encontra em uma cidade." << std::endl;
            return false;
        }
    }
    std::cout << "Nenhuma caravana com este ID foi encontrada." << std::endl;
    return false;
}


void mapManager::criarTempestade(int centroX, int centroY, const int raio, const Buffer &buffer) {
    const int limColunas = buffer.getColunas();
    const int limLinhas = buffer.getLinhas();

    centroX = Buffer::ajustarCoordenada(centroX, limColunas);
    centroY = Buffer::ajustarCoordenada(centroY, limLinhas);

    for (const auto &caravana: caravanas) {
        const int distX = Buffer::ajustarCoordenada(caravana->getPosX() - centroX, limColunas);
        const int distY = Buffer::ajustarCoordenada(caravana->getPosY() - centroY, limLinhas);

        if (const int distSquared = distX * distX + distY * distY; distSquared <= raio * raio) {
            std::cout << "Caravana " << caravana->getId() << " foi afetada pela tempestade.\n";
            caravana->tempestadeHandler();
        }
    }
}

void mapManager::exibirEstado(const Buffer &buffer) {
    std::cout << "Estado do Mapa:\n";

    buffer.transcreverParaConsole();

    std::cout << "Moedas: "
            << player.getMoedas() << std::endl;
}


void mapManager::spawnarCaravanaBarbara(Buffer &buffer) {
    if (instanteAtual % gameData.getInstantesEntreNovosBarbaros() == 0) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distL(0, buffer.getLinhas());
        std::uniform_int_distribution<> distC(0, buffer.getColunas());

        while (true) {
            const int posX = distL(gen);
            const int posY = distC(gen);

            if (const char charAtXY = buffer.getCharAt(posX, posY); charAtXY == '.') {
                const auto caravanaBarbara = std::make_shared<CBarbara>(posX, posY, gameData.getDuracaoBarbaros());
                adicionarCaravana(caravanaBarbara);
                std::cout << "Nova Caravana Barbara criada em (" << posX << ", " << posY << ").\n";
                buffer.moverCursor(posX, posY);
                buffer.imprimirChar('!');
                return;
            }
        }
    }
}

bool mapManager::autoSpawnarCB(Buffer &buffer, const int l, const int c) {
    const int posX = c;
    const int posY = l;

    if (const char charAtXY = buffer.getCharAt(posX, posY); charAtXY == '.') {
        const auto caravanaBarbara = std::make_shared<CBarbara>(posX, posY, gameData.getDuracaoBarbaros());
        adicionarCaravana(caravanaBarbara);
        std::cout << "Nova Caravana Barbara criada em (" << posX << ", " << posY << ").\n";
        buffer.moverCursor(posX, posY);
        buffer.imprimirChar('!');
        return true;
    }

    std::cout << "Posicao invalida para criação de caravana Barbara" << std::endl;
    return false;
}


void mapManager::spawnarItem(Buffer &buffer) {
    if (instanteAtual % gameData.getInstantesEntreNovosItens() == 0 && itens.size() < gameData.getMaxItens()) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distL(0, buffer.getLinhas());
        std::uniform_int_distribution<> distC(0, buffer.getColunas());
        std::uniform_int_distribution<> dist(0, 4);

        const std::vector<TipoItem> tiposItens = {
            TipoItem::CaixaDePandora,
            TipoItem::ArcaDoTesouro,
            TipoItem::Jaula,
            TipoItem::Mina,
            TipoItem::Surpresa
        };

        const TipoItem itemType = tiposItens[dist(gen)];

        int tentativas = 0;
        constexpr int maxTentativas = 100;

        while (tentativas < maxTentativas) {
            const int posX = distL(gen);
            const int posY = distC(gen);

            if (const char charAtXY = buffer.getCharAt(posX, posY); charAtXY == '.') {
                std::shared_ptr<Item> novoItem;
                switch (itemType) {
                    case TipoItem::CaixaDePandora:
                        novoItem = std::make_shared<CaixaDePandora>(posX, posY, gameData.getDuracaoItem());
                        break;
                    case TipoItem::ArcaDoTesouro:
                        novoItem = std::make_shared<ArcaDoTesouro>(posX, posY, gameData.getDuracaoItem());
                        break;
                    case TipoItem::Jaula:
                        novoItem = std::make_shared<Jaula>(posX, posY, gameData.getDuracaoItem());
                        break;
                    case TipoItem::Mina:
                        novoItem = std::make_shared<Mina>(posX, posY, gameData.getDuracaoItem());
                        break;
                    case TipoItem::Surpresa:
                        novoItem = std::make_shared<Surpresa>(posX, posY, gameData.getDuracaoItem());
                        break;
                }
                adicionarItem(novoItem);
                std::cout << "Novo Item criado em (" << posX << ", " << posY << ").\n";
                buffer.moverCursor(posX, posY);
                buffer.imprimirChar('&');
                return;
            }
            tentativas++;
        }
        std::cerr << "Nao foi possivel criar um novo item.\n";
    }
}

bool mapManager::comprarCaravana(const char cidadeNome, const TipoCaravana tipo, Buffer &buffer) {
    int contaCaravanas = 0;
    for (const auto &caravana: caravanas) {
        if (caravana->getTipo() != TipoCaravana::Barbara) {
            contaCaravanas++;
        }
    }

    if (contaCaravanas >= 9) {
        std::cerr << "Numero maximo de caravanas atingido." << std::endl;
        return false;
    }

    for (const auto &cidade: cidades) {
        if (cidade->getNome() == cidadeNome) {
            if (const auto novaCaravana = cidade->comprarCaravana(tipo, buffer)) {
                adicionarCaravana(novaCaravana);
                return true;
            }
        }
    }
    std::cout << "Cidade " << cidadeNome << " nao encontrada.\n";
    return false;
}

bool mapManager::automatizarCaravana(const int id, const bool movAut) {
    for (const auto &caravana: caravanas) {
        if (caravana->getId() == id) {
            caravana->setMovAutonomo(movAut);
            std::cout << "Caravana com ID: " << id << " teve seu movimento autonomo setado para " << movAut <<
                    std::endl;
            return true;
        }
    }
    std::cout << "Caravana com ID: " << id << " nao encontrada.\n";
    return false;
}

void mapManager::decrementaTempoItens(Buffer &buffer) {
    if (itens.empty()) {
        return;
    }

    for (auto it = itens.begin(); it != itens.end();) {
        (*it)->reduzirDuracao();

        if ((*it)->getDuracao() <= 0) {
            buffer.moverCursor(it->get()->getPosX(), it->get()->getPosY());
            buffer.imprimirChar('.');
            it = itens.erase(it);
        } else {
            ++it;
        }
    }
}


void mapManager::removeItem(int x, int y) {
    const auto it = std::find_if(itens.begin(), itens.end(), [x, y](const std::shared_ptr<Item> &item) {
        return item->getPosX() == x && item->getPosY() == y;
    });

    if (it != itens.end()) {
        itens.erase(it);
        std::cout << "Item removido em (" << x << ", " << y << ").\n";
    }
}

void mapManager::battleTime(Buffer &buffer) {
    // Set de pares de ids de caravanas para evitar batalhas duplicadas
    std::set<std::pair<int, int> > batalhasRealizadas;

    for (const auto &caravana: caravanas) {
        std::vector<std::pair<int, int> > adjPositions = {
            {caravana->getPosX() + 1, caravana->getPosY()}, {caravana->getPosX() - 1, caravana->getPosY()},
            {caravana->getPosX(), caravana->getPosY() + 1}, {caravana->getPosX(), caravana->getPosY() - 1},
            {caravana->getPosX() + 1, caravana->getPosY() + 1}, {caravana->getPosX() - 1, caravana->getPosY() - 1},
            {caravana->getPosX() + 1, caravana->getPosY() - 1}, {caravana->getPosX() - 1, caravana->getPosY() + 1}
        };

        for (const auto &[fst, snd]: adjPositions) {
            for (const auto &caravana2: caravanas) {
                if (caravana2->getPosX() == fst && caravana2->getPosY() == snd) {
                    int id1 = caravana->getId();
                    int id2 = caravana2->getId();
                    if (id1 > id2) std::swap(id1, id2);

                    if (!batalhasRealizadas.contains({id1, id2})) {
                        bool batalha = false;
                        if ((caravana->getTipo() == TipoCaravana::Militar || caravana->getTipo() ==
                             TipoCaravana::Comercial) && caravana2->getTipo() == TipoCaravana::Barbara) {
                            batalha = true;
                        } else if (caravana->getTipo() == TipoCaravana::Barbara
                                   && (caravana2->getTipo() == TipoCaravana::Militar || caravana2->getTipo() ==
                                       TipoCaravana::Comercial)) {
                            batalha = true;
                        } else if (caravana->getTipo() == TipoCaravana::Secreta && caravana2->getTipo() ==
                                   TipoCaravana::Barbara) {
                            batalha = true;
                        }

                        if (batalha) {
                            if (!caravana->batalhar(*caravana2)) {
                                std::cerr << "Ocorreu algum erro durante a batalha entre as caravanas " << id1 << " e "
                                        << id2 << ".\n";
                                return;
                            }
                            batalhasRealizadas.insert({id1, id2});
                        }
                    }
                }
            }
        }
    }
}

void mapManager::reset() {
    caravanas.clear();
    cidades.clear();
    itens.clear();
    bufferCopies.clear();
    instanteAtual = 0;
    Caravana::resetId();
}


bool mapManager::saves(const std::string &name, const Buffer &buffer) {
    bufferCopies[name] = buffer;
    if (bufferCopies[name] == buffer) {
        std::cout << "Buffer salvo como " << name << ".\n";
        return true;
    }
    std::cerr << "Erro ao salvar o buffer como " << name << ".\n";
    return false;
}

bool mapManager::loads(const std::string &name, Buffer &buffer) {
    auto it = bufferCopies.find(name);
    if (it != bufferCopies.end()) {
        buffer = it->second;
        std::cout << "Buffer carregado de " << name << ".\n";
        return true;
    }

    std::cerr << "Erro: Buffer com nome " << name << " nao encontrado.\n";
    return false;
}

bool mapManager::lists() const {
    if (bufferCopies.empty()) {
        std::cout << "Nenhuma copia de buffer armazenada.\n";
        return false;
    }

    std::cout << "Copias de buffer armazenadas:\n";
    for (const auto &pair: bufferCopies) {
        std::cout << "- " << pair.first << "\n";
    }
    return true;
}

bool mapManager::dels(const std::string &name) {
    auto it = bufferCopies.find(name);
    if (it != bufferCopies.end()) {
        bufferCopies.erase(it);
        std::cout << "Buffer " << name << " apagado.\n";
        return true;
    }
    std::cerr << "Erro: Buffer com nome " << name << " nao encontrado.\n";
    return false;
}

bool mapManager::checkGameOver(Buffer &buffer) {
    int contaCaravanas = 0;

    for (const auto &caravana: caravanas) {
        if (caravana->getTipo() != TipoCaravana::Barbara) {
            contaCaravanas++;
        }
    }

    if (player.getMoedas() < gameData.getPrecoCaravana() && contaCaravanas == 0) {
        std::cout << "Voce perdeu! Sem caravanas e sem dinheiro.\n";
        player.mostrarEstatisticas();

        buffer.limpar();
        reset();

        std::cout << "Carregue um novo mapa para comecar ou digite /sair.\n";
        return true;
    }
    return false;
}

void mapManager::resetaMovCaravanas() {
    for (const auto &caravana: caravanas) {
        if (caravana->getTipo() == TipoCaravana::Comercial) {
            caravana->setContadorMovs(-2);
        }
        if (caravana->getTipo() == TipoCaravana::Militar) {
            caravana->setContadorMovs(-3);
        }
    }
}


void mapManager::avancarInstante(const int n, Buffer &buffer) {
    for (int i = 0; i < n; ++i) {
        instanteAtual++;
        std::cout << "Instante: " << instanteAtual << "\n";

        spawnarCaravanaBarbara(buffer);

        spawnarItem(buffer);

        atualizarCaravanas(buffer);

        battleTime(buffer);

        exibirEstado(buffer);

        resetaMovCaravanas();

        decrementaTempoItens(buffer);

        if (checkGameOver(buffer))
            return;
    }
}
