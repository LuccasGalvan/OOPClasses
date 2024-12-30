#include <iostream>
#include <sstream>
#include <fstream>
#include "headers/Buffer.h"
#include "headers/mapManager.h"
#include "headers/Player.h"
#include "headers/gameData.h"

bool running = true;

Buffer buffer;

void reiniciarSimulacao(mapManager &manager, Buffer &buffer) {
    buffer.limpar();
    manager.reset();
    std::cout << "Simulacao reiniciada. Carregue um novo mapa para comecar.\n";
}

void mostraComandos() {
    std::cout << "Bem-vindo ao simulador de viagens no deserto. \n"
            "Comandos atualmente testaveis: \n"
            "Carregar mapa de ficheiro - Sintaxe: /config <nomeFicheiro>\n"
            "Imprimir caracter em uma posicao do mapa - Sintaxe: /print <x>, <y>, <char>\n"
            "Mover uma caravana N - Sintaxe: /move <N>, C/B/D/E/CE/CD/BE/BD\n"
            "Inicializar entidades - Sintaxe: /init\n"
            "Avancar X instantes - Sintaxe: /prox <x>\n"
            "Iniciar uma tempestade - Sintaxe: /areia <x> <y> <raio>\n"
            "Comprar uma caravana de uma cidade - Sintaxe: /comprac <cidade> <tipo>\n"
            "Listar caravanas atracadas na cidade - Sintaxe: /cidade <cidade>\n"
            "Mostrar as características de uma caravana especifica - Sintaxe: /caravana <id>\n"
            "Comprar M toneladas de mercadoria para a caravana N - Sintaxe: /compra <N> <M>\n"
            "Vender toda a mercadoria da caravana N - Sintaxe: /vende <N>\n"
            "Ativar/Desativar movimento autonomo da caravana N - Sintaxe: /auto ou /stop <N>\n"
            "Adicionar X moedas para o jogador - Sintaxe: /moedas <X>\n"
            "Comprar T tripulantes para a caravana N - Sintaxe: /tripul <N> <T>\n"
            "Remover uma caravana N de uma cidade - Sintaxe: /removec <N>\n"
            "Criar um savestate do buffer - Sintaxe: /saves <Nome>\n"
            "carregar um savestate do buffer - Sintaxe: /loads <Nome>\n"
            "deletar um savestate do buffer - Sintaxe: /dels <Nome>\n"
            "listar os savestates do buffer - Sintaxe: /lists\n"
            "Terminar a simulacao sem sair do programa - Sintaxe: /terminar\n"
            "Sair - Sintaxe: /sair\n";
}

bool executarComandosDeArquivo(const std::string &nomeFicheiro, mapManager &manager);

