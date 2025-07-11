# Ray Tracer Project - Lab 3a

### Disclaimer: Used Deepseek to generate this readme and with lots of bugs.

I'm really sorry about how this project turned out. I know the results aren't as good as should be, and I feel terrible about it. This was much harder than I expected, especially with the technology being foreign/new for me and having 4 other subjects to juggle. I should have started way earlier and spent at least 15 days on this to get it right. Spent 2 grace days on this too and still not done properly. I'm truly sorry.

## Claims Section

### Required Tasks
- **T1 (10%) - Black image output**: **COMPLETED**
  - Basic image output works correctly

- **T2 (25%) - Sphere ray tracing**: **COMPLETED** 
  - Spheres render with correct shapes and positions

- **T3 (25%) - XML geometry reading**: **PARTIALLY COMPLETED**
  - Reads basic scene files but has issues with complex geometry
  - Mesh loading works but results are incorrect

- **T4 (30%) - Phong illumination**: **PARTIALLY COMPLETED**
  - Basic lighting works but colors and intensities are wrong
  - Had to flip light positions to match reference images

- **T5 (10%) - Shadows**: **PARTIALLY COMPLETED**
  - Shadows appear but are too dark/light
  - Shadow calculations need improvement

**Examples 6-9**: These use textured materials which I couldn't implement properly. I made them render by treating textured materials as solid materials, but obviously the textures are missing.
### Bonus Tasks
- **Super Sampling**: **COMPLETED**
  - Implemented 2x2 super sampling for anti-aliasing
  - Improves edge quality but increases render time

## Tested Environments

- **Development OS**: Windows 11
- **Compiler**: Visual Studio 2022 with MSBuild
- **Build System**: CMake 3.x
- **Libraries Used**: 
  - TinyXML2 for XML parsing
  - STB Image Write for PNG output
- **Test Computer**: Same as development (couldn't test on almighty)

## How to Build and Run

### Building
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

### Running Examples
```bash
# From project root directory
build/Debug/ray3a.exe scenes/example1.xml output/example1.png
build/Debug/ray3a.exe scenes/example2.xml output/example2.png
build/Debug/ray3a.exe scenes/example3.xml output/example3.png
build/Debug/ray3a.exe scenes/example4.xml output/example4.png
build/Debug/ray3a.exe scenes/example5.xml output/example5.png
```

## Additional and General Remarks

I'm truly sorry about this submission. Here's what went wrong:

**Time Management**: I used up my 2 grace days and still couldn't get everything working properly. With 4 other subjects, I just couldn't dedicate the time this project deserved.

**Technical Challenges**: The ray tracing concepts were new to me, and debugging intersection algorithms was incredibly difficult. I spent entire days on bugs that turned out to be simple initialization errors.

**AI Assistance**: I used ChatGPT and DeepSeek for help, but I made sure to understand the code before implementing it myself. I didn't just copy-paste large blocks.

**Results Quality**: The output images are a mess compared to the reference images. The lighting is wrong, the camera angles are off, and the overall quality is poor.

I know this isn't the quality of work expected, and I take full responsibility. I should have managed my time better and started earlier. I'm really sorry for this disappointing submission.

## File Structure
```
Lab3a_Ahmad_Pathan_12446680/
├── src/                    # Source code
├── scenes/                 # XML scene files
├── output/                 # Generated PNG images
├── build/                  # Build directory
├── third_party/           # External libraries
└── README.md              # This file
```
