//
// Created by Luccas on 13/11/2024.
//

#include "headers/Caravana.h"
#include "headers/Player.h"
#include <string>
#include <iostream>
#include <random>

int Caravana::nextId = 1;

void Caravana::resetId() {
    nextId = 1;
}

//Getters
int Caravana::getId() const { return id; }
int Caravana::getPosX() const { return posX; }
int Caravana::getPosY() const { return posY; }
int Caravana::getCarga() const { return carga; }
int Caravana::getMaxCarga() const { return capacidadeCarga; }
int Caravana::getAgua() const { return agua; }
int Caravana::getMaxAgua() const { return capacidadeAgua; }
int Caravana::getTripulacao() const { return tripulacao; }
int Caravana::getMaxTripulacao() const { return capacidadeTripulacao; }
bool Caravana::getMovAutonomo() const { return movAutonomo; }
bool Caravana::getIsDestroyed() const { return isDestroyed; }
int Caravana::getDeathTimer() const { return deathTimer; }
TipoCaravana Caravana::getTipo() const { return tipo; }

//Setters
void Caravana::setPosX(const int novoX) { posX = novoX; }
void Caravana::setPosY(const int novoY) { posY = novoY; }
void Caravana::setAgua(const int novaAgua) { agua = novaAgua; }
void Caravana::setCarga(const int novaCarga) { carga = novaCarga; }
void Caravana::setTripulacao(const int novaTrip) { tripulacao = novaTrip; }
void Caravana::setMovAutonomo(const bool newV) { movAutonomo = newV; }


void Caravana::abastecerAgua() { agua = capacidadeAgua; }
void Caravana::descarregarCarga() { carga = 0; }

void Caravana::carregarCarga(const int qtd) {
    if (carga + qtd <= capacidadeCarga) {
        carga += qtd;
    } else {
        carga = capacidadeCarga;
        std::cout << "Um pouco da carga foi perdida por ultrapassar a capacidade maxima.\n";
    }
}

void Caravana::imprimirDetalhes() const {
    std::cout << "ID: " << id
            << "\n- Pos: (" << posX << ", " << posY << ")"
            << "\n- Carga: " << carga << "/" << capacidadeCarga
            << "\n- agua: " << agua << "/" << capacidadeAgua
            << "\n- Tripulacao: " << tripulacao << "/" << capacidadeTripulacao
            << std::endl;
}

void Caravana::triggerIsDestroyed() {
    isDestroyed = true;
}

void Caravana::increaseDeathTimer() {
    deathTimer++;
}