int processaComandos(const std::string &comando, mapManager &manager) {
    try {
        std::istringstream is(comando);
        std::string cmd;
        is >> cmd;

        std::vector<std::string> parametros;
        std::string temp;
        while (is >> temp) {
            parametros.push_back(temp);
        }

        if (cmd == "/sair") {
            return -1;
        }

        if (cmd == "/move") {
            if (parametros.size() != 2)
                return 0;

            int novoX = 0;
            int novoY = 0;

            const std::string &direction = parametros[1];

            if (direction == "D") {
                novoX = 1;
                novoY = 0;
            } else if (direction == "E") {
                novoX = -1;
                novoY = 0;
            } else if (direction == "C") {
                novoX = 0;
                novoY = -1;
            } else if (direction == "B") {
                novoX = 0;
                novoY = 1;
            } else if (direction == "CD") {
                novoX = 1;
                novoY = -1;
            } else if (direction == "CE") {
                novoX = -1;
                novoY = -1;
            } else if (direction == "BD") {
                novoX = 1;
                novoY = 1;
            } else if (direction == "BE") {
                novoX = -1;
                novoY = 1;
            } else {
                return 0;
            }

            const int id = std::stoi(parametros[0]);

            manager.moverCaravana(buffer, novoX, novoY, id);
            buffer.transcreverParaConsole();
            return 1;
        }


        if (cmd == "/config") {
            if (parametros.size() != 1)
                return 0;

            if (!buffer.carregarMapa(parametros[0])) {
                return -2;
            }

            std::cout << "Mapa carregado com sucesso: \n";
            buffer.transcreverParaConsole();

            return 1;
        }

        if (cmd == "/print") {
            if (parametros.size() != 3 || parametros[2].size() != 1)
                return 0;

            const int p1 = std::stoi(parametros[0]);
            const int p2 = std::stoi(parametros[1]);

            const char c = parametros[2][0];

            buffer.moverCursor(p1, p2);
            buffer.imprimirChar(c);
            buffer.transcreverParaConsole();
            return 1;
        }

        if (cmd == "/init") {
            if (parametros.size() > 1)
                return 0;

            if (manager.spawnDefaultEntities(buffer))
                return 1;
        }

        if (cmd == "/prox") {
            if (parametros.size() > 1)
                return 0;

            int quant = 1;

            if (parametros[0].data())
                quant = std::stoi(parametros[0]);

            manager.avancarInstante(quant, buffer);
            return 1;
        }

        if (cmd == "/areia") {
            if (parametros.size() != 3) return 0;

            const int centroX = std::stoi(parametros[0]);
            const int centroY = std::stoi(parametros[1]);
            const int raio = std::stoi(parametros[2]);

            manager.criarTempestade(centroX, centroY, raio, buffer);
            return 1;
        }

        if (cmd == "/comprac") {
            if (parametros.size() != 2) return 0;

            TipoCaravana tipo;

            switch (parametros[1][0]) {
                case 'C':
                    tipo = TipoCaravana::Comercial;
                    break;
                case 'M':
                    tipo = TipoCaravana::Militar;
                    break;
                case 'S':
                    tipo = TipoCaravana::Secreta;
                    break;
                default:
                    return 0;
            }

            if (manager.comprarCaravana(parametros[0][0], tipo, buffer))
                return 1;
        }

        if (cmd == "/precos") {
            if (parametros.size() > 1) return 0;

            std::cout << "Preco de Compra: " << gameData.getPrecoCompraMercadoria() << std::endl;
            std::cout << "Preco de Venda: " << gameData.getPrecoVendaMercadoria() << std::endl;
            return 1;
        }

        if (cmd == "/cidade") {
            if (parametros.size() != 1) return 0;

            if (manager.listarCaravanasCidade(parametros[0][0]))
                return 1;
        }

        if (cmd == "/caravana") {
            if (parametros.size() != 1) return 0;

            if (const int id = std::stoi(parametros[0]); manager.descricaoCaravana(id))
                return 1;
        }

        if (cmd == "/compra") {
            if (parametros.size() != 2) return 0;

            const int id = std::stoi(parametros[0]);
            const int quantidade = std::stoi(parametros[1]);

            if (manager.comprarMercadoria(id, quantidade))
                return 1;
        }

        if (cmd == "/vende") {
            if (parametros.size() != 1) return 0;

            const int id = std::stoi(parametros[0]);

            if (manager.venderMercadoria(id))
                return 1;
        }

        if (cmd == "/auto") {
            if (parametros.size() != 1) return 0;

            const int id = std::stoi(parametros[0]);

            if (manager.automatizarCaravana(id, true))
                return 1;
        }

        if (cmd == "/stop") {
            if (parametros.size() != 1) return 0;

            const int id = std::stoi(parametros[0]);

            if (manager.automatizarCaravana(id, false))
                return 1;
        }

        if (cmd == "/barbaro") {
            if (parametros.size() != 2) return 0;

            const int l = std::stoi(parametros[0]);
            const int c = std::stoi(parametros[1]);

            if (manager.autoSpawnarCB(buffer, l, c))
                return 1;
        }

        if (cmd == "/moedas") {
            if (parametros.size() > 1) return 0;

            const int quant = std::stoi(parametros[0]);

            player.adicionarMoedas(quant);
            return 1;
        }

        if (cmd == "/tripul") {
            if (parametros.size() != 2) return 0;

            const int id = std::stoi(parametros[0]);
            const int quantidade = std::stoi(parametros[1]);

            if (manager.comprarTripulantes(id, quantidade))
                return 1;
        }

        if (cmd == "/removec") {
            if (parametros.size() != 1) return 0;

            const int id = std::stoi(parametros[0]);
            if (manager.removerCaravanaDaCidade(id, buffer))
                return 1;
        }

        if (cmd == "/saves") {
            if (parametros.size() != 1) return 0;

            if (manager.saves(parametros[0], buffer))
                return 1;
        }

        if (cmd == "/loads") {
            if (parametros.size() != 1) return 0;

            if (manager.loads(parametros[0], buffer))
                return 1;
        }

        if (cmd == "/lists") {
            if (parametros.size() > 1) return 0;

            if (manager.lists())
                return 1;
        }

        if (cmd == "/dels") {
            if (parametros.size() != 1) return 0;

            if (manager.dels(parametros[0]))
                return 1;
        }

        if (cmd == "/terminar") {
            std::cout << "Pontuacao: " << std::endl;
            player.mostrarEstatisticas();
            return -3;
        }

        if (cmd == "/exec") {
            if (parametros.size() != 1)
                return 0;

            if (executarComandosDeArquivo(parametros[0], manager))
                return 1;
        }

        if (cmd == "/commands") {
            if (parametros.size() > 1) return 0;

            mostraComandos();
            return 1;
        }
    } catch (const std::exception &e) {
        std::cerr << "Erro ao processar comando: " << e.what() << std::endl;
    }
    return 0;
}

bool executarComandosDeArquivo(const std::string &nomeFicheiro, mapManager &manager) {
    std::ifstream file(nomeFicheiro);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << nomeFicheiro << std::endl;
        return false;
    }

    std::string linha;
    while (std::getline(file, linha)) {
        if (linha.empty()) continue;
        std::cout << "Executando comando: " << linha << std::endl;
        processaComandos(linha, manager);
    }

    file.close();
    return true;
}

int main() {
    mapManager manager;
    system("CLS");

    std::cout << "Bem-vindo ao simulador de viagens no deserto. \n"
            "Digite /commands para ver os comandos disponiveis.\n";

    std::string comando;
    while (running) {
        std::getline(std::cin, comando);

        const int aux = processaComandos(comando, manager);

        if (aux == -1)
            running = false;
        if (aux == -2)
            std::cout << "Caminho do ficheiro inválido\n";
        if (aux == -3)
            reiniciarSimulacao(manager, buffer);
        if (aux == 0)
            std::cout << "Sintaxe ou comando invalido, tente novamente.\n";
    }
}