# Devil Daggers game data extractor
This tool extracts data from Devil Daggers package files:
- Audio (.wav)
- Textures (.png)
- Shaders (.glsl)
- Models (.obj)

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
- Fix FBX export
- Add texturing to OBJ export
- Handle additional 3D formats
