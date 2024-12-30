//
// Created by Luccas on 13/11/2024.
//

#ifndef CARAVANA_H
#define CARAVANA_H

#include <vector>
#include "Buffer.h"
#include "mapManager.h"

//para impedir os erros de dependencias circulares
class mapManager;
class Buffer;

enum class TipoCaravana {
    Comercial,
    Militar,
    Secreta,
    Barbara
};

class Caravana {
    static int nextId;
    int id;
    int posX;
    int posY;
    int carga;
    int agua;
    int tripulacao;
    int capacidadeCarga;
    int capacidadeAgua;
    int capacidadeTripulacao;
    bool isDestroyed;
    int deathTimer;
    bool movAutonomo;
    TipoCaravana tipo;

public:
    Caravana(const int _posX, const int _posY, const int _capCarga, const int _capAgua, const int _capTrip, const TipoCaravana _tipo, const bool ma)
        : id(nextId++), posX(_posX), posY(_posY), carga(0), agua(_capAgua),
          tripulacao(_capTrip), capacidadeCarga(_capCarga), capacidadeAgua(_capAgua),
          capacidadeTripulacao(_capTrip), isDestroyed(false), deathTimer(0), movAutonomo(ma), tipo(_tipo) {}

    virtual ~Caravana() = default;

    [[nodiscard]] int getId() const;
    [[nodiscard]] int getPosX() const;
    [[nodiscard]] int getPosY() const;
    [[nodiscard]] int getCarga() const;
    [[nodiscard]] int getMaxCarga() const;
    [[nodiscard]] int getAgua() const;
    [[nodiscard]] int getMaxAgua() const;
    [[nodiscard]] int getTripulacao() const;
    [[nodiscard]] int getMaxTripulacao() const;
    [[nodiscard]] bool getMovAutonomo() const;
    [[nodiscard]] TipoCaravana getTipo() const;
    [[nodiscard]] bool getIsDestroyed() const;
    [[nodiscard]] int getDeathTimer() const;

    void setPosX(int novoX);
    void setPosY(int novoY);
    void setCarga(int novaCarga);
    void setAgua(int novaAgua);
    void setTripulacao(int novaTrip);
    void setMovAutonomo(bool newV);

    void triggerIsDestroyed();
    void increaseDeathTimer();
    bool batalhar(Caravana &caravanaInimiga);

    virtual void mover(int novaX, int novaY) = 0;

    virtual void comportamentoAutonomo(const std::vector<std::pair<int, int>> &caravanasCoords, Buffer &buffer, mapManager &manager) = 0;
    virtual void consumirAgua() = 0;
    virtual void tempestadeHandler() = 0;
    [[nodiscard]] virtual int getContadorMovs() const { return 0; }
    virtual void setContadorMovs(int m) = 0;

    void abastecerAgua();
    void descarregarCarga();
    void carregarCarga(int qtd);

    void imprimirDetalhes() const;

    static void resetId();
};


class CComercial final : public Caravana {
    int contadorMovs;
public:
    CComercial(const int _posX, const int _posY)
        : Caravana(_posX, _posY, 40, 200, 20, TipoCaravana::Comercial, false), contadorMovs(0) {}

    void mover(int novaX, int novaY) override;
    void comportamentoAutonomo(const std::vector<std::pair<int, int>> &caravanasCoords, Buffer &buffer, mapManager &manager) override;
    void consumirAgua() override;
    void tempestadeHandler() override;
    void setContadorMovs(int m) override;
    [[nodiscard]] int getContadorMovs() const override;
};

class CMilitar final : public Caravana {
    int contadorMovs;
public:
    CMilitar(const int _posX, const int _posY)
        : Caravana(_posX, _posY, 5, 400, 40, TipoCaravana::Militar, false), contadorMovs(0) {}

    void mover(int novaX, int novaY) override;
    void comportamentoAutonomo(const std::vector<std::pair<int, int>> &caravanasCoords, Buffer &buffer, mapManager &manager) override;
    void consumirAgua() override;
    void tempestadeHandler() override;
    void setContadorMovs(int m) override;
    [[nodiscard]] int getContadorMovs() const override;
};

class CSecreta final : public Caravana {
    int instantesRestantes;
public:
    CSecreta(const int _posX, const int _posY)
        : Caravana(_posX, _posY, 0, 0, 5, TipoCaravana::Secreta, true), instantesRestantes(75) {}

    void mover(int novaX, int novaY) override;
    void comportamentoAutonomo(const std::vector<std::pair<int, int>> &caravanasCoords, Buffer &buffer, mapManager &manager) override;
    void consumirAgua() override;
    void tempestadeHandler() override;
    void setContadorMovs(int m) override;
    void decrementarInstantes();
    void resetInstantes();
    [[nodiscard]] int getInstantes() const;
    [[nodiscard]] int getContadorMovs() const override;
};

class CBarbara final : public Caravana {
    int instantesRestantes;
public:
    CBarbara(const int _posX, const int _posY, const int timer)
        : Caravana(_posX, _posY, 0, 0, 40, TipoCaravana::Barbara, true), instantesRestantes(timer) {}

    void comportamentoAutonomo(const std::vector<std::pair<int, int>> &caravanasCoords, Buffer &buffer, mapManager &manager) override;
    void mover(int novaX, int novaY) override;
    void consumirAgua() override;
    void tempestadeHandler() override;
    void decrementarInstantes();
    void setContadorMovs(int m) override;
    [[nodiscard]] int getContadorMovs() const override;
};

#endif // CARAVANA_H