bool Caravana::batalhar(Caravana &caravanaInimiga) {
    std::cout << "A caravana " << this->getId() << " atacou a caravana " << caravanaInimiga.getId() << std::endl;

    if (getTipo() != TipoCaravana::Secreta) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist1(0, this->getTripulacao());
        std::uniform_int_distribution<> dist2(0, caravanaInimiga.getTripulacao());

        int sorteio1 = dist1(gen);
        int sorteio2 = dist2(gen);

        Caravana *vencedora = nullptr;
        Caravana *perdedora = nullptr;

        //a caravana que iniciou o combate tem a vantagem
        if (sorteio1 >= sorteio2) {
            vencedora = this;
            perdedora = &caravanaInimiga;
            std::cout << "A caravana " << this->getId() << " venceu a batalha.\n";
        } else if (sorteio2 > sorteio1) {
            vencedora = &caravanaInimiga;
            perdedora = this;
            std::cout << "A caravana " << this->getId() << " perdeu a batalha.\n";
        } else {
            //nunca deve cair aqui, mas para evitar memory leaks.....
            std::cerr << "Houve um erro que nao faz sentido na batalha\n";
            return false;
        }

        const int perdaVencedora = std::floor(vencedora->getTripulacao() * 0.2);
        const int perdaPerdedora = perdaVencedora * 2;

        vencedora->setTripulacao(vencedora->getTripulacao() - perdaVencedora);
        perdedora->setTripulacao(perdedora->getTripulacao() - perdaPerdedora);
        std::cout << "A caravana " << vencedora->getId() << " perdeu " << perdaVencedora << " tripulantes.\n";
        std::cout << "A caravana " << perdedora->getId() << " perdeu " << perdaPerdedora << " tripulantes.\n";

        if (perdedora->getTripulacao() <= 0) {
            perdedora->triggerIsDestroyed();

            const int aguaTransferida = std::min(perdedora->getAgua(), vencedora->getMaxAgua() - vencedora->getAgua());
            vencedora->setAgua(vencedora->getAgua() + aguaTransferida);
            std::cout << "A caravana " << perdedora->getId() <<
                    " foi destruida e sua agua foi transferida para a caravana "
                    <<
                    vencedora->getId() << ".\n";
        }

        if (vencedora->getTripulacao() <= 0) {
            vencedora->triggerIsDestroyed();
            const int aguaTransferida = std::min(vencedora->getAgua(), perdedora->getMaxAgua() - perdedora->getAgua());
            perdedora->setAgua(perdedora->getAgua() + aguaTransferida);
            std::cout << "A caravana " << vencedora->getId() <<
                    " foi destruida e sua agua foi transferida para a caravana "
                    <<
                    perdedora->getId() << ".\n";
        }

        if (vencedora->getTipo() != TipoCaravana::Barbara) {
            player.incrementarCaravanasBarbarasAbatidas();
        }

        return true;
    }
    auto *secreta = dynamic_cast<CSecreta *>(this);
    if(!secreta) {
        std::cerr << "Erro ao fazer cast da caravana fantasma\n";
        return false;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> chance(0, 100);

    if (const int roll = chance(gen); roll < 50) {
        caravanaInimiga.triggerIsDestroyed();
        player.incrementarCaravanasBarbarasAbatidas();
        secreta->resetInstantes();

        std::cout << "A caravana " << this->getId() << " Assombrou a caravana inimiga para fora do deserto.\n";

    }else {
        std::cout << "A caravana " << this->getId() << " Falhou em assombrar a caravana inimiga. "
                                                       "Instantes restantes para a caravana Fantasma desaparecer: "
                                                    << secreta->getInstantes() << std::endl;
    }

    return true;
}

//CComercial
int CComercial::getContadorMovs() const {
    return contadorMovs;
}

void CComercial::setContadorMovs(const int m) {
    if (m < 0) {
        contadorMovs = 0;
    } else {
        contadorMovs += m;
    }
    contadorMovs += m;
}

void CComercial::mover(const int novaX, const int novaY) {
    setPosX(novaX);
    setPosY(novaY);
    setContadorMovs(1);
}

