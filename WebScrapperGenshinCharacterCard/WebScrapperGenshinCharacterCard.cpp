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
std::ifstream readCsv("FileName.csv");

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

void extract_character_link() {
    //std::string line;
    //int rowCount = 0;
    //int rowIdx = 0;
    //while (std::getline(readCsv, line)) {
    //    rowCount++;
    //}

    //std::vector<std::vector<std::string>> data(rowCount);
 
    //readCsv.clear();
    //readCsv.seekg(readCsv.beg);
    //while(getline(readCsv, line)) {
    //    std::stringstream SS(line);
    //    std::string value;
    //    while(getline(SS, value, ',')) {
    //        data[rowIdx].push_back(SS.str());
    //    }
    //    rowIdx++;
    //}
    //
    ////Its not printing spesific column
    //int colNum = 1;
    //for (int row = 0; row < rowCount; row++) {
    //    std::cout << data[row][colNum] << "\n";
    //    writeImgLink << data[row][colNum] << "\n";
    //}
    //readCsv.close();

    std::string type, name, link;

    while (readCsv.good()) {
        std::getline(readCsv, type, ',');
        std::getline(readCsv, name, ',');
        std::getline(readCsv, link, ',');
        /*std::cout << type << "\n";
        std::cout << name << "\n";*/
        std::cout << link << "\n";
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
            //std::cout << title->value << " " << href->value << "\n";
            std::string FileName = title->value;
            std::string LinkStr = href->value;
            if (FileName.rfind("File:") == 0) {
                writeCsv << "\"File\"," << "\"" << FileName << "\"" << "," << "\"" << LinkStr << "\"" << "\n";
            }
            else
            {
                writeCsv << "\"Misc\"," << "\"" << FileName << "\"" << "," << "\"" << LinkStr << "\"" << "\n";
            }
        }
    }
    GumboVector* child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++) {
        search_for_a_name(static_cast<GumboNode*>(child->data[i]));
    }
}


int main() {
    std::string page_content = extract_html_page_category();
    GumboOutput* parsed_res = gumbo_parse(page_content.c_str());
    writeCsv << "Type,FileName,InnerLink" << "\n";
    search_for_a_name(parsed_res->root);
    writeCsv.close();
    gumbo_destroy_output(&kGumboDefaultOptions, parsed_res);
    extract_character_link();
    return 0;
}