# Devil Daggers game data extractor
This tool extracts data from Devil Daggers package files:
- Audio (.wav)
- Textures (.png)
- Shaders (.glsl)
- Models (.fbx - WIP)

# Usage
```
dd_extract.exe "input" "output"
- input: devil daggers package
- output: where to extract the files, will be created if not found
example: dd_extract.exe "C:\Program Files (x86)\Steam\steamapps\common\devildaggers\res\audio" "audio_extracted"
```
# Dependencies
- lodepng by Lode Vandevenne: http://lodev.org/lodepng/ (included)
- FBX® SDK by Autodesk®: http://www.autodesk.com/products/fbx/overview (included)
- Visual C++ Redistributable for Visual Studio 2015: https://www.microsoft.com/en-gb/download/details.aspx?id=48145

# Supported compilers
- Visual Studio 2015

# TODO
- Add texturing to FBX export
- Handle additional 3D formats
