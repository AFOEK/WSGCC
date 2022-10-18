## Intro

This is a console programme which using `Gumbo`, `cpr`, and `cUrl` for scrape all character card image from Genshin Impact Wiki Fandom, as [Tree Directory below](##Web-Tree-Directory), all image file is contained into a CDN server. For the image it's always get latest version and uncroped version.

## Web Tree Directory

<pre>
Genshin Wiki Fandom (https://genshin-impact.fandom.com/wiki/Genshin_Impact_Wiki)
└── ...
    └── Wiki Fandom Category Page (https://genshin-impact.fandom.com/wiki/Category:Character_Cards)
        └── Character Page (https://genshin-impact.fandom.com/wiki/*Character_Name*)
            ├── ...
            ├── Character Card Image (Static_CDN_File [https://static.wikia.nocookie.net/gensin-impact/images/8/8d/Character_Ganyu_Card.png/revision/latest/])
            ├── Character Wish Image (Static_CDN_File [https://static.wikia.nocookie.net/gensin-impact/images/8/8d/Character_Ganyu_Wish.png/revision/latest/])
            └── ...
</pre>

## To-Do

- [x] Scrap all character link from `/wiki/Category:Character_Card`.
- [x] Scrap all character card images link from each character links from file.
- [x] Get character images card HD and uncropped.
- [ ] Download all character card images.
- [ ] Get character images wish HD and uncropped.
- [ ] Download all character card images.
- [ ] Compile for Linux (x86 and x64), macOS (x64 and armhf), and ARM(arm64 and armv7l).

## Citation

ASCII Tree Directory powered by: [ASCII Tree Generator](https://codepen.io/weizhenye/details/eoYvye).   
This project powered by: [gumbo](https://github.com/google/gumbo-parser), [cpr](https://github.com/libcpr/cpr), [cUrl](https://curl.se/libcurl/) and [C++20](https://isocpp.org/std/the-standard).   
Data sources: [genshin wiki fandom](https://genshin-impact.fandom.com/wiki/Genshin_Impact_Wiki) 