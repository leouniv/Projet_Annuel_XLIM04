# Spécification Fonctionnelle - Projet Annuel XLIM04

## 1. Introduction

Ce document décrit les spécifications fonctionnelles du projet "Projet Annuel XLIM04", un prototype de système de rendu à distance ou de "cloud gaming". Il vise à détailler les comportements attendus du système du point de vue de l'utilisateur et des interactions entre ses composants, sans entrer dans les détails de leur implémentation technique interne.

## 2. Objectifs du Projet

L'objectif principal du projet est de démontrer la capacité à :
*   Rendre des graphiques 3D sur un serveur (backend).
*   Diffuser ces graphiques en temps réel vers un client léger (frontend web).
*   Permettre une interaction utilisateur à faible latence depuis le client pour contrôler la scène 3D rendue sur le serveur.

## 3. Fonctionnalités Générales

Le système doit permettre à un utilisateur d'accéder, via un navigateur web, à un flux vidéo d'une scène 3D générée en temps réel sur un serveur distant. L'utilisateur doit pouvoir interagir avec cette scène en utilisant sa souris, et voir les résultats de ses actions presque instantanément.

## 4. Spécifications Fonctionnelles Détaillées

### 4.1. Frontend (Client Web)

Le client web est l'interface utilisateur du système, exécuté dans un navigateur web standard.

#### 4.1.1. Connexion au Serveur
*   **SF-FE-001**: Le client doit être capable d'accéder à l'application via une adresse locale (e.g., `localhost` ou une IP locale spécifique). Au démarrage, ou sur action explicite de l'utilisateur (e.g., clic sur un bouton "Démarrer la connexion"), le client doit tenter d'établir une connexion WebRTC avec le backend en utilisant des serveurs STUN.
*   **SF-FE-002**: La signalisation pour l'établissement de la connexion WebRTC doit se faire via un WebSocket écoutant sur le port `8000`.
*   **SF-FE-003**: Le client doit afficher un état visuel clair indiquant la tentative de connexion, la connexion réussie ou un message d'erreur explicite si le serveur est inaccessible ou si la connexion échoue.

#### 4.1.2. Affichage du Flux Vidéo
*   **SF-FE-004**: Une fois la connexion WebRTC établie, le client doit recevoir des données brutes de pixels (raw pixel buffers) via un canal de données WebRTC.
*   **SF-FE-005**: Le client doit interpréter les données reçues comme des buffers de pixels bruts (e.g., RGB ou RGBA).
*   **SF-FE-006**: Chaque buffer de pixels reçu doit être affiché sur un élément `<canvas>` HTML, remplaçant la frame précédente. Le client doit s'assurer que l'orientation de l'image est correcte (e.g., en appliquant une transformation si nécessaire).
*   **SF-FE-007**: L'affichage doit être fluide, avec une latence minimale entre la réception des données et leur rendu sur le canvas.

#### 4.1.3. Gestion des Entrées Utilisateur (Souris)
*   **SF-FE-008**: Le client doit capturer les mouvements de la souris de l'utilisateur lorsque le curseur se trouve au-dessus de l'élément `<canvas>`. Les mouvements doivent déclencher une rotation fluide de l'objet 3D.
*   **SF-FE-009**: Les coordonnées des mouvements de la souris doivent être encodées selon le protocole `MOUSE:x,y` (où x et y sont les déplacements relatifs de la souris) et envoyées au backend via le canal de données WebRTC.
*   **SF-FE-010**: Les entrées souris doivent être envoyées avec une latence minimale pour assurer une interaction réactive et une rotation fluide.

#### 4.1.4. Affichage des Indicateurs de Performance (KPI)
*   **SF-FE-011**: Le client doit afficher un panneau (overlay) visible en haut de l'écran contenant les indicateurs de performance clés (KPI) suivants : le Ping et le FPS (Frames Per Second). Le FPS doit être mis à jour chaque seconde. Le Ping doit être calculé via un système de "heartbeat" entre le client et le serveur. L'objectif est de permettre à l'utilisateur de diagnostiquer les ralentissements ou la latence réseau.

### 4.2. Backend (Serveur C++)

Le backend est le composant serveur responsable du rendu 3D, de la gestion des connexions WebRTC et de la diffusion du contenu.