void CComercial::comportamentoAutonomo(const std::vector<std::pair<int, int> > &caravanasCoords, Buffer &buffer,
                                       mapManager &manager) {
    const int limColunas = buffer.getColunas();
    const int limLinhas = buffer.getLinhas();
    const int posX = getPosX();
    const int posY = getPosY();

    for (const auto &[x, y]: caravanasCoords) {
        const int targetX = Buffer::ajustarCoordenada(x, limColunas);
        const int targetY = Buffer::ajustarCoordenada(y, limLinhas);

        const double distance = std::sqrt(std::pow(targetX - posX, 2) + std::pow(targetY - posY, 2));

        if (distance <= 2.0) {
            const int novoX = targetX > posX
                                  ? Buffer::ajustarCoordenada(posX + 2, limColunas)
                                  : Buffer::ajustarCoordenada(posX - 2, limColunas);

            const int novoY = targetY > posY
                                  ? Buffer::ajustarCoordenada(posY + 2, limLinhas)
                                  : Buffer::ajustarCoordenada(posY - 2, limLinhas);

            if (std::isdigit(buffer.getCharAt(novoX, novoY)) || buffer.getCharAt(novoX, novoY) == '!') {
                std::vector<std::pair<int, int> > adjPositions = {
                    {novoX + 1, novoY}, {novoX - 1, novoY},
                    {novoX, novoY + 1}, {novoX, novoY - 1},
                    {novoX + 1, novoY + 1}, {novoX - 1, novoY - 1},
                    {novoX + 1, novoY - 1}, {novoX - 1, novoY + 1}
                };

                bool moved = false;
                for (const auto &[adjX, adjY]: adjPositions) {
                    const int adjXAdj = Buffer::ajustarCoordenada(adjX, limColunas);
                    const int adjYAdj = Buffer::ajustarCoordenada(adjY, limLinhas);
                    if (buffer.getCharAt(adjXAdj, adjYAdj) == '.') {
                        mover(adjXAdj, adjYAdj);
                        std::cout << "Caravana Comercial " << getId() << " moveu-se para (" << adjXAdj << ", " <<
                                adjYAdj << ").\n";
                        moved = true;
                        break;
                    }
                }

                if (moved) {
                    return;
                }
            } else if (const char charAtXY = buffer.getCharAt(novoX, novoY);
                !isdigit(charAtXY) && charAtXY != '+' && charAtXY != '!') {
                if (charAtXY == '&') {
                    const auto item = manager.getItemAt(novoX, novoY);
                    item->aplicarEfeito(*this);
                    std::cout << "Efeito de item aplicado na Caravana Comercial " << getId() << std::endl;
                    manager.removeItem(novoX, novoY);
                }
                if (isalpha(charAtXY)) {
                    const auto cidade = manager.getCityAt(novoX, novoY);
                    auto caravana = manager.getCaravanaById(this->getId());
                    cidade->aceitaCaravana(caravana, buffer);
                    buffer.moverCursor(getPosX(), getPosY());
                    buffer.imprimirChar(cidade->getNome());
                }
                mover(novoX, novoY);
                std::cout << "Caravana Comercial " << getId() << " moveu-se para (" << targetX << ", " << targetY <<
                        ").\n";
                return;
            }
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(-2, 2);

    while (true) {
        const int novoX = Buffer::ajustarCoordenada(posX + dist(gen), limColunas);
        const int novoY = Buffer::ajustarCoordenada(posY + dist(gen), limLinhas);

        if (const char charAtXY = buffer.getCharAt(novoX, novoY);
            !isdigit(charAtXY) && charAtXY != '+' && charAtXY != '!') {
            if (buffer.getCharAt(novoX, novoY) == '&') {
                const auto item = manager.getItemAt(novoX, novoY);
                item->aplicarEfeito(*this);
                std::cout << "Efeito de item aplicado na Caravana Comercial " << getId() << std::endl;
                manager.removeItem(novoX, novoY);
            }
            if (isalpha(charAtXY)) {
                const auto cidade = manager.getCityAt(novoX, novoY);
                auto caravana = manager.getCaravanaById(this->getId());
                cidade->aceitaCaravana(caravana, buffer);
                buffer.moverCursor(getPosX(), getPosY());
                buffer.imprimirChar(cidade->getNome());
            }
            mover(novoX, novoY);
            std::cout << "Caravana Comercial " << getId() << " moveu-se aleatoriamente para (" << novoX << ", " << novoY
                    << ").\n";
            return;
        }
    }
}

void CComercial::consumirAgua() {
    if (getAgua() > 0) {
        if (getTripulacao() < getTripulacao() / 2) {
            setAgua(getAgua() - 1);
        }
        setAgua(getAgua() - 2);
    } else {
        setTripulacao(getTripulacao() - 1);
    }

    if (getTripulacao() < 0)
        setTripulacao(0);
}

void CComercial::tempestadeHandler() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 100);

    const int chance = (getCarga() > getMaxCarga() / 2) ? 50 : 25;
    const int roll = dist(gen);

    if (roll <= chance) {
        triggerIsDestroyed();
        std::cout << "Caravana Comercial " << getId() << " foi destruida pela tempestade.\n";
    } else {
        const int novaCarga = getCarga() - static_cast<int>(getMaxCarga() * 0.25);
        setCarga(novaCarga < 0 ? 0 : novaCarga);
        std::cout << "Caravana Comercial " << getId() << " sobreviveu a tempestade e perdeu 25% da carga.\n";
    }
}

//CMilitar
int CMilitar::getContadorMovs() const {
    return contadorMovs;
}

void CMilitar::setContadorMovs(const int m) {
    if (m < 0) {
        contadorMovs = 0;
    } else {
        contadorMovs += m;
    }
}

void CMilitar::mover(const int novaX, const int novaY) {
    setPosX(novaX);
    setPosY(novaY);
    setContadorMovs(1);
}

