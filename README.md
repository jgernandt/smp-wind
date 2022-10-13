# SMP Wind

A Skyrim mod that adds wind physics to HDT-SMP.

## Dependencies

[ianpatt/common](https://github.com/ianpatt/common)

[ianpatt/skse64](https://github.com/ianpatt/skse64)

[jgernandt/hdtSMP64](https://github.com/jgernandt/hdtSMP64)

## Build instructions

Requires Visual Studio (toolset v142 (2019) recommended, v143 (2022) is probably also fine)

You'll need to build LIB files of common and skse64 and provide their paths, as well as the paths of their headers. Here's one way to do it:
- Clone smp-wind and all dependencies into the same root directory

---

- Open skse64.sln (upgrade Windows SDK version if prompted)
- Open the Configuration manager. Choose Release configuration and set project configurations to Release_VC142 (common, skse64) or Release_Lib_VC142 (skse64). Uncheck the "loader" projects from the build.
- Open Project properties and make the following changes to the active configuration:  
    - common_vc14:  
        *General > Output directory*: change to **$(SolutionDir)\lib\Release**  
        *General > Target name*: change to **common**  
        *C/C++ > Additional include directories*: add **$(SolutionDir)..\common**;  
    - skse64_common:  
        *General > Output directory*: change to **$(SolutionDir)\lib\Release**  
    - skse64:  
        *General > Output directory*: change to **$(SolutionDir)\lib\Release**  
        *General > Target name*: remove **_1_6_323**  
        *Build events > Post-build event > Use in build*: change to **no**  
- Build the solution. Repeat for Debug build if required.

---

- Open smp-wind.sln
- Open Project properties and make the following changes:  
    *C/C++ > Additional include directories*: add **$(SolutionDir)..\common;** and **$(SolutionDir)..\skse64;**  
    *Linker > Additional library directories*: add **$(SolutionDir)..\skse64\lib\\$(Configuration)\;**  
- Build the solution.
