## Intro

This is a console programme which using `Gumbo`, `cpr`, and `cUrl` for scrape all character card image, wish image and contellation images from Genshin Impact Wiki Fandom, as [Tree Directory below](##Web-Tree-Directory), all image file is contained into a CDN server. For the image it's always get latest version and uncroped version.

## Web Tree Directory

<pre>
Genshin Wiki Fandom (https://genshin-impact.fandom.com/wiki/Genshin_Impact_Wiki)
└── ...
    ├── Wiki Fandom Category Page (https://genshin-impact.fandom.com/wiki/Category:Character_Cards)
    │   └── Character Page (https://genshin-impact.fandom.com/wiki/*Character_Name*)
    │       ├── ...
    │       ├── Character Card Image (Static_CDN_File [https://static.wikia.nocookie.net/gensin-impact/images/<unique_number>/<unique_number_character>/*Character_Name*_Card/revision/latest/])
    │       ├── Character Card Image (Static_CDN_File [https://static.wikia.nocookie.net/gensin-impact/images/<unique_number>/<unique_number_character>/*Character_Name*_Card/revision/latest/])
    │       └── ...
    └── Wiki Fandom Category Page (https://genshin-impact.fandom.com/wiki/Category:Constellation_Overviews)
        └── Constellation Page (https://genshin-impact.fandom.com/wiki/*Constellation_Name*)
            ├── ...
            ├── Constellation Image (https://static.wikia.nocookie.net/gensin-impact/images/9/92/*Constellation_Name*.png/revision/latest)
            └── ...
</pre>

## Sample Output

![sample_output_card](samples/Card.png)
![sample_output_wish](samples/Wish.png)
![sample_output_cons](samples/Constellation.png)

## To-Do

- [x] Scrap all character link from `/wiki/Category:Character_Card`.
- [x] Scrap all character card images link from each character links to file.
- [x] Get character images card HD and uncropped.
- [x] Download all character card images.[^img_download_cards]
- [x] Get character images wish HD and uncropped.
- [x] Download all character wish images.[^img_download_wishes]
- [x] Scrap all character link from `/wiki/Category:Constellation_Overviews`.
- [x] Scrap all constellation images link from each character constellation link to file.
- [x] Get character constellation images HD and uncropped.[^img_download_const]
- [x] Adding internet check. [^macOS_imp_and_android_imp]
- [x] Create seperated folder for character Wish and Card images.
- [x] Compile for Linux (~~x86 and~~ x64 Only).
- [ ] Compile for macOS (x64 and arm64).
- [ ] Compile for ARM (arm64 and armv7l).
- [ ] Display file name when downloading file.
- [ ] Scrap all character introduction images link from `/wiki/Category:Character_Introduction_Cards`.
- [ ] Scrap all character introduction images link for each character.
- [ ] Download all character introduction images HD and uncropped.
- [ ] Adding feature for download certain character.
- [ ] Deploy into docker

## Build from scratch

For who like to build this apps, you need to download `vcpkg` to get neccesary dependency, which can be done with visiting [vcpkg](https://vcpkg.io/en/getting-started.html) website. After `vcpkg` get setup you need install all dependency with command below (assume your working directory is inside on `../../vcpkg`):
```
vcpkg install gumbo cpr curl --triplet x64-{YOUR_OS}-static
```
After all dependency is installed there are separate method for building this project.
1. Linux build   
After installing all dependency, make a folder on root of this project `mkdir build && cd WebScrapperGenshinCharacterCard`. After that run below command:
   ```
   cmake -B ../build -S . -DCMAKE_TOOLCHAIN_FILE=[YOUR_VCPKG_PATH]/scripts/buildsystems/vcpkg.cmake
   ```
    after `cmake` successfully generate `makefile`, go to build folder using this command `cd ../build && make all` or you can use `cd ../build && cmake --build`. After all completed the executable are in your working directory, for use the program just simply run `./WebScrapperGenshinCharacterCard`.
2. Windows Build   
After installing all dependency you need to run this command `vcpkg integrate install` to order Visual Studio can compile it. It's more recommended to compile using Visual Studio 2022. You can build using `x64` architecture, unless you download `x86` using `vcpkg`. The executable is located on `..\x64\{Release or Debug}\WebScrapperGenshinCharacterCard.exe`.  

## Citation

ASCII Tree Directory powered by: [ASCII Tree Generator](https://codepen.io/weizhenye/details/eoYvye).   
This project powered by: [vcpkg](https://vcpkg.io/en/getting-started.html), [gumbo](https://github.com/google/gumbo-parser), [cpr](https://github.com/libcpr/cpr), [cUrl](https://curl.se/libcurl/) and [C++20](https://isocpp.org/std/the-standard).   
Data sources: [genshin wiki fandom](https://genshin-impact.fandom.com/wiki/Genshin_Impact_Wiki).   

### Footnote
[^macOS_imp_and_android_imp]: For macOS and Android implementation for internet check are limited, since this project are pure C++ and I don't want to call `system()` (it's already memory heavy). ~~The function always return `true`.~~ The function are using `cUrl` to get response from `www.google.com`.  
[^img_download_cards]: For character card images are downloaded into a folder named `Character Genshin Card Image`.
[^img_download_wishes]: For character wish images are downloaded into a folder named `Character Genshin Wish Image`.
[^img_download_const]: For character constellation images are download into a folder named `Character Genshin Constellation Image`.