void CMilitar::comportamentoAutonomo(const std::vector<std::pair<int, int> > &caravanasCoords, Buffer &buffer,
                                     mapManager &manager) {
    const int limColunas = buffer.getColunas();
    const int limLinhas = buffer.getLinhas();
    const int posX = getPosX();
    const int posY = getPosY();

    for (const auto &[x, y]: caravanasCoords) {
        const int targetX = Buffer::ajustarCoordenada(x, limColunas);
        const int targetY = Buffer::ajustarCoordenada(y, limLinhas);

        const double distance = std::sqrt(std::pow(targetX - posX, 2) + std::pow(targetY - posY, 2));

        if (distance <= 6.0) {
            const int novoX = targetX > posX
                                  ? Buffer::ajustarCoordenada(posX + 6, limColunas)
                                  : Buffer::ajustarCoordenada(posX - 6, limColunas);

            const int novoY = targetY > posY
                                  ? Buffer::ajustarCoordenada(posY + 6, limLinhas)
                                  : Buffer::ajustarCoordenada(posY - 6, limLinhas);

            if (std::isdigit(buffer.getCharAt(novoX, novoY)) || buffer.getCharAt(novoX, novoY) == '!') {
                std::vector<std::pair<int, int> > adjPositions = {
                    {novoX + 1, novoY}, {novoX - 1, novoY},
                    {novoX, novoY + 1}, {novoX, novoY - 1},
                    {novoX + 1, novoY + 1}, {novoX - 1, novoY - 1},
                    {novoX + 1, novoY - 1}, {novoX - 1, novoY + 1}
                };

                bool moved = false;
                for (const auto &[adjX, adjY]: adjPositions) {
                    const int adjXAdj = Buffer::ajustarCoordenada(adjX, limColunas);
                    const int adjYAdj = Buffer::ajustarCoordenada(adjY, limLinhas);
                    if (buffer.getCharAt(adjXAdj, adjYAdj) == '.') {
                        mover(adjXAdj, adjYAdj);
                        std::cout << "Caravana Militar " << getId() << " moveu-se para (" << adjXAdj << ", " << adjYAdj
                                << ").\n";
                        moved = true;
                        break;
                    }
                }
                if (moved) {
                    return;
                }
            } else if (const char charAtXY = buffer.getCharAt(novoX, novoY);
                !isdigit(charAtXY) && charAtXY != '+' && charAtXY != '!') {
                if (buffer.getCharAt(novoX, novoY) == '&') {
                    const auto item = manager.getItemAt(novoX, novoY);
                    item->aplicarEfeito(*this);
                    std::cout << "Efeito de item aplicado na Caravana Militar " << getId() << std::endl;
                    manager.removeItem(novoX, novoY);
                }
                if (isalpha(charAtXY)) {
                    const auto cidade = manager.getCityAt(novoX, novoY);
                    auto caravana = manager.getCaravanaById(this->getId());
                    cidade->aceitaCaravana(caravana, buffer);
                    buffer.moverCursor(getPosX(), getPosY());
                    buffer.imprimirChar(cidade->getNome());
                }
                mover(novoX, novoY);
                std::cout << "Caravana Militar " << getId() << " moveu-se para (" << targetX << ", " << targetY <<
                        ").\n";
                return;
            }
        }
    }
}

void CMilitar::consumirAgua() {
    if (getTripulacao() > 0) {
        if (getAgua() > 0) {
            if (getTripulacao() < getTripulacao() / 2) {
                setAgua(getAgua() - 1);
            }
            setAgua(getAgua() - 3);
        } else {
            setTripulacao(getTripulacao() - 1);
        }
    }

    if (getTripulacao() < 0)
        setTripulacao(0);
}

void CMilitar::tempestadeHandler() {
    const int novaTripulacao = getTripulacao() - static_cast<int>(getTripulacao() * 0.1);
    setTripulacao(novaTripulacao < 0 ? 0 : novaTripulacao);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 100);

    if (const int roll = dist(gen); roll <= 33) {
        triggerIsDestroyed();
        std::cout << "Caravana Militar " << getId() << " foi destruida pela tempestade.\n";
    } else {
        std::cout << "Caravana Militar " << getId() << " sobreviveu a tempestade.\n";
    }
}

//Caravana Secreta TODO
void CSecreta::mover(const int novaX, const int novaY) {
    setPosX(novaX);
    setPosY(novaY);
}

