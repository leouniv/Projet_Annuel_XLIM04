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
// Taille de rendu = taille d'affichage (1:1, zero etirement)
const int RENDER_WIDTH = 1280;
const int RENDER_HEIGHT = 720;
const int SERVER_PORT = 8000;
const int TARGET_FPS = 60;
const int JPEG_QUALITY = 80;

// Variables Globales
std::shared_ptr<rtc::PeerConnection> pc;
std::shared_ptr<rtc::DataChannel> dc;
Renderer myRenderer;
float currentAngle = 0.0f;

int main() {
    rtc::InitLogger(rtc::LogLevel::Warning); // On affiche moins de logs système
    
    // 1. Démarrage Moteur 3D
    myRenderer.init(RENDER_WIDTH, RENDER_HEIGHT);
    std::cout << "[Systeme] Moteur 3D initialise (Mode Invisible)" << std::endl;

    // 2. Configuration Reseau
    rtc::Configuration config;
    config.iceServers.emplace_back("stun:stun.l.google.com:19302");
    config.maxMessageSize = 1024 * 1024; // 1 Mo max par message

    rtc::WebSocketServer::Configuration wsConfig;
    wsConfig.port = SERVER_PORT;
    rtc::WebSocketServer wsServer(wsConfig);

    std::cout << "[Systeme] En attente de client sur le port " << SERVER_PORT << "..." << std::endl;

    wsServer.onClient([config](std::shared_ptr<rtc::WebSocket> ws) {
        std::cout << "[Client] Nouvelle connexion detectee." << std::endl;

        pc = std::make_shared<rtc::PeerConnection>(config);

        pc->onDataChannel([](std::shared_ptr<rtc::DataChannel> incomingDc) {
            dc = incomingDc;
            dc->onOpen([]() { 
                std::cout << "[Client] Canal de donnees ouvert. Pret." << std::endl;
            });

            // GESTION INPUT SOURIS + PING
            dc->onMessage([](std::variant<rtc::binary, rtc::string> data) {
                if (std::holds_alternative<rtc::string>(data)) {
                    std::string msg = std::get<rtc::string>(data);
                    
                    if (msg.rfind("PING:", 0) == 0) {
                        if (dc && dc->isOpen()) dc->send("PONG:" + msg.substr(5));
                    }
                    else if (msg.rfind("MOUSE:", 0) == 0) {
                        try {
                            float deltaX = std::stof(msg.substr(6));
                            currentAngle += deltaX * 0.02f;
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

    // 3. BOUCLE PRINCIPALE (Game Loop a 60 FPS)
    const auto frameDuration = std::chrono::microseconds(1000000 / TARGET_FPS);
    auto lastFrame = std::chrono::steady_clock::now();

    while (!glfwWindowShouldClose(myRenderer.getWindow())) {
        glfwPollEvents();

        auto now = std::chrono::steady_clock::now();
        if (now - lastFrame >= frameDuration) {
            lastFrame = now;

            // A. Rendu & Capture
            myRenderer.render(currentAngle);

            // B. Encodage JPEG & Envoi (AVANT swap, le back buffer est encore valide)
            if (dc && dc->isOpen()) {
                auto jpegData = myRenderer.getJpegData(JPEG_QUALITY);
                auto dataPtr = reinterpret_cast<const std::byte*>(jpegData.data());
                dc->send(rtc::binary(dataPtr, dataPtr + jpegData.size()));
            }

            // C. Swap Buffer
            glfwSwapBuffers(myRenderer.getWindow());
        }
        std::this_thread::sleep_for(1ms);
    }
    return 0;
}