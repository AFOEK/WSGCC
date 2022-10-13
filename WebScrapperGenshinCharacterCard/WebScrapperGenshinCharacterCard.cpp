#include <cstdio>
#include <string>
#include "cpr/cpr.h"
#include "gumbo.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

//This is an example static links assets of character card image:
//https://static.wikia.nocookie.net/gensin-impact/images/f/f8/Character_Albedo_Card.png
//This is an example static links assets of character wish image:
//https://static.wikia.nocookie.net/gensin-impact/images/5/51/Character_Albedo_Full_Wish.png
// This is an example character wiki page link:
// https://genshin-impact.fandom.com/wiki/Albedo
//Tutorial link:
//https://www.webscrapingapi.com/c-web-scraping/

std::string root_url = "https://genshin-impact.fandom.com/";
std::ofstream writeCsv("FileName.csv");
std::ofstream writeImgLink("FileImg.txt");
std::ofstream writeLink("ImgLink.txt");
std::ifstream readCsv("FileName.csv");
std::ifstream readImgLink("FileImg.txt");

std::string extract_html_page_category() {
    cpr::Url url_category = cpr::Url{root_url+"wiki/Category:Character_Cards"};
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

    while(getline(readCsv, line)) {
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
    return data_img;
}

std::vector<std::string> sanitize_vecs(std::vector<std::string> vecs) {
    auto isEmptyOrBlank = [](const std::string& s) {
        return s.find_first_not_of("\t") == std::string::npos;
    };
    vecs.erase(std::remove_if(vecs.begin(), vecs.end(), isEmptyOrBlank), vecs.end());
    return vecs;
}

void search_for_img_a(GumboNode* node) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return;
    }
    if (node->v.element.tag == GUMBO_TAG_IMAGE) {
        GumboAttribute* img_href = gumbo_get_attribute(&node->v.element.attributes, "src");
        if (img_href) {
            std::string img_link = img_href->value;
            if (img_link.rfind("https://static") == 0) {
                std::cout << img_href->value << "\n";
                writeLink << img_link;
            }
            else {
                throw std::invalid_argument("Empty Image Link !");
            }
        }
    }
    GumboVector* child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++) {
        search_for_img_a(static_cast<GumboNode*>(child->data[i]));
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


int main() {
    std::vector<std::string> img_vecs, temp;
    std::string page_content = extract_html_page_category();
    GumboOutput* parsed_res = gumbo_parse(page_content.c_str());
    search_for_a_name(parsed_res->root);
    writeCsv.close();
    gumbo_destroy_output(&kGumboDefaultOptions, parsed_res);
    temp = extract_character_link();
    img_vecs = sanitize_vecs(temp);
    std::cout << img_vecs[0];
    /*for (int i = 0; i < img_vecs.size(); i++) {
        std::string page_chara_content = extract_html_page_character(img_vecs[i]);
        GumboOutput* parsed_res_chara = gumbo_parse(page_chara_content.c_str());
        search_for_img_a(parsed_res_chara->root);
    }*/
    writeLink.close();
    return 0;
}