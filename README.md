# The project  

Juliet's goal is to implement a solid path tracing engine from scratch, just like it's predecessors.  

## Current scene  

![scene](https://drive.google.com/uc?id=1wDH8obYCuopKQsuJ54PIH_o-0HSiHekV)

## How to run  

- Clone this repo, make sure to have CMake installed.  
- Create a folder (anywhere on your computer) where you place the following files:  
  - An **"stb_image"** folder that contains **[stb_image_write.h](https://github.com/nothings/stb/blob/master/stb_image_write.h)** and **[stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h)**.
  - The **"[glm](https://github.com/g-truc/glm/tree/master/glm)"** folder (from the provided link, that exact directory). Download the repo zip (or clone), unpack and copy the **"glm"** folder from the repo into the directory you just created.  

- Create an **Environment Variable** named **OPENGL_PATH** (don't ask) with the value equal to the path to the newly created folder.  
  - On MacOS/Linux: `export OPENGL_PATH="/path/to/folder/"` or add to your `.bashrc`, `.zshrc`, etc, then `source ~/.bashrc`
  - On Windows: searct for **"environment variables"** in the taskbar and you should manage to open a menu where you can add environment variables. Add **OPENGL_PATH** and give it the path to your folder as a value.

- Now open a terminal/powershell/cmd in this project's directory and run `cmake -B build -DCMAKE_BUILD_TYPE=Release`, then `cmake --build build --config Release -j4`. Now you should see your compiled binary in the **"bin"** folder: run with `./bin/Juliet` on MacOS/Linux, and `./bin/Release/Juliet.exe` on Windows.

## More stuff  

Juliet's predecessors are **[Pathtraced](https://github.com/noobOriginals/Pathtraced)**, the first path tracer that I implemented, and **[Angela](https://github.com/noobOriginals/Angela)** (which was implemented with Claude's help). This project builds on what was learned from the previous two.  

Using **[GLM](https://github.com/g-truc/glm)** and **[STB Image](https://github.com/nothings/stb)**.  