#### 4.2.1. Rendu 3D (Cube Rotatif)
*   **SF-BE-001**: Le backend doit initialiser un contexte de rendu OpenGL dans une fenêtre cachée de `384x216` pixels.
*   **SF-BE-002**: Il doit rendre une scène 3D basique, à savoir un cube centré dans le viewport. Le cube doit avoir des textures ou des couleurs différentes pour chaque face, et l'éclairage doit permettre de distinguer clairement les arêtes. L'implémentation du dessin du cube doit utiliser le mode immédiat de l'OpenGL hérité (`glBegin(GL_QUADS)`) avec gestion du Z-Buffer.
*   **SF-BE-003**: Le rendu doit être continu et à une fréquence d'images cible définie.
*   **SF-BE-004**: La rotation du cube doit être influencée par les entrées souris reçues du frontend.

#### 4.2.2. Capture et Encodage des Frames
*   **SF-BE-005**: Après chaque rendu de frame, le backend doit capturer le contenu du framebuffer OpenGL en extrayant les données RGB brutes via `glReadPixels` dans un `std::vector`.
*   **SF-BE-006**: La capture doit utiliser des techniques efficaces (e.g., Pixel Buffer Objects - PBO) pour minimiser l'impact sur les performances de rendu.
*   **SF-BE-007**: Les frames capturées (buffers de pixels bruts) doivent être transmises directement (sans encodage lourd tel que JPEG) au client.
*   **SF-BE-008**: La transmission des données brutes de pixels doit être optimisée pour minimiser la latence.

#### 4.2.3. Gestion de la Connexion WebRTC
*   **SF-BE-009**: Le backend doit écouter les connexions WebSocket sur le port `8000` pour la signalisation WebRTC.
*   **SF-BE-010**: Il doit gérer le processus de négociation WebRTC (`rtc::PeerConnection` avec SDP offer/answer, ICE candidates) avec le frontend via WebSocket, en utilisant des serveurs STUN.
*   **SF-BE-011**: Une fois la négociation terminée, un canal de données WebRTC doit être établi pour la communication bidirectionnelle.
*   **SF-BE-012**: Le backend doit envoyer les buffers de pixels bruts sur ce canal de données au frontend.
*   **SF-BE-013**: Le backend doit être capable de gérer la déconnexion des clients et la libération des ressources associées.

#### 4.2.4. Réception et Traitement des Entrées Utilisateur
*   **SF-BE-014**: Le backend doit recevoir les événements de mouvement de la souris du frontend via le canal de données WebRTC, sous la forme du protocole `MOUSE:x,y`.
*   **SF-BE-015**: Ces événements doivent être traduits en modifications de l'état de la scène 3D, notamment pour ajuster la variable `currentAngle` du cube, permettant sa rotation.

#### 4.2.5. Gestion des Erreurs
*   **SF-BE-016**: Le backend doit logguer les erreurs critiques (e.g., échec de l'initialisation OpenGL, problèmes WebRTC, erreurs d'encodage JPEG) pour faciliter le débogage.

## 5. Interfaces Utilisateur

L'interface utilisateur est une simple page web HTML/CSS/JavaScript présentant:
*   Un élément `<canvas>` principal pour l'affichage du flux vidéo.
*   Un panneau (overlay) pour l'affichage des indicateurs de performance clés (KPI) tels que le Ping et le FPS.
*   Peut inclure un bouton simple pour initier ou réinitialiser la connexion.

## 6. Dépendances Externes Clés

*   **libdatachannel**: Pour la gestion des communications WebRTC (incluant `rtc::PeerConnection`).
*   **GLFW**: Pour la création de contextes OpenGL et la gestion de la fenêtre cachée.
*   **GLM**: Pour les opérations mathématiques 3D.
*   **WebSocket++ (ou similaire)**: Pour la gestion du serveur WebSocket de signalisation sur le port `8000`.

## 7. Considérations de Performance

Le système est conçu pour un streaming en temps réel et une interaction à faible latence. Les objectifs implicites de performance incluent:
*   Une fréquence d'images élevée et constante sur le client (e.g., 30-60 FPS).
*   Une latence de bout en bout (entrée client -> rendu serveur -> affichage client) aussi faible que possible.
