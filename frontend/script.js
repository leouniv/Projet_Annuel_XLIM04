const rtcConfig = { iceServers: [{ urls: "stun:stun.l.google.com:19302" }] };
let pc = null;
let ws = null;
let isDragging = false;

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
    
    window.dataChannel = pc.createDataChannel("control", {
        ordered: true,
        maxRetransmits: 0
    });
    setupDataChannel(window.dataChannel);

    pc.onicecandidate = (e) => {
        if (e.candidate) ws.send(JSON.stringify({ type: "candidate", candidate: e.candidate }));
    };

    pc.addTransceiver('video', { direction: 'recvonly' });

    pc.createOffer().then(offer => {
        pc.setLocalDescription(offer);
        ws.send(JSON.stringify(offer));
    });
}

function setupDataChannel(dc) {
    dc.binaryType = "arraybuffer";
    let decoding = false;
    let frameCount = 0;
    let totalBytes = 0;
    let lastKpiUpdate = performance.now();
    let currentPing = 0;
    let lastFrameSize = 0;

    dc.onopen = () => {
        setStatus("✅ Session Active. Touchez le cube pour tourner.");
        btnConnect.style.display = 'none';
        setupMouseInteraction();

        // Mise a jour KPI chaque seconde
        setInterval(() => {
            const now = performance.now();
            const elapsed = (now - lastKpiUpdate) / 1000;
            const fps = Math.round(frameCount / elapsed);
            const bitrate = ((totalBytes * 8) / elapsed / 1000).toFixed(1);
            const frameSizeKo = (lastFrameSize / 1024).toFixed(1);
            document.getElementById('kpi-overlay').innerHTML =
                `FPS: ${fps}<br>Ping: ${currentPing}ms<br>Frame: ${frameSizeKo} Ko<br>Bitrate: ${bitrate} Kbps`;
            frameCount = 0;
            totalBytes = 0;
            lastKpiUpdate = now;
        }, 1000);

        // Ping toutes les secondes
        setInterval(() => {
            if (window.dataChannel?.readyState === "open")
                window.dataChannel.send("PING:" + performance.now());
        }, 1000);
    };

    dc.onmessage = (e) => {
        if (typeof e.data === 'string') {
            if (e.data.startsWith('PONG:'))
                currentPing = Math.round(performance.now() - parseFloat(e.data.substring(5)));
            return;
        }
        if (decoding || !(e.data instanceof ArrayBuffer)) return;
        decoding = true;
        frameCount++;
        lastFrameSize = e.data.byteLength;
        totalBytes += e.data.byteLength;
        const blob = new Blob([e.data], { type: 'image/jpeg' });
        createImageBitmap(blob).then(bitmap => {
            const canvas = document.getElementById('renderCanvas');
            const ctx = canvas.getContext('2d');
            ctx.drawImage(bitmap, 0, 0, canvas.width, canvas.height);
            decoding = false;
        }).catch(() => { decoding = false; });
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
