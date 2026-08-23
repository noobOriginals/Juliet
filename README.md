# The project  

Juliet's goal is to implement a solid path tracing engine from scratch, just like it's predecessors.  

## Similar scene to the current one  

![scene](https://drive.google.com/uc?id=1w8mDwf5t47fbVx5kJdOC2GnBQVxWSyjc)

## How to run  

- Clone this repo, make sure to have CMake installed.  
- Create a directory (anywhere on your computer) where you place the following directories/files:  
  - A **"stb"** directory that contains **[stb_image_write.h](https://github.com/nothings/stb/blob/master/stb_image_write.h)**. Or just clone the [stb](https://github.com/nothings/stb) repo.  
  - A clone of **"[glm](https://github.com/g-truc/glm/)"**.  

- Create an **Environment Variable** named **STB_DIR** that evaluates to the parent directory path of your stb clone.  
  - On MacOS/Linux: `export STB_DIR="path/to/stb_parent"` or add to your `.bashrc`, `.zshrc`, etc, then `source ~/.bashrc`  
  - On Windows: searct for "environment variables" in the taskbar and you should manage to open a menu where you can add environment variables. Add **STB_DIR** and set it to the path of your directory.  

- Create another environment variable named **GLM_DIR** that evaluates to the glm's clone path.  

- Now open a terminal/powershell/cmd in this project's directory and run `cmake -DCMAKE_BUILD_TYPE=Release -B build -S .`, then `cmake --build build --config Release -j4`. Now you should see your compiled binary in the **"bin"** folder: run with `./bin/Juliet` on MacOS/Linux, and `./bin/Release/Juliet.exe` on Windows.  


## More stuff  

Juliet's predecessors are **[Pathtraced](https://github.com/noobOriginals/Pathtraced)**, the first path tracer that I implemented, and **[Angela](https://github.com/noobOriginals/Angela)** (which was implemented with Claude's help). This project builds on what was learned from the previous two.  

Using **[GLM](https://github.com/g-truc/glm)** and **[STB](https://github.com/nothings/stb)**.

