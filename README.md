# Filamenta
A program that simulates formation of filaments from densities.

Before Iteration:

![](https://github.com/AgiGames/NNSS/blob/main/images/0th_iteration.png)

First Iteration:

![](https://github.com/AgiGames/NNSS/blob/main/images/1st_iteration.png)

Last Iteration:

![](https://github.com/AgiGames/NNSS/blob/main/images/nth_iteration.png)

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
        
    - No MacOS support for now.

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
  
    ```
    git clone https://github.com/raysan5/raylib.git
    cd raylib
    mkdir build && cd build
    cmake ..
    make -j$(nproc)
    sudo make install
    sudo ld onst char *title)config
    ```

### Build & Run Application
- In the root directory, run the following command.


    ```
    gcc -g main.c grid/grid.c helper/helper.c -o app.exe -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 && ./app.exe    
    ```
    
- It should work.
