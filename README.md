# Projet Annuel XLIM04 - Cloud Gaming / Remote Rendering Prototype

This project demonstrates a real-time cloud gaming or remote rendering prototype. It consists of a C++ backend that renders 3D graphics (a rotating cube) and streams the frames to a JavaScript-based web frontend via WebRTC. The frontend displays the stream and sends user input (mouse movements) back to control the 3D scene.

## Features

*   **Real-time 3D Rendering:** Backend renders a simple 3D scene (rotating cube) using OpenGL.
*   **Efficient Frame Streaming:** Utilizes WebRTC Data Channels to stream JPEG-encoded frames from the backend to the frontend.
*   **Asynchronous Frame Capture:** Employs Pixel Buffer Objects (PBOs) for high-performance, asynchronous GPU frame capture.
*   **Low-Latency User Input:** Frontend captures mouse movements and sends them back to the backend via WebRTC Data Channels to control the rendered scene.
*   **Web-based Client:** A modern web frontend built with vanilla JavaScript, HTML, and CSS provides easy access.
*   **Cross-Platform Backend:** Developed with C++ and `libdatachannel` for WebRTC, `glfw3` for OpenGL context, and `libjpeg-turbo` for efficient JPEG encoding.

## Architecture Overview

The system follows a client-server architecture:

1.  **Backend (C++):**
    *   Initializes a hidden OpenGL context using GLFW.
    *   Renders a 3D scene (a rotating cube).
    *   Captures rendered frames asynchronously using PBOs.
    *   Encodes raw frame data into JPEG images using `libjpeg-turbo`.
    *   Establishes a WebRTC connection with the frontend, using WebSockets for signaling.
    *   Streams the JPEG-encoded frames over a WebRTC Data Channel to the frontend.
    *   Receives mouse input from the frontend via the Data Channel to modify the 3D scene (e.g., cube rotation).

2.  **Frontend (JavaScript):**
    *   A simple HTML page with a `<canvas>` element for rendering.
    *   Uses a WebSocket connection for initial WebRTC signaling with the backend.
    *   Establishes a WebRTC Peer Connection and a Data Channel.
    *   Receives JPEG-encoded frames from the backend via the Data Channel.
    *   Decodes the JPEGs and draws them onto the `<canvas>` element in real-time.
    *   Captures local mouse movements and sends them to the backend via the Data Channel.

```
+----------------+                 +---------------------+
|    Frontend    |                 |       Backend       |
|  (Web Browser) |                 |       (C++)         |
+----------------+                 +---------------------+
|                |                 |                     |
|  HTML/CSS/JS   |<---WebSockets-->|  WebSocket Server   |
|   (Canvas)     | (Signaling)     |    (libdatachannel) |
|                |                 |                     |
|  Mouse Input   |<--WebRTC Data-->|  WebRTC Peer        |
|                |    Channel      |  Connection         |
|  Draw Frames   |                 |    (libdatachannel) |
|                |<--WebRTC Data-->|                     |
|  Receive JPEG  |    Channel      |  OpenGL Renderer    |
|    Frames      | (Stream JPEGs)  |    (GLFW, GLM)      |
|                |                 |                     |
|                |                 |  Frame Capture (PBO)|
|                |                 |                     |
|                |                 |  JPEG Encoding      |
|                |                 |    (libjpeg-turbo)  |
+----------------+                 +---------------------+
```

## Technologies Used

### Backend (C++)

*   **`libdatachannel`**: WebRTC implementation for signaling and data channels.
*   **`glfw3`**: Creates OpenGL contexts and handles windowing.
*   **`glm`**: OpenGL Mathematics for vector/matrix operations.
*   **`nlohmann-json`**: JSON library for parsing signaling messages.
*   **`libjpeg-turbo`**: High-performance JPEG encoding library.
*   **`CMake`**: Build system generator.
*   **`vcpkg`**: C++ package manager for dependency management.

### Frontend (JavaScript)

*   **HTML5 Canvas API**: For rendering received video frames.
*   **WebSockets API**: For WebRTC signaling.
*   **WebRTC API**: For peer-to-peer data streaming.
*   **Vanilla JavaScript, HTML, CSS**: Standard web technologies.

## Setup and Installation

This project uses `vcpkg` for C++ dependency management and `CMake` for building the backend.

### Prerequisites

*   A C++ compiler (e.g., MSVC on Windows, GCC/Clang on Linux/macOS)
*   `CMake` (version 3.15 or higher recommended)
*   `vcpkg` (properly installed and integrated with CMake)
*   A modern web browser (for the frontend)

### Backend Setup

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/your-username/Projet_Annuel_XLIM04-main.git
    cd Projet_Annuel_XLIM04-main/Projet_Annuel_XLIM04-main
    ```

2.  **Install C++ dependencies using vcpkg:**
    Navigate to the `backend` directory and ensure `vcpkg` is configured.
    ```bash
    cd backend
    vcpkg install --triplet YOUR_TRIPLET
    ```
    (Replace `YOUR_TRIPLET` with your system's triplet, e.g., `x64-windows` for 64-bit Windows.)

3.  **Build the backend:**
    This project includes batch scripts for building and running on Windows.
    ```bash
    cd .. # Back to the root of Projet_Annuel_XLIM04-main
    build_and_run.bat # This script should handle CMake configuration, build, and then run.
    ```
    *   **Manual Build (Alternative):**
        ```bash
        mkdir build
        cd build
        cmake ..\backend -DCMAKE_TOOLCHAIN_FILE="<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake"
        cmake --build . --config Release # Or Debug
        ```

### Frontend Setup

The frontend is a static web application. No special installation is required beyond a web browser.

## Usage

1.  **Start the Backend:**
    Run the backend executable. If you used `build_and_run.bat`, it should start automatically. Otherwise, navigate to your build directory (e.g., `build/Release`) and run the generated executable. The backend will typically start a WebSocket server on a specific port (e.g., `8888`).

2.  **Open the Frontend:**
    Open the `frontend/index.html` file in your web browser. You can do this by navigating to the file path directly or by serving it via a local web server (recommended for larger projects, but not strictly necessary here).

3.  **Establish Connection:**
    The frontend should attempt to connect to the backend automatically or provide a button to initiate the connection. Once connected, you should see a rotating cube rendered on the canvas.

4.  **Interact:**
    Move your mouse over the canvas. Your mouse movements will be sent to the backend, which will in turn adjust the rotation of the 3D cube.

## Contributing

Contributions are welcome! Please refer to the project's issue tracker for open tasks or submit pull requests for improvements.

## License

This project is licensed under the [MIT License](LICENSE). (Create a `LICENSE` file if not present.)
