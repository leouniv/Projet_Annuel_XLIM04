#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <variant>
#include <atomic>
#include <string>

#include <rtc/rtc.hpp>
#include <nlohmann/json.hpp>
#include "Renderer.hpp"

using json = nlohmann::json;
using namespace std::chrono_literals;

// --- CONFIGURATION ---
// Taille de rendu (doit matcher la taille du canvas cote frontend)
// 384x216 ~ 16:9, meilleure qualite tout en restant raisonnable en taille de frame brute
const int RENDER_WIDTH = 384;
const int RENDER_HEIGHT = 216;
const int SERVER_PORT = 8000;

// Variables Globales
std::shared_ptr<rtc::PeerConnection> pc;
std::shared_ptr<rtc::DataChannel> dc;
Renderer myRenderer;
float currentAngle = 0.0f;
std::atomic<bool> renderRequested(false);

int main() {
    rtc::InitLogger(rtc::LogLevel::Warning); // On affiche moins de logs système
    
    // 1. Démarrage Moteur 3D
    myRenderer.init(RENDER_WIDTH, RENDER_HEIGHT);
    std::cout << "[Systeme] Moteur 3D initialise (Mode Invisible)" << std::endl;

    // 2. Configuration Reseau
    rtc::Configuration config;
    config.iceServers.emplace_back("stun:stun.l.google.com:19302");

    rtc::WebSocketServer::Configuration wsConfig;
    wsConfig.port = SERVER_PORT;
    rtc::WebSocketServer wsServer(wsConfig);

    std::cout << "[Systeme] En attente de client sur le port " << SERVER_PORT << "..." << std::endl;

    wsServer.onClient([](std::shared_ptr<rtc::WebSocket> ws) {
        std::cout << "[Client] Nouvelle connexion detectee." << std::endl;

        pc = std::make_shared<rtc::PeerConnection>(rtc::Configuration{});

        pc->onDataChannel([](std::shared_ptr<rtc::DataChannel> incomingDc) {
            dc = incomingDc;
            dc->onOpen([]() { 
                std::cout << "[Client] Canal de donnees ouvert. Pret." << std::endl;
            });

            // GESTION INPUT SOURIS
            dc->onMessage([](std::variant<rtc::binary, rtc::string> data) {
                if (std::holds_alternative<rtc::string>(data)) {
                    std::string msg = std::get<rtc::string>(data);
                    
                    // Format attendu : "MOUSE:x"
                    if (msg.rfind("MOUSE:", 0) == 0) {
                        try {
                            float deltaX = std::stof(msg.substr(6));
                            currentAngle += deltaX * 0.02f; // Sensibilité rotation
                            renderRequested = true;
                        } catch (...) {}
                    }
                }
            });
        });

        // WebRTC Signaling (Boilerplate)
        ws->onMessage([ws](std::variant<rtc::binary, rtc::string> data) {
            if (std::holds_alternative<rtc::string>(data)) {
                try {
                    auto msg = json::parse(std::get<rtc::string>(data));
                    std::string type = msg["type"];
                    if (type == "offer") pc->setRemoteDescription(rtc::Description(msg["sdp"], type));
                    else if (type == "candidate") pc->addRemoteCandidate(rtc::Candidate(msg["candidate"]["candidate"], msg["candidate"]["sdpMid"]));
                } catch (...) {}
            }
        });

        pc->onLocalDescription([ws](rtc::Description desc) {
            json msg; msg["type"] = desc.typeString(); msg["sdp"] = std::string(desc);
            ws->send(msg.dump());
        });
        pc->onLocalCandidate([ws](rtc::Candidate cand) {
            json msg; msg["type"] = "candidate"; msg["candidate"]["candidate"] = std::string(cand); msg["candidate"]["sdpMid"] = cand.mid();
            ws->send(msg.dump());
        });
    });

    // 3. BOUCLE PRINCIPALE (Game Loop)
    while (!glfwWindowShouldClose(myRenderer.getWindow())) {
        
        // Maintient l'application vivante même si fenêtre invisible
        glfwPollEvents();

        if (renderRequested) {
            renderRequested = false;

            // A. Rendu & Capture
            myRenderer.render(currentAngle);
            auto pixels = myRenderer.getPixels();
            
            // B. Swap Buffer (Techniquement optionnel si invisible, mais bonne pratique)
            glfwSwapBuffers(myRenderer.getWindow());

            // C. Envoi Réseau (pixels bruts RGB)
            if (dc && dc->isOpen()) {
                auto dataPtr = reinterpret_cast<const std::byte*>(pixels.data());
                dc->send(rtc::binary(dataPtr, dataPtr + pixels.size()));
            }
        }
        std::this_thread::sleep_for(5ms);
    }
    return 0;
}