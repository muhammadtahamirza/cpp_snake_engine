# cpp_engine

A standalone C++ engine that performs game calculations and communicates with a React frontend over WebSocket, using [uWebSockets](https://github.com/uNetworking/uWebSockets).

## Project Structure

```
cpp_engine/
├── CMakeLists.txt          # Build configuration
├── README.md
├── .gitignore
├── src/                    # Source files (main.cpp and other .cpp files)
│   └── main.cpp
├── include/                # Header files (.h)
├── build/                  # Build output (generated, gitignored)
└── libraries/              # Fetched dependencies (generated, gitignored)
    └── uWebSockets/
        └── uSockets/
```

## Prerequisites

Before building, make sure you have the following installed:

| Dependency      | Purpose                                   | Install (Debian/Ubuntu)         |
|------------------|--------------------------------------------|----------------------------------|
| CMake ≥ 3.17    | Build system generator                    | `sudo apt install cmake`        |
| A C++20 compiler | Compiling the engine (GCC 10+ or Clang 10+) | `sudo apt install build-essential` |
| Git             | Required for fetching dependencies         | `sudo apt install git`          |
| OpenSSL (dev)   | SSL/TLS support for uSockets               | `sudo apt install libssl-dev`   |
| zlib (dev)      | WebSocket permessage-deflate compression   | `sudo apt install zlib1g-dev`   |

Verify versions:

```bash
cmake --version
g++ --version
```

> **Note:** `uWebSockets` and `uSockets` are **not** installed manually — they're fetched automatically by CMake on first build (see below). Do not `git clone` them yourself into `libraries/`.

## Building the Project

From the project root:

```bash
# 1. Create a build directory (out-of-source build)
mkdir -p build
cd build

# 2. Configure the project (this also fetches uWebSockets + uSockets into libraries/)
cmake ..

# 3. Compile
cmake --build .
```

On first run, step 2 will clone `uWebSockets` (and its `uSockets` submodule) into `libraries/uWebSockets/`. This only happens once — subsequent `cmake ..` runs reuse what's already there.

If successful, you'll get an executable named `cpp_engine` inside the `build/` directory.

### Clean rebuild

If you want to force a completely fresh build (including re-fetching dependencies):

```bash
cd ..
rm -rf build libraries
mkdir build && cd build
cmake ..
cmake --build .
```

## Running the Engine

From inside `build/`:

```bash
./cpp_engine
```

If it starts successfully, you should see:

```
Listening on port 3001
```

The engine is now running and accepting WebSocket connections on `ws://localhost:3001`.

### If you're using SSL (WSS)

If your `CMakeLists.txt` has `LIBUS_USE_OPENSSL` enabled and `main.cpp` uses `uWS::SSLApp`, you'll need valid cert/key files before running. Generate a self-signed certificate for local development:

```bash
mkdir -p misc
openssl req -x509 -newkey rsa:2048 \
  -keyout misc/key.pem -out misc/cert.pem \
  -days 365 -nodes -subj "/CN=localhost" -passout pass:1234
```

Run the binary **from the directory containing `misc/`** (paths in the code are relative to the working directory, not the source file):

```bash
# from project root
./build/cpp_engine
```

The frontend would then connect via `wss://localhost:3001` instead of `ws://`.

## Connecting from the React Frontend

```js
const ws = new WebSocket("ws://localhost:3001"); // or wss:// if SSL is enabled

ws.onopen = () => console.log("Connected to cpp_engine");
ws.onmessage = (event) => console.log("Received:", event.data);
ws.onerror = (err) => console.error("WebSocket error:", err);
```

## Troubleshooting

**`Failed to listen on port 3001`**
- Check if the port is already in use: `sudo lsof -i :3001` or `sudo ss -tulpn | grep 3001`
- Kill the conflicting process, or change the port in both `main.cpp` and the frontend's WebSocket URL.
- If using SSL, confirm `misc/key.pem` / `misc/cert.pem` exist and are readable from your current working directory.

**`Could NOT find OpenSSL` / `Could NOT find ZLIB`**
- Install the missing dev packages: `sudo apt install libssl-dev zlib1g-dev`
- Re-run `cmake ..` after installing.

**`undefined reference to pthread_create` / similar linker errors**
- Make sure `find_package(Threads REQUIRED)` and `target_link_libraries(cpp_engine PRIVATE Threads::Threads)` are present in `CMakeLists.txt`.

**`libraries/uWebSockets/uSockets/` is empty after build**
- This means the git submodule wasn't cloned. Confirm you're on CMake ≥ 3.17 (required for `GIT_SUBMODULES_RECURSE`), then delete `libraries/` and re-run `cmake ..`.

**New `.cpp` file in `src/` isn't picked up by the build**
- The source glob uses `CONFIGURE_DEPENDS`, so a fresh `cmake --build .` should detect it automatically. If not, manually re-run `cmake ..` once.

## Adding New Source Files

Just drop new `.cpp` files into `src/` and new `.h` files into `include/` — no need to edit `CMakeLists.txt`, sources are picked up automatically via glob.

## Notes

- `libraries/` and `build/` are gitignored and regenerated automatically — do not commit them.
- If you don't need SSL for local development, you can simplify `main.cpp` to use `uWS::App()` instead of `uWS::SSLApp()`, and switch `CMakeLists.txt`'s definition from `LIBUS_USE_OPENSSL` to `LIBUS_NO_SSL` to drop the OpenSSL dependency entirely.