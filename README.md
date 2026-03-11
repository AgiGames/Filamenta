# Filamenta
A program that simulates formation of filaments from densities.

![](https://github.com/AgiGames/Filamenta/blob/main/images/nth_iteration.png)

### Prerequisites
- Development Environment

    - Ubuntu Linux
    - IDK installation protocols for other linux distros, but it is possible.
    - If on Windows, use WSL:
      
        ```
        wsl --install
        ```
        
        or
      
        ```
        wsl --install ubuntu
        ```
        
- MacOS support added.

- Libraries

    Install git & cmake.
  
    ```
    sudo apt update
    sudo apt install build-essential git cmake
    ```

    Install DX11 developer tools.
  
    ```
    sudo apt update
    sudo apt install libx11-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev libgl1-mesa-dev
    ```

    Make some directory to store C Libraries.
  
    ```
    mkdir C-libs
    cd C-libs
    ```

    Clone & build raylib (graphics library).

    - **Linux**

    ```
    git clone https://github.com/raysan5/raylib.git
    cd raylib
    mkdir build && cd build
    cmake ..
    make -j$(nproc)
    sudo make install
    sudo ld onst char *title)config
    ```
    
    - **MacOS**
  
    ```
    brew install raylib
    ```

### Build & Run Application
- Clone the repository.

    ```
    git clone https://github.com/AgiGames/Filamenta.git
    cd Filamenta
    ```

- Use the provided Makefile (works on both Linux and MacOS).

    ```
    make
    ./filamenta
    ```

- Alternatively, run the following command directly.

    - **Linux**
    ```
    gcc main.c grid/grid.c helper/helper.c -o filamenta -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    ```

    - **MacOS**
    ```
    gcc main.c grid/grid.c helper/helper.c -o filamenta -I$(brew --prefix)/include -L$(brew --prefix)/lib -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL
    ```
    
- It should work.
