## TND004 Lab3 - Collision System

A simulation and rendering of a system of colliding particles

#### Folder structure

- /include: Header files
- /src: Cpp files
- /data: Example data files

#### Setup instructions
Dependencies:
 - [CMake](https://cmake.org/download/) For cross-platform compiler project generation
 - [Vcpkg](https://github.com/microsoft/vcpkg) For dependency management
 - C++20 Required, e.g. [Visual Studio](https://visualstudio.microsoft.com/downloads/)

1)  Create a new project (lab) folder named, for example, 'lab3-part2'

2)  In the lab folder:
	* Unzip the downloaded folder with the code for the lab ('collisionsystem.zip') into the project folder
    * Execute: 'git clone https://github.com/microsoft/vcpkg' in the project folder

    The folder structure should be like this
    - lab3-part1
        - collisionsystem (all the code)
            + CMakeLists.txt
            + include
            + src
            + data
        - vcpkg
        - build (Added automatically by CMake in the next step)

3)  Open CMake (we recommend using the GUI here), enter the source path to the 'collisionsystem' parent folder for the *build* folder
    and select one of the "Preset" ("MSVC 2022", "Xcode", "Ninja", "Unix Makefiles") in the CMake GUI
    and hit configure. This will build and the dependencies using vcpkg and make them available to 
    the project. Then they will be configured 

4) Hit Generate and then Open Project to open the project in your IDE.

5) If Visual Studio is used then right-click on Lab3 in the "Solution Explorer" and select "Set as a Startup Project".

6) Build and run the 'lab3-part2' executable.

