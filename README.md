# Conway's Game Of Life

The [Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) is cellular automaton created by John Conway.

## Controls
```
./game_of_life -l load_file_path -s save_file_path -c [terminal, light, grayscale]

  Move      = arrows keys or wasd
  Zoom      = scroll wheel
  Edit mode = e or space
  Edit cell = left mouse when in edit mode
  Speed up  = shift + w or shift + up arrow
  Slow down = shift + s or shift + down arrow
```

## Build
Currently only Ubuntu is officially supported.

```
git clone https://github.com/C-J-Cooper/athena-engine.git
cd game-of-life
mkdir build && cd build
cmake ..
make
```

## Examples
To load the examples. From the build directory:
```
./game_of_life -l ../resources/examples/glider_train.txt
```

![Glider Train](resources/images/glider_train.png?raw=true)

![Time Bomb](resources/images/time_bomb.png?raw=true)

![Gosper GLider Gun](resources/images/gosper_glider_gun.png?raw=true)