void CSecreta::comportamentoAutonomo(const std::vector<std::pair<int, int> > &caravanasCoords, Buffer &buffer,
                                     mapManager &manager) {
    decrementarInstantes();
    const int limColunas = buffer.getColunas();
    const int limLinhas = buffer.getLinhas();
    const int posX = getPosX();
    const int posY = getPosY();

    for (const auto &[x, y]: caravanasCoords) {
        const int targetX = Buffer::ajustarCoordenada(x, limColunas);
        const int targetY = Buffer::ajustarCoordenada(y, limLinhas);

        const double distance = std::sqrt(std::pow(targetX - posX, 2) + std::pow(targetY - posY, 2));

        if (distance <= 8.0) {
            const int novoX = targetX > posX
                                  ? Buffer::ajustarCoordenada(posX + 8, limColunas)
                                  : Buffer::ajustarCoordenada(posX - 8, limColunas);

            const int novoY = targetY > posY
                                  ? Buffer::ajustarCoordenada(posY + 8, limLinhas)
                                  : Buffer::ajustarCoordenada(posY - 8, limLinhas);

            if (std::isdigit(buffer.getCharAt(novoX, novoY)) || buffer.getCharAt(novoX, novoY) == '!') {
                std::vector<std::pair<int, int> > adjPositions = {
                    {novoX + 1, novoY}, {novoX - 1, novoY},
                    {novoX, novoY + 1}, {novoX, novoY - 1},
                    {novoX + 1, novoY + 1}, {novoX - 1, novoY - 1},
                    {novoX + 1, novoY - 1}, {novoX - 1, novoY + 1}
                };

                bool moved = false;
                for (const auto &[adjX, adjY]: adjPositions) {
                    const int adjXAdj = Buffer::ajustarCoordenada(adjX, limColunas);
                    const int adjYAdj = Buffer::ajustarCoordenada(adjY, limLinhas);
                    if (buffer.getCharAt(adjXAdj, adjYAdj) == '.') {
                        mover(adjXAdj, adjYAdj);
                        std::cout << "Caravana Fantasma " << getId() << " moveu-se para (" << adjXAdj << ", " << adjYAdj
                                << ").\n";
                        moved = true;
                        break;
                    }
                }

                if (moved) return;
            } else if (buffer.getCharAt(novoX, novoY) == '.' || buffer.getCharAt(novoX, novoY) == '&') {
                mover(novoX, novoY);
                if (buffer.getCharAt(novoX, novoY) == '&') {
                    const auto item = manager.getItemAt(novoX, novoY);
                    item->aplicarEfeito(*this);
                    std::cout << "Efeito de item aplicado na Caravana Fantasma " << getId() << std::endl;
                    manager.removeItem(novoX, novoY);
                }
                std::cout << "Caravana Fantasma " << getId() << " moveu-se para (" << targetX << ", " << targetY <<
                        ").\n";
                return;
            }
        }
    }
}

void CSecreta::consumirAgua() {
}

void CSecreta::tempestadeHandler() {
    const int novosInstantes = getInstantes()/4;

    for(int i = 0; i < novosInstantes; i++) {
        decrementarInstantes();
    }
    std::cout << "Caravana Fantasma " << getId() << " sobreviveu a tempestade, mas como fantasmas odeiam areia, "
                                                    "seus instantes restantes foram diminuídos para: " << getInstantes() << std::endl;
}

int CSecreta::getContadorMovs() const {
    return 0;
}

void CSecreta::setContadorMovs(const int m) {
}

void CSecreta::decrementarInstantes() {
    if (--instantesRestantes <= 0) {
        triggerIsDestroyed();
        std::cout << "Caravana Fantasma " << getId() <<
                " ficou envergonhada por nao assombrar ninguem ha muito tempo e foi embora.\n";
    }
}

void CSecreta::resetInstantes() {
    instantesRestantes = 75;
}

int CSecreta::getInstantes() const {
    return instantesRestantes;
}


//CBarbara
void CBarbara::decrementarInstantes() {
    if (--instantesRestantes <= 0) {
        triggerIsDestroyed();
        std::cout << "Caravana Barbara " << getId() << " se aborreceu e foi embora.\n";
    }
}

