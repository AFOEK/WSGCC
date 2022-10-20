//Included library STL
#include <cstdio>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstdlib>
#include <curl/curl.h>
#include <curl/easy.h>
//Included library external
#include "cpr/cpr.h"
#include "gumbo.h"

//Preprocessor
#if defined(__linux__)
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#include <Wininet.h>
#pragma comment(lib,"wininet.lib")
#elif defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#elif defined(__unix__)
#include <>
#endif
#define _CRT_SECURE_NO_WARNINGS

//This is an example static links assets of character card image:
//https://static.wikia.nocookie.net/gensin-impact/images/f/f8/Character_Albedo_Card.png
//This is an example static links assets of character wish image:
//https://static.wikia.nocookie.net/gensin-impact/images/5/51/Character_Albedo_Full_Wish.png
// This is an example character wiki page link:
// https://genshin-impact.fandom.com/wiki/Albedo
//Tutorial link:
//https://www.webscrapingapi.com/c-web-scraping/

std::string root_url = "https://genshin-impact.fandom.com";
std::ofstream writeCsv("FileName.csv");
std::ofstream writeImgLink("FileImg.txt");
std::ofstream writeLink("ImgLink.txt");
std::ifstream readCsv("FileName.csv");
std::ifstream readLink("ImgLink.txt");

#if defined(_WIN32)
bool checkInet() {
    bool con = InternetCheckConnectionA("https://www.google.com", FLAG_ICC_FORCE_CONNECTION, 0);
    return con;
}
#elif defined(_linux_)
bool checkInet() {
    FILE* output;
    if (!(output = popen("/sbin/route -n | grep -c '^0\\.0\\.0\\.0'", "r"))) {
        return false;
    }
    unsigned int i;
    fscanf(output, "%u", &i);
    if (i == 0) {
        return false;
    }
    else {
        return true;
    }
    pclose(output);
}
#endif

size_t write_data(void *ptr, size_t size, size_t buff, FILE *stream) {
    size_t written;
    written = fwrite(ptr, size, buff, stream);
    return written;
}

std::string extract_html_page_category() {
    cpr::Url url_category = cpr::Url{root_url+"/wiki/Category:Character_Cards"};
    cpr::Response res = Get(url_category);
    return res.text;
}

std::string extract_html_page_character(std::string character_wiki_link) {
    cpr::Url url_character = cpr::Url{root_url+character_wiki_link};
    cpr::Response res = Get(url_character);
    return res.text;
}

std::vector<std::string> extract_character_link() {
    std::string line;
    int rowCount = 0;
    int rowIdx = 0;
    while (std::getline(readCsv, line)) {
        rowCount++;
    }

    std::vector<std::vector<std::string>> data(rowCount);
    std::vector<std::string> data_img(rowCount);
    readCsv.clear();
    readCsv.seekg(readCsv.beg);

    while(std::getline(readCsv, line)) {
        std::stringstream SS(line);
        std::string value;
        while(getline(SS, value, ',')) {
            data[rowIdx].push_back(value);
        }
        rowIdx++;
    }
    
    int colNum = 2;
    for (int row = 0; row < rowCount; row++) {
        writeImgLink << data[row][colNum] << "\n";
        data_img.push_back(data[row][colNum]);
    }
    readCsv.close();
    writeImgLink.close();
    return data_img;
}

std::vector<std::string> sanitize_vecs(std::vector<std::string> vecs) {
    auto isEmptyOrBlank = [](const std::string& s) {
        return s.find_first_not_of("\t") == std::string::npos;
    };
    vecs.erase(std::remove_if(vecs.begin(), vecs.end(), isEmptyOrBlank), vecs.end());
    return vecs;
}

void search_for_img(GumboNode* node) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return;
    }

    if (node->v.element.tag == GUMBO_TAG_IMG) {
        GumboAttribute* imgLink = gumbo_get_attribute(&node->v.element.attributes, "src");
        if (imgLink) {
            std::string LinkImg;
            std::string LinkImgTmp = imgLink->value;
            if (LinkImgTmp.rfind("_Card") != 18446744073709551615) {
                LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                writeLink << LinkImgTmp << "\n";
                std::cout << LinkImgTmp << "\n";
            }
        }
    }
    GumboVector* child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++) {
        search_for_img(static_cast<GumboNode*>(child->data[i]));
    }
}

void search_for_a_name(GumboNode* node) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return;
    }

    if (node->v.element.tag == GUMBO_TAG_A) {
        GumboAttribute* title = gumbo_get_attribute(&node->v.element.attributes, "title");
        GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
        if (title && href) {
            std::string FileName = title->value;
            std::string LinkStr = href->value;
            if (FileName.rfind("File:") == 0) {
                writeCsv << "File,"  << FileName << "," << LinkStr << "\n";
            }
            else
            {
                writeCsv << "Misc," << FileName  << "," << LinkStr << "\n";
            }
        }
    }
    GumboVector* child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++) {
        search_for_a_name(static_cast<GumboNode*>(child->data[i]));
    }
}

std::vector<std::string> get_img_links() {
    std::string line;
    std::vector<std::string> img_links;
    while (std::getline(readLink, line)) {
        std::istringstream ISS;
        img_links.push_back(line);
    }

    /*for (int i = 0; i < img_links.size(); i++) {
        std::cout << img_links[i] << "\n";
    }*/
    return img_links;
}

void downloads_images(std::string url, std::string file_name) {
    std::filesystem::create_directory("Character Genshin Card Image");
    file_name = "Character Genshin Card Image\\" + file_name;
    CURL* curl;
    FILE* f;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        f= fopen(file_name.c_str(), "wb");
        if (f) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl,CURLOPT_WRITEDATA, f);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            fclose(f);
        }
        else {
            std::cout << (stderr, "Can't create file !");
            exit(0);
        }
    }
    else {
        std::cout << (stderr, "Can't initialize cUrl !");
    }
}

int main() {
    if (!checkInet()) {
        std::cout << "Failed to connect to internet, this program need internet to working properly !";
        system("PAUSE");
        exit(-1);
    }
    else {
        std::cout << "OK";
        //Get character list from /wiki/Category:Character_Cards
        std::vector<std::string> img_vecs, temp;
        std::string page_content = extract_html_page_category();
        GumboOutput* parsed_res = gumbo_parse(page_content.c_str());
        search_for_a_name(parsed_res->root);
        writeCsv.close();
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res);
        //Get character link based character category
        temp = extract_character_link();
        img_vecs = sanitize_vecs(temp);
        for (int i = 8; i < img_vecs.size() - 3; i++) {
            std::string page_chara_content = extract_html_page_character(img_vecs[i]);
            GumboOutput* parsed_res_chara = gumbo_parse(page_chara_content.c_str());
            search_for_img(parsed_res_chara->root);
            gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_chara);
        }
        //Don't delete it's for debugging !!
        /*std::string page_chara_content = extract_html_page_character(img_vecs[69]);
        GumboOutput* parsed_res_chara = gumbo_parse(page_chara_content.c_str());
        search_for_img(parsed_res_chara->root);
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_chara);*/
        writeLink.close();
        //Download Img
        std::vector<std::string> link_vecs;
        std::string file_name;
        link_vecs = get_img_links();
        for (int i = 0; i < link_vecs.size(); i++) {
            file_name = link_vecs[i];
            //std::cout << file_name.rfind("Character_") << file_name.rfind("Traveler_") << "\n";
            //downloads_images(link_vecs[i],file_name);
        }
        readLink.close();
        //Exit gracefully and return memory to OS
        return 0;
    }
}