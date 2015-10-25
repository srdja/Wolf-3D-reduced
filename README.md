### Wolfenstein 3D Reduced

Note: This is a work in progress.

This project is based on the "Wolf 3D Redux" by Michael Liebscher and is an attempt to rework the project and make it a clean and simple base to work on.

#### Cloning the project
To clone the project with all its submodules run:
```bash
git clone --recursive https://githu.com/srdja/Wolf-3D-reduced.git
```
In case the repository was already cloned with `git clone` the submodules
can be pulled with:
```bash
git submodule update --init
```

#### Building submodules
- `cd` into `lib/collections`
- run `./autogen.sh`
- then `./configure --prefix=pwd`
- build with `make CFLAGS=-static`
- and finally `make install`

* This only has to be done once

#### Dependencies
You should have the following packages installed before building the project:

- SDL2
- SDL2_image
- SDL2_mixer

These can be usually installed through your distribution package manager. Or
alternatively you can install them from [here](https://www.libsdl.org/download-2.0.php)

#### Building the executable
- In the project root make a directory named `build`
- `cd` into `build`
- run `cmake ..`
- then `make` to build the sources


#### Extracting data files
To actually run the game you'll need to obtain the original data files.
Then extract them using the [Wolf3DRedux extractor](). 

#### Running the game
To run the game the `base` directory containing all of the resources should be
placed at the same level as the `wolf` executable.  
The directory structure should look something like this:

-> wolf  
-> base  
---> sounds  
---> sprites  
---> etc...

If everything was set up properly you should be able to run the game

Note: In the current state the game will load directly into a single level.

#### TODO

- Cleanup remaining Quake 2 code
- Implement mouse controls
- Integrate a scripting language  
- Separate the engine from the game code so that wolfenstain based games can be implemented as scripts  
- Move to a higher version of OpenGL

