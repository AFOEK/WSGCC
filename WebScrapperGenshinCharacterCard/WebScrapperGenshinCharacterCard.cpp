#include <stdio.h>
#include <string.h>
#include "cpr/cpr.h"
#include "gumbo.h"
#include <fstream>
#include <iostream>

//This is example static links assets of character card image:
//https://static.wikia.nocookie.net/gensin-impact/images/f/f8/Character_Albedo_Card.png
//This is example static links assets of character wish image:
//https://static.wikia.nocookie.net/gensin-impact/images/5/51/Character_Albedo_Full_Wish.png
//Tutorial link:
//https://www.webscrapingapi.com/c-web-scraping/

using namespace std;
using namespace cpr;

ofstream writeCsv("FileName.csv");

string extract_html_page() {
    Url url = Url{"https://genshin-impact.fandom.com/wiki/Category:Character_Cards"};
    Response res = Get(url);
    return res.text;
}

void check_csv_exist(const string &filename) {
    ifstream fin(filename);
    if (fin.fail()) {
        return;
    }
    else {
        remove("FileName.csv");
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
            cout << title->value << " " << href->value << "\n";
            string FileName = title->value;
            string LinkStr = href->value;
            string Chara_name;
            if (FileName.rfind("File:") == 0) {
                if (FileName.rfind("Traveler") == 0)
                {
                    Chara_name = FileName.substr(0);
                }
                else {
                    Chara_name = FileName.substr(15);
                }
                writeCsv << "File," << FileName << "," << LinkStr << "," << Chara_name << "\n";
            }
            else
            {
                writeCsv << "Misc," << FileName << "," << LinkStr << "," << NULL << "\n";
            }
        }
    }
    GumboVector* child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++) {
        search_for_a_name(static_cast<GumboNode*>(child->data[i]));
    }
}


int main() {
    check_csv_exist("FileName.csv");
    string page_content = extract_html_page();
    GumboOutput* parsed_res = gumbo_parse(page_content.c_str());
    writeCsv << "Type,FileName,InnerLink" << "\n";
    search_for_a_name(parsed_res->root);
    writeCsv.close();
    gumbo_destroy_output(&kGumboDefaultOptions, parsed_res);
    return 0;
}