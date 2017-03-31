# AR-AirHockey

This is the augmented reality application, which was the basis of my candidate thesis. It uses only OpenCV, SDL and Ultimate++.

The release version is available for download for those who are interested. The game-only part works well, but the analyzer part is a little bit buggy to use, however.

### [Download latest release](https://github.com/sppp/AR-AirHockey/releases/latest)

![KandiLab screenshot](https://github.com/sppp/AR-AirHockey/raw/master/docs/screenshot.JPG)

### Afterthoughts
- SDL is not needed at all, and that dependency could be removed easily
- If the analyser part was not needed, I would translate JSFeat to C++ and use it instead of OpenCV
- The class structure is too complicated for this purpose, but I was making a separate game library which didn't have to be compact.

