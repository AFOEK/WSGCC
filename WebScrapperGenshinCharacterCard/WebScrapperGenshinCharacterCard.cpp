#include <stdio.h>
#include <string.h>
#include "cpr/cpr.h"
#include "gumbo.h"
#include <fstream>
#include <iostream>

//This is static links assets of character card image:
//https://static.wikia.nocookie.net/gensin-impact/images/f/f8/Character_Albedo_Card.png
//This is static links assets of character wish image:
//https://static.wikia.nocookie.net/gensin-impact/images/5/51/Character_Albedo_Full_Wish.png
//Tutorial link:
//https://www.webscrapingapi.com/c-web-scraping/

using namespace std;
using namespace cpr;

ofstream writeCsv("FileName.csv");
string character_card_link = "https://static.wikia.nocookie.net/gensin-impact/images/f/f8/";

string extract_html_page() {
    Url url = Url{"https://genshin-impact.fandom.com/wiki/Category:Character_Cards"};
    Response res = Get(url);
    return res.text;
}

void check_csv_exist() {
    FILE* f;
    if (f = fopen("FileName.csv", "r")) {
        fclose(f);
        remove("FilenName.csv");
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
            if (FileName.rfind("File:") == 0) {
                writeCsv << "File," << FileName << "," << LinkStr << "\n";
            }
            else
            {
                writeCsv << "Misc," << FileName << "," << LinkStr << "\n";
            }
        }
    }
    GumboVector* child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++) {
        search_for_a_name(static_cast<GumboNode*>(child->data[i]));
    }
}

int main() {
    check_csv_exist();
    string page_content = extract_html_page();
    GumboOutput* parsed_res = gumbo_parse(page_content.c_str());
    writeCsv << "Type,FileName,InnerLink" << "\n";
    search_for_a_name(parsed_res->root);
    writeCsv.close();
    gumbo_destroy_output(&kGumboDefaultOptions, parsed_res);
    return 0;
}