void CBarbara::comportamentoAutonomo(const std::vector<std::pair<int, int> > &caravanasCoords, Buffer &buffer,
                                     mapManager &manager) {
    decrementarInstantes();
    const int limColunas = buffer.getColunas();
    const int limLinhas = buffer.getLinhas();
    const int posX = getPosX();
    const int posY = getPosY();

    for (const auto &[x, y]: caravanasCoords) {
        const int targetX = Buffer::ajustarCoordenada(x, limColunas);
        const int targetY = Buffer::ajustarCoordenada(y, limLinhas);

        const double distance = std::sqrt(std::pow(targetX - posX, 2) + std::pow(targetY - posY, 2));

        if (distance <= 8.0) {
            const int novoX = targetX > posX
                                  ? Buffer::ajustarCoordenada(posX + 8, limColunas)
                                  : Buffer::ajustarCoordenada(posX - 8, limColunas);

            const int novoY = targetY > posY
                                  ? Buffer::ajustarCoordenada(posY + 8, limLinhas)
                                  : Buffer::ajustarCoordenada(posY - 8, limLinhas);

            if (std::isdigit(buffer.getCharAt(novoX, novoY)) || buffer.getCharAt(novoX, novoY) == '!') {
                std::vector<std::pair<int, int> > adjPositions = {
                    {novoX + 1, novoY}, {novoX - 1, novoY},
                    {novoX, novoY + 1}, {novoX, novoY - 1},
                    {novoX + 1, novoY + 1}, {novoX - 1, novoY - 1},
                    {novoX + 1, novoY - 1}, {novoX - 1, novoY + 1}
                };

                bool moved = false;
                for (const auto &[adjX, adjY]: adjPositions) {
                    const int adjXAdj = Buffer::ajustarCoordenada(adjX, limColunas);
                    const int adjYAdj = Buffer::ajustarCoordenada(adjY, limLinhas);
                    if (buffer.getCharAt(adjXAdj, adjYAdj) == '.') {
                        mover(adjXAdj, adjYAdj);
                        std::cout << "Caravana Barbara " << getId() << " moveu-se para (" << adjXAdj << ", " << adjYAdj
                                << ").\n";
                        moved = true;
                        break;
                    }
                }

                if (moved) return;
            } else if (buffer.getCharAt(novoX, novoY) == '.' || buffer.getCharAt(novoX, novoY) == '&') {
                mover(novoX, novoY);
                if (buffer.getCharAt(novoX, novoY) == '&') {
                    const auto item = manager.getItemAt(novoX, novoY);
                    item->aplicarEfeito(*this);
                    std::cout << "Efeito de item aplicado na Caravana Barbara " << getId() << std::endl;
                    manager.removeItem(novoX, novoY);
                }
                std::cout << "Caravana Barbara " << getId() << " moveu-se para (" << targetX << ", " << targetY <<
                        ").\n";
                return;
            }
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(-1, 1);

    while (true) {
        const int novoX = Buffer::ajustarCoordenada(posX + dist(gen), limColunas);
        const int novoY = Buffer::ajustarCoordenada(posY + dist(gen), limLinhas);

        if (buffer.getCharAt(novoX, novoY) == '.' || buffer.getCharAt(novoX, novoY) == '&') {
            mover(novoX, novoY);
            if (buffer.getCharAt(novoX, novoY) == '&') {
                const auto item = manager.getItemAt(novoX, novoY);
                item->aplicarEfeito(*this);
                std::cout << "Efeito de item aplicado na Caravana Barbara " << getId() << std::endl;
                manager.removeItem(novoX, novoY);
            }
            std::cout << "Caravana Barbara " << getId() << " moveu-se aleatoriamente para (" << novoX << ", " << novoY
                    << ").\n";
            return;
        }
    }
}

void CBarbara::consumirAgua() {
}

void CBarbara::mover(const int novaX, const int novaY) {
    setPosX(novaX);
    setPosY(novaY);
}

void CBarbara::tempestadeHandler() {
    const int novaTripulacao = getTripulacao() - static_cast<int>(getTripulacao() * 0.1);
    setTripulacao(novaTripulacao < 0 ? 0 : novaTripulacao);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 100);

    if (const int roll = dist(gen); roll <= 25) {
        triggerIsDestroyed();
        std::cout << "Caravana Barbara " << getId() << " foi destruida pela tempestade.\n";
    } else {
        std::cout << "Caravana Barbara " << getId() << " sobreviveu a tempestade.\n";
    }
}

int CBarbara::getContadorMovs() const {
    return 0;
}

void CBarbara::setContadorMovs(int m) {
}
