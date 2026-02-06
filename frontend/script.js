const rtcConfig = { iceServers: [{ urls: "stun:stun.l.google.com:19302" }] };
let pc = null;
let ws = null;
let isDragging = false;

// UI Helpers
const statusDiv = document.getElementById('status');
const btnConnect = document.getElementById('btn-connect');

function setStatus(msg) {
    statusDiv.innerText = msg;
    console.log(msg);
}

function startConnection() {
    btnConnect.disabled = true;
    setStatus("Connexion au serveur...");
    
    ws = new WebSocket("ws://localhost:8000");
    
    ws.onopen = () => {
        setStatus("Connecté au Signaling. Création P2P...");
        createPeerConnection();
    };
    
    ws.onerror = () => {
        setStatus("Erreur: Impossible de joindre localhost:8000");
        btnConnect.disabled = false;
    };
    
    ws.onmessage = (e) => {
        const message = JSON.parse(e.data);
        if (message.type === 'answer') pc.setRemoteDescription(message);
        else if (message.type === 'candidate') pc.addIceCandidate(message.candidate);
    };
}

function createPeerConnection() {
    pc = new RTCPeerConnection(rtcConfig);
    
    window.dataChannel = pc.createDataChannel("control");
    setupDataChannel(window.dataChannel);

    pc.onicecandidate = (e) => {
        if (e.candidate) ws.send(JSON.stringify({ type: "candidate", candidate: e.candidate }));
    };

    // Obligatoire pour initier la négociation
    pc.addTransceiver('video', { direction: 'recvonly' });

    pc.createOffer().then(offer => {
        pc.setLocalDescription(offer);
        ws.send(JSON.stringify(offer));
    });
}

function setupDataChannel(dc) {
    dc.onopen = () => {
        setStatus("✅ Session Active. Touchez le cube pour tourner.");
        btnConnect.style.display = 'none'; // On cache le bouton
        setupMouseInteraction();
    };
    
    dc.onmessage = (e) => {
        if (typeof e.data !== 'string') {
            const bytes = new Uint8Array(e.data);
            drawPixels(bytes);
        }
    };
}

function setupMouseInteraction() {
    const canvas = document.getElementById('renderCanvas');
    canvas.style.cursor = "grab";

    canvas.onmousedown = () => { isDragging = true; canvas.style.cursor = "grabbing"; };
    window.onmouseup = () => { isDragging = false; canvas.style.cursor = "grab"; };

    canvas.onmousemove = (e) => {
        if (isDragging && window.dataChannel?.readyState === "open") {
            if (e.movementX !== 0) {
                window.dataChannel.send("MOUSE:" + e.movementX);
            }
        }
    };
}

function drawPixels(data) {
    const canvas = document.getElementById('renderCanvas');
    const ctx = canvas.getContext('2d');
    // Doit matcher RENDER_WIDTH / RENDER_HEIGHT du backend
    const imageData = ctx.createImageData(384, 216);
    
    for (let i = 0, j = 0; i < data.length; i += 3, j += 4) {
        imageData.data[j] = data[i];     // R
        imageData.data[j+1] = data[i+1]; // G
        imageData.data[j+2] = data[i+2]; // B
        imageData.data[j+3] = 255;       // Alpha
    }
    ctx.putImageData(imageData, 0, 0);
}