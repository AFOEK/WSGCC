// Suppress warning for fopen, deprecated, and confused min max (depecration warning: Security SEVERE)
#define _CRT_SECURE_NO_WARNINGS
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define NOMINMAX
// Include library STL
#include <cstdio>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstdlib>
#include <cmath>
#include <functional>
#include <unordered_set>
// Include library external
#include <curl/curl.h>
#include <curl/easy.h>
#include <cpr/cpr.h>
#include <gumbo.h>
#include <indicators/progress_bar.hpp>
#include <indicators/cursor_control.hpp>
#include <argparse/argparse.hpp>

// Preprocessor
#if defined(__linux__)
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#include <Wininet.h>
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Wldap32.lib")
#pragma comment(lib, "Crypt32.lib")
#elif defined(__APPLE__) && defined(__MACH__)
#include <sys/sysctl.h>
#elif defined(__ANDROID__)
// Android library
#endif

// This is an example static links assets of character card image:
// https://static.wikia.nocookie.net/gensin-impact/images/f/f8/Character_Albedo_Card.png/revision/latest
// This is an example static links assets of character wish image:
// https://static.wikia.nocookie.net/gensin-impact/images/5/51/Character_Albedo_Full_Wish.png/revision/latest
// This is an example static links assets of character constellation images :
// https://static.wikia.nocookie.net/gensin-impact/images/8/84/Trifolium_Shape.png/revision/latest
// This is an example static links assets of character introduction images:
// https://static.wikia.nocookie.net/gensin-impact/images/b/b1/Character_Albedo_Introduction.png/revision/latest
// This is an example static links assets of character namecard images:
// https://static.wikia.nocookie.net/gensin-impact/images/5/55/Item_Albedo_Sun_Blossom.png/revision/latest
// This is an example static links assets of battle pass namecard images:
// https://static.wikia.nocookie.net/gensin-impact/images/a/ae/Item_Travel_Notes_Catch_the_Wind.png/revision/latest
// This is an example static links assets of TCG character dynamics card images:
// https://static.wikia.nocookie.net/gensin-impact/images/c/c2/Ganyu_Dynamic_Skin.ogv/revision/latest
// This is an example static links assets of version images:
// https://static.wikia.nocookie.net/gensin-impact/images/6/61/Splashscreen_Welcome_To_Teyvat.png/revision/latest
// This is an example static links assets of TCG character card images:
// https://static.wikia.nocookie.net/gensin-impact/images/8/87/Ganyu_Character_Card.png/revision/latest
// This is an example character wiki page link:
// https://genshin-impact.fandom.com/wiki/Albedo
// This is a version wiki page link:
// https://genshin-impact.fandom.com/wiki/Version
// Tutorial link:
// https://www.webscrapingapi.com/c-web-scraping/

int nb_bar;
int opt = 99;
bool verbose = true;
std::string app_version = "v0.0.9", root_url = "https://genshin-impact.fandom.com", dir;
double last_progress, progress_bar_adv;
std::ofstream writeChara("FileName.gsct");
std::ofstream writeImgLink("FileImg.gsct");
std::ofstream writeLink("ImgLink.gsct");
std::ofstream writeConst("FileConst.gsct");
std::ofstream writeIntro("FileIntro.gsct");
std::ofstream writeNameCardChara("FileNamecardChara.gsct");
std::ofstream writeNameCardBP("FileNamecardBP.gsct");
std::ofstream writeDynamicsTGC("FileTGCDynamic.gsct");
std::ofstream writeVer("Version.gsct");
std::ofstream writeTGC("FileTGC.gsct");
std::ifstream readChara("FileName.gsct");
std::ifstream readLink("ImgLink.gsct");
std::ifstream readConst("FileConst.gsct");
std::ifstream readIntro("FileIntro.gsct");
std::ifstream readNameCardChara("FileNamecardChara.gsct");
std::ifstream readNameCardBP("FileNamecardBP.gsct");
std::ifstream readVer("Version.gsct");
std::ifstream readTGC("FileTGC.gsct");
std::ifstream readDynamicsTGC("FileTGCDynamic.gsct");

#if defined(_WIN32)
bool checkInet()
{
    bool con = InternetCheckConnectionA("https://www.google.com", FLAG_ICC_FORCE_CONNECTION, 0);
    return con;
}
#elif defined(__linux__) && defined(__unix__) && !defined(__ANDROID__)
bool checkInet()
{
    FILE *output;
    if (!(output = popen("/sbin/route -n | grep -c '^0\\.0\\.0\\.0'", "r")))
    {
        return false;
    }
    unsigned int i;
    fscanf(output, "%u", &i);
    if (i == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
    pclose(output);
}
#elif (defined(__APPLE__) && defined(__MACH__)) || defined(__ANDROID__)

size_t write_data_silent(char *ptr, size_t size, size_t buff, void *userp)
{
    return size * buff;
}

bool checkInet()
{
    /*Fall back using cUrl, this is more cost effective method other then forcing to use OS command
    to check availablity internet on devices.*/
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://www.google.com");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_silent);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (res == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
#endif

size_t write_data(void *ptr, size_t size, size_t buff, FILE *stream)
{
    size_t written;
    written = fwrite(ptr, size, buff, stream);
    return written;
}

std::string extract_html_page_category()
{
    cpr::Url url_category = cpr::Url{root_url + "/wiki/Category:Character_Cards"};
    cpr::Response res = Get(url_category);
    return res.text;
}

std::string extract_html_page_category_TGC()
{
    cpr::Url url_category = cpr::Url{root_url + "/wiki/Category:Genius_Invokation_TCG_Character_Cards"};
    cpr::Response res = Get(url_category);
    return res.text;
}

std::string extract_html_page_category_chara_intro()
{
    cpr::Url url_category = cpr::Url{root_url + "/wiki/Category:Character_Introduction_Cards"};
    cpr::Response res = Get(url_category);
    return res.text;
}

std::string extract_html_page_category_namecard()
{
    cpr::Url url_category = cpr::Url{root_url + "/wiki/Category:Character_Namecards"};
    cpr::Response res = Get(url_category);
    return res.text;
}

std::string extract_html_page_category_namecard_bp()
{
    cpr::Url url_category = cpr::Url{ root_url + "/wiki/Category:Battle_Pass_Namecards" };
    cpr::Response res = Get(url_category);
    return res.text;
}

std::string extract_html_page_category_const()
{
    cpr::Url url_category = cpr::Url{root_url + "/wiki/Category:Constellation_Overviews"};
    cpr::Response res = Get(url_category);
    return res.text;
}

std::string extract_html_page_version()
{
    cpr::Url url_category = cpr::Url{root_url + "/wiki/Version"};
    cpr::Response res = Get(url_category);
    return res.text;
}

std::string extract_html_page_character(std::string character_wiki_link)
{
    cpr::Url url_character = cpr::Url{root_url + character_wiki_link};
    cpr::Response res = Get(url_character);
    return res.text;
}

std::vector<std::string> sanitize_vecs(std::vector<std::string> vecs)
{
    auto isEmptyOrBlank = [](const std::string &s)
    {
        return s.find_first_not_of("\t") == std::string::npos;
    };
    vecs.erase(std::remove_if(vecs.begin(), vecs.end(), isEmptyOrBlank), vecs.end());
    return vecs;
}

void search_for_img(GumboNode *node, int imgType)
{
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        return;
    }

    if (node->v.element.tag == GUMBO_TAG_IMG)
    {
        GumboAttribute *imgLink = gumbo_get_attribute(&node->v.element.attributes, "src");
        if (imgLink)
        {
            std::string LinkImg;
            std::string LinkImgTmp = imgLink->value;

            switch (imgType)
            {
            case 1:
                if ((LinkImgTmp.rfind("_Card.") != 18446744073709551615UL) && !(LinkImgTmp.rfind("Introduction") != 18446744073709551615UL))
                {
                    LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                    writeLink << LinkImgTmp << "\n";
                    std::cout << LinkImgTmp << "\n";
                    /*std::cout << LinkImgTmp.rfind("_Card.") << "->" << LinkImgTmp << "\n";*/
                }
                break;
            case 2:
                if (LinkImgTmp.rfind("_Wish") != 18446744073709551615UL)
                {
                    LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                    writeLink << LinkImgTmp << "\n";
                    std::cout << LinkImgTmp << "\n";
                    /*std::cout << LinkImgTmp.rfind("_Wish") << "->" << LinkImgTmp << "\n";*/
                }
                break;
            case 3:
                if (LinkImgTmp.rfind("_Shape") != 18446744073709551615UL)
                {
                    LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                    writeLink << LinkImgTmp << "\n";
                    std::cout << LinkImgTmp << "\n";
                    /*std::cout << LinkImgTmp.rfind("_Shape") << "->" << LinkImgTmp << "\n";*/
                }
                break;
            case 4:
                if (LinkImgTmp.rfind("_Introduction_Card.png") != 18446744073709551615UL)
                {
                    LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                    writeLink << LinkImgTmp << "\n";
                    std::cout << LinkImgTmp << "\n";
                    /*std::cout << LinkImgTmp.rfind("_Introduction_Card.png") << "->" << LinkImgTmp << "\n";*/
                }
                break;
            case 5:
                if (LinkImgTmp.rfind("Namecard_Background_") != 18446744073709551615UL)
                {
                    LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                    writeLink << LinkImgTmp << "\n";
                    std::cout << LinkImgTmp << "\n";
                    /*std::cout << LinkImgTmp.rfind("Namecard_Background_") << "->" << LinkImgTmp << "\n";*/
                }
                break;
            /*Case 6: already reserved for Version Images*/
            case 7:
                if (LinkImgTmp.rfind("_Character_Card") != 18446744073709551615UL)
                {
                    LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                    writeLink << LinkImgTmp << "\n";
                    std::cout << LinkImgTmp << "\n";
                    /*std::cout << LinkImgTmp.rfind("_Character_Card") << "->" << LinkImgTmp << "\n";*/
                }
                break;
            case 8:
                if (LinkImgTmp.rfind("Namecard_Background_Travel_Notes") != 18446744073709551615UL)
                {
                    LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                    writeLink << LinkImgTmp << "\n";
                    std::cout << LinkImgTmp << "\n";
                    /*std::cout << LinkImgTmp.rfind("Namecard_Background_Travel_Notes") << "->" << LinkImgTmp << "\n";*/
                }
                break;
            }
        }
    }
    GumboVector *child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++)
    {
        search_for_img(static_cast<GumboNode *>(child->data[i]), imgType);
    }
}

void search_for_video(GumboNode *node){
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        return;
    }
    
    if(node->v.element.tag == GUMBO_TAG_A){
        GumboAttribute *VidLink = gumbo_get_attribute(&node->v.element.attributes, "href");
        if (VidLink) {
            std::string LinkVid;
            std::string LinkVidTmp = VidLink->value;
            if (LinkVidTmp.rfind("_Dynamic_Skin.gif/") != 4294967295UL)
            {
                LinkVidTmp.erase(LinkVidTmp.end() - 18, LinkVidTmp.end());
                writeLink << LinkVidTmp << "\n";
                std::cout << LinkVidTmp << "\n";
                /*std::cout << LinkVidTmp.rfind("_Dynamic_Skin.gif/") << "->" << LinkVidTmp << "\n";*/
            }
        }
    }
    GumboVector *child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++)
    {
        search_for_video(static_cast<GumboNode *>(child->data[i]));
    }
}

void search_for_a_name(GumboNode *node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        return;
    }

    if (node->v.element.tag == GUMBO_TAG_A)
    {
        GumboAttribute *title = gumbo_get_attribute(&node->v.element.attributes, "title");
        GumboAttribute *href = gumbo_get_attribute(&node->v.element.attributes, "href");
        if (title && href)
        {
            std::string FileName = title->value;
            std::string LinkStr = href->value;
            if (FileName.rfind("File:") == 0)
            {
                writeChara << LinkStr << "\n";
            }
        }
    }
    GumboVector *child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++)
    {
        search_for_a_name(static_cast<GumboNode *>(child->data[i]));
    }
}

void search_for_a_const(GumboNode *node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        return;
    }

    if (node->v.element.tag == GUMBO_TAG_A)
    {
        GumboAttribute *classes = gumbo_get_attribute(&node->v.element.attributes, "class");
        GumboAttribute *href = gumbo_get_attribute(&node->v.element.attributes, "href");
        if (classes && href)
        {
            std::string ClassName = classes->value;
            std::string LinkStr = href->value;
            if (ClassName.rfind("category-page__member-link") == 0)
            {
                writeConst << LinkStr << "\n";
            }
        }
    }
    GumboVector *child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++)
    {
        search_for_a_const(static_cast<GumboNode *>(child->data[i]));
    }
}

void search_for_a_namecard_chara(GumboNode *node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        return;
    }

    if (node->v.element.tag == GUMBO_TAG_A)
    {
        GumboAttribute *classes = gumbo_get_attribute(&node->v.element.attributes, "class");
        GumboAttribute *href = gumbo_get_attribute(&node->v.element.attributes, "href");
        if (classes && href)
        {
            std::string ClassName = classes->value;
            std::string LinkStr = href->value;
            if (ClassName.rfind("category-page__member-link") == 0)
            {
                writeNameCardChara << LinkStr << "\n";
            }
        }
    }
    GumboVector *child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++)
    {
        search_for_a_namecard_chara(static_cast<GumboNode *>(child->data[i]));
    }
}

void search_for_a_namecard_bp(GumboNode* node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        return;
    }

    if (node->v.element.tag == GUMBO_TAG_A)
    {
        GumboAttribute* classes = gumbo_get_attribute(&node->v.element.attributes, "class");
        GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
        if (classes && href)
        {
            std::string ClassName = classes->value;
            std::string LinkStr = href->value;
            if (ClassName.rfind("category-page__member-link") == 0)
            {
                writeNameCardBP << LinkStr << "\n";
            }
        }
    }
    GumboVector* child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++)
    {
        search_for_a_namecard_bp(static_cast<GumboNode*>(child->data[i]));
    }
}

void search_for_a_TGC(GumboNode *node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        return;
    }

    if (node->v.element.tag == GUMBO_TAG_A)
    {
        GumboAttribute *classes = gumbo_get_attribute(&node->v.element.attributes, "class");
        GumboAttribute *href = gumbo_get_attribute(&node->v.element.attributes, "href");
        if (classes && href)
        {
            std::string ClassName = classes->value;
            std::string LinkStr = href->value;
            if (ClassName.rfind("category-page__member-link") == 0)
            {
                writeTGC << LinkStr << "\n";
            }
        }
    }
    GumboVector *child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++)
    {
        search_for_a_TGC(static_cast<GumboNode *>(child->data[i]));
    }
}

void search_for_a_intro(GumboNode *node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        return;
    }

    if (node->v.element.tag == GUMBO_TAG_A)
    {
        GumboAttribute *classes = gumbo_get_attribute(&node->v.element.attributes, "class");
        GumboAttribute *href = gumbo_get_attribute(&node->v.element.attributes, "href");
        if (classes && href)
        {
            std::string ClassName = classes->value;
            std::string LinkStr = href->value;
            if (ClassName.rfind("category-page__member-link") == 0)
            {
                writeIntro << LinkStr << "\n";
            }
        }
    }
    GumboVector *child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++)
    {
        search_for_a_intro(static_cast<GumboNode *>(child->data[i]));
    }
}

void search_for_img_version(GumboNode *node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        return;
    }

    if (node->v.element.tag == GUMBO_TAG_A)
    {
        GumboAttribute *classA = gumbo_get_attribute(&node->v.element.attributes, "class");
        GumboAttribute *href = gumbo_get_attribute(&node->v.element.attributes, "href");
        if (classA && href)
        {
            std::string ClassAtr = classA->value;
            std::string LinkStr = href->value;
            if (LinkStr.rfind("/Splashscreen_") != 18446744073709551615UL)
            {
                writeVer << LinkStr << "\n";
            }
        }
    }
    GumboVector *child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++)
    {
        search_for_img_version(static_cast<GumboNode *>(child->data[i]));
    }
}

std::vector<std::string> extract_img_links(int opt)
{
    std::string line;
    std::vector<std::string> img_links;
    return img_links;
}

std::vector<std::string> get_img_links()
{
    std::string line;
    std::vector<std::string> img_links;
    while (std::getline(readLink, line))
    {
        std::istringstream ISS;
        img_links.push_back(line);
    }
    readLink.close();
    return img_links;
}

std::vector<std::string> extract_character_chara_link()
{
    std::string line;
    std::vector<std::string> img_links;
    while (std::getline(readChara, line))
    {
        std::istringstream ISS;
        img_links.push_back(line);
    }
    readChara.close();
    return img_links;
}

std::vector<std::string> extract_character_const_link()
{
    std::string line;
    std::vector<std::string> img_links;
    while (std::getline(readConst, line))
    {
        std::istringstream ISS;
        img_links.push_back(line);
    }
    readConst.close();
    return img_links;
}

std::vector<std::string> extract_character_tgc_link()
{
    std::string line;
    std::vector<std::string> img_links;
    while (std::getline(readTGC, line))
    {
        std::istringstream ISS;
        img_links.push_back(line);
    }
    readTGC.close();
    return img_links;
}

std::vector<std::string> extract_battle_pass_namecard_link()
{
    std::string line;
    std::vector<std::string> img_links;
    while (std::getline(readNameCardBP, line))
    {
        std::istringstream ISS;
        img_links.push_back(line);
    }
    readNameCardBP.close();
    return img_links;
}


std::vector<std::string> extract_version_link()
{
    std::string line;
    std::vector<std::string> img_links;
    while (std::getline(readVer, line))
    {
        std::istringstream ISS;
        img_links.push_back(line);
    }
    readVer.close();
    return img_links;
}

std::vector<std::string> extract_character_intro_link()
{
    std::string line;
    std::vector<std::string> img_links;
    while (std::getline(readIntro, line))
    {
        std::istringstream ISS;
        img_links.push_back(line);
    }
    readIntro.close();
    return img_links;
}

std::vector<std::string> extract_character_namecard_chara_link()
{
    std::string line;
    std::vector<std::string> img_links;
    while (std::getline(readNameCardChara, line))
    {
        std::istringstream ISS;
        img_links.push_back(line);
    }
    readNameCardChara.close();
    return img_links;
}

std::vector<std::string> extract_character_namecard_bp_link()
{
    std::string line;
    std::vector<std::string> img_links;
    while (std::getline(readNameCardBP, line))
    {
        std::istringstream ISS;
        img_links.push_back(line);
    }
    readNameCardBP.close();
    return img_links;
}

int progress_bar(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    indicators::ProgressBar *prog_bar = static_cast<indicators::ProgressBar *>(clientp);

    if (prog_bar->is_completed())
    {
        ;
    }
    else if (dltotal == 0)
    {
        prog_bar->set_progress(0);
    }
    else
    {
        int percentage = static_cast<float>(dlnow) / static_cast<float>(dltotal);
    }
    return 0;
}

int download_progress_default_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    return CURL_PROGRESSFUNC_CONTINUE;
}

void close_all(bool verbose)
{
    writeChara.close();
    writeImgLink.close();
    writeLink.close();
    writeConst.close();
    writeIntro.close();
    writeNameCardChara.close();
    writeNameCardBP.close();
    writeDynamicsTGC.close();
    writeVer.close();
    writeTGC.close();
    readChara.close();
    readLink.close();
    readConst.close();
    readIntro.close();
    readNameCardChara.close();
    readNameCardBP.close();
    readVer.close();
    readTGC.close();
    readDynamicsTGC.close();
#if defined(__ANDROID__)
    if(verbose == false){
        for (auto const &entry : std::__fs::filesystem::directory_iterator{std::__fs::filesystem::current_path().string()})
        {
            if (entry.path().extension().string() == ".gsct")
            {
                std::__fs::filesystem::remove(entry.path());
            }
        }
    }
#else
    if(verbose == false){
        for (auto const &entry : std::filesystem::directory_iterator{std::filesystem::current_path().string()})
        {
            if (entry.path().extension().string() == ".gsct")
            {
                std::filesystem::remove(entry.path());
            }
        }
    }
#endif
}

void downloads_images(std::string url, std::string file_name)
{
    indicators::show_console_cursor(false);
    indicators::ProgressBar prog_bar{
        indicators::option::BarWidth{65},
        indicators::option::Start{" ["},
        indicators::option::Fill{"█"},
        indicators::option::Lead{"█"},
        indicators::option::Remainder{"-"},
        indicators::option::End{"]"},
        indicators::option::PrefixText{file_name},
        indicators::option::ShowElapsedTime{true},
        indicators::option::ShowRemainingTime{true}};
    CURL *curl;
    FILE *f;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        f = fopen(file_name.c_str(), "wb");
        if (f)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_bar);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, static_cast<void *>(&prog_bar));
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            fclose(f);
        }
        else
        {
            std::cout << (stderr, "Can't create file !");
            close_all(verbose);
            exit(-1);
        }
    }
    else
    {
        std::cout << (stderr, "Can't initialize cUrl !");
        close_all(verbose);
        exit(-1);
    }
    indicators::show_console_cursor(true);
}

void create_download_folder(int opt){
    switch (opt)
    {
    case 1:
        #if defined(_WIN32)
            dir = "Character Genshin Card Image\\";
        #else
            dir = "Character Genshin Card Image/";
        #endif
        break;
    case 2:
        #if defined(_WIN32)
            dir = "Character Genshin Wish Image\\";
        #else
            dir = "Character Genshin Wish Image/";
        #endif
        break;
    case 3:
        #if defined(_WIN32)
            dir = "Character Genshin Constellation Image\\";
        #else
            dir = "Character Genshin Constellation Image/";
        #endif
        break;
    case 4:
        #if defined(_WIN32)
            dir = "Character Genshin Introduction Card Image\\";
        #else
            dir = "Character Genshin Introduction Card Image/";
        #endif
        break;
    case 5:
        #if defined(_WIN32)
            dir = "Character Genshin Namecard Background Image\\";
        #else
            dir = "Character Genshin Namecard Background Image/";
        #endif
        break;
    case 6:
        #if defined(_WIN32)
            dir = "Genshin Version Image\\";
        #else
            dir = "Genshin Version Image/";
        #endif
        break;
    case 7:
        #if defined(_WIN32)
            dir = "Genshin TGC Character Card Image\\";
        #else
            dir = "Genshin TGC Character Card Image/";
        #endif
        break;
    case 8:
        #if defined(_WIN32)
            dir = "Genshin BP Namecard Image\\";
        #else
            dir = "Genshin BP Namecard Image/";
        #endif
        break;
    case 9:
        #if defined(_WIN32)
            dir = "Genshin TGC Dynamics Character Card\\";
        #else
            dir = "Genshin TGC Dynamics Character Card/";
        #endif
        break;
    default:
        #if defined(_WIN32)
            dir = "Character Genshin Card Image\\";
        #else
            dir = "Character Genshin Card Image/";
        #endif
        break;
    }

    #if defined(__ANDROID__)
        if (std::__fs::filesystem::is_directory(dir))
        {
            if (!std::__fs::filesystem::is_empty(dir))
            {
                for (const auto &files : std::__fs::filesystem::directory_iterator(dir))
                {
                    std::cout << "Clearing existing file\n";
                    std::__fs::filesystem::remove_all(files.path());
                }
            }
        }
        else
        {
            std::__fs::filesystem::create_directory(dir);
            std::cout << "Creating folder\n";
        }
        #else
        if (std::filesystem::is_directory(dir))
        {
            if (!std::filesystem::is_empty(dir))
            {
                for (const auto &files : std::filesystem::directory_iterator(dir))
                {
                    std::cout << "Clearing existing file\n";
                    std::filesystem::remove_all(files.path());
                }
            }
        }
        else
        {
            std::filesystem::create_directory(dir);
            std::cout << "Creating folder\n";
        #if defined(__linux__) && defined(__unix__)
            std::filesystem::permissions(dir, std::filesystem::perms::owner_all | std::filesystem::perms::group_read, std::filesystem::perm_options::add);
        #endif
        }
    #endif
}

int main(int argc, char **argv)
{
    if (!checkInet())
    {
        std::cout << "Failed to connect to internet, this program need internet to working properly !"<< "\n";
        std::cin.ignore();
        std::cin.get();
        close_all(verbose);
        exit(-1);
    }
    else
    {   
        if(argc != 1){
            argparse::ArgumentParser program(argv[0]);
            program.add_argument("--verbose","-vv").help("It will not delete all download log").default_value(false).implicit_value(true);
            program.add_argument("--characard", "-cc").help("Downloads all Character card images").default_value(false).implicit_value(true);
            program.add_argument("--wishimg", "-wi").help("Downloads all Character wish images").default_value(false).implicit_value(true);
            program.add_argument("--constel","-co").help("Downloads all Character Constellation images").default_value(false).implicit_value(true);
            program.add_argument("--charanamecard","-cn").help("Downloads all Character Namecard images").default_value(false).implicit_value(true);
            program.add_argument("--introcard","-ic").help("Downloads all Character Introduction images").default_value(false).implicit_value(true);
            program.add_argument("--verimg","-vi").help("Downloads all Version Images").default_value(false).implicit_value(true);
            program.add_argument("--tcgimg","-ti").help("Downloads all TGC Character Images").default_value(false).implicit_value(true);
            program.add_argument("--bpcard","-bc").help("Downloads all Battle Pass namecard images").default_value(false).implicit_value(true);
            program.add_argument("--tcgdyn", "-td").help("Downloads all TGC Character Images").default_value(false).implicit_value(true);

            if (program["--characard"] == true || program["--cc"] == true) {
                opt = 1;
            }
            else if (program["--wishimg"] == true || program["--wi"] == true) {
                opt = 2;
            }
            else if (program["--constel"] == true || program["--co"] == true) {
                opt = 3;
            }
            else if (program["--charanamecard"] == true || program["--cn"] == true) {
                opt = 5;
            }
            else if (program["--introcard"] == true || program["--ic"] == true) {
                opt = 4;
            }
            else if (program["--verimg"] == true || program["--vi"] == true) {
                opt = 6;
            }
            else if (program["--tcgimg"] == true || program["--ti"] == true) {
                opt = 7;
            }
            else if (program["--bpcard"] == true || program["--bc"] == true) {
                opt = 8;
            }
            else if (program["--tcgdyn"] == true || program["--td"] == true) {
                opt = 9;
            }

            try{
                program.parse_args(argc, argv);
            }catch(const std::runtime_error& err){
    
                std::cerr << err.what() << std::endl;
                std::cerr << program;
                std::cin.ignore();
                std::cin.get();
                close_all(verbose);
                std::exit(-1);
            }
        } else {

            std::cout << "No command line args supplied\n";
        }
        // Init variables
        std::cout << "Getting character list from wiki\n";
        std::vector<std::string> const_vecs, img_vecs, card_vecs, ver_vecs, tgc_vecs, card_bp_vecs, temp_chara, temp_const, intro_vecs, temp_vecs, temp_intro, temp_card_chara, temp_ver, temp_tgc, temp_card, temp_card_bp_vecs;
        // Get character list from /wiki/Category:Character_Cards
        std::string page_content_chara = extract_html_page_category();
        GumboOutput *parsed_res_chara = gumbo_parse(page_content_chara.c_str());
        search_for_a_name(parsed_res_chara->root);
        writeChara.close();
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_chara);
        // Get character constellation list from /wiki/Category:Constellation_Overviews
        std::cout << "Getting character constellation list from wiki\n";
        std::string page_content_const = extract_html_page_category_const();
        GumboOutput *parsed_res_const = gumbo_parse(page_content_const.c_str());
        search_for_a_const(parsed_res_const->root);
        writeConst.close();
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_const);
        // Get character introduction banner list from /wiki/Category:Character_Introduction_Cards
        std::cout << "Getting character introduction list from wiki\n";
        std::string page_content_intro = extract_html_page_category_chara_intro();
        GumboOutput *parsed_res_intro = gumbo_parse(page_content_intro.c_str());
        search_for_a_intro(parsed_res_intro->root);
        writeIntro.close();
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_intro);
        // Get character namecards list from /wiki/Category:Character_Namecards
        std::cout << "Getting character namecard list from wiki\n";
        std::string page_content_namecard = extract_html_page_category_namecard();
        GumboOutput *parsed_res_namecard = gumbo_parse(page_content_namecard.c_str());
        search_for_a_namecard_chara(parsed_res_namecard->root);
        writeNameCardChara.close();
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_namecard);
        // Get version images from /wiki/Version
        std::cout << "Getting version list from wiki\n";
        std::string page_content_version = extract_html_page_version();
        GumboOutput *parsed_res_version = gumbo_parse(page_content_version.c_str());
        search_for_img_version(parsed_res_version->root);
        writeVer.close();
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_version);
        // Get TGC character card list from /wiki/Category:Genius_Invokation_TCG_Character_Cards
        std::cout << "Getting TCG card list from wiki\n";
        std::string page_content_TGC = extract_html_page_category_TGC();
        GumboOutput *parsed_res_TGC = gumbo_parse(page_content_TGC.c_str());
        search_for_a_TGC(parsed_res_TGC->root);
        writeTGC.close();
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_TGC);
        // Get TGC character card list from /wiki/Category:Genius_Invokation_TCG_Character_Cards
        std::cout << "Getting Battle Pass Namecard list from wiki\n";
        std::string page_content_BP_namecard = extract_html_page_category_namecard_bp();
        GumboOutput* parsed_res_BP_namecard = gumbo_parse(page_content_BP_namecard.c_str());
        search_for_a_namecard_bp(parsed_res_BP_namecard->root);
        writeNameCardBP.close();
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_BP_namecard);
        // Get character link based by character category
        temp_chara = extract_character_chara_link();
        img_vecs = sanitize_vecs(temp_chara);
        // Get constellation link based by constellation category
        temp_const = extract_character_const_link();
        const_vecs = sanitize_vecs(temp_const);
        // Get introdcution link based by introduction category
        temp_intro = extract_character_intro_link();
        intro_vecs = sanitize_vecs(temp_intro);
        // Get namecard link based by namecard category
        temp_card = extract_character_namecard_chara_link();
        card_vecs = sanitize_vecs(temp_card);
        // Get version link based by version page
        temp_ver = extract_version_link();
        ver_vecs = sanitize_vecs(temp_ver);
        // Get namecard link based by namecard category
        temp_tgc = extract_character_tgc_link();
        tgc_vecs = sanitize_vecs(temp_tgc);
        // Get namecard link based by BP namecard category
        temp_card_bp_vecs = extract_battle_pass_namecard_link();
        card_bp_vecs = sanitize_vecs(temp_card_bp_vecs);
        if(opt == 99){
            std::cout << "Getting character link image.\nWhat image do you want ?\n1. Card\n2. Wish\n3. Constellation\n4. Introduction Banner\n5. Namecard\n6. Version\n7. Character TGC Card\n8. Battle Pass Namecard\n9. Dynamics Character TGC Card\n0. Cancel\n";
            std::cin >> opt;
        }
        switch (opt)
        {
        case 1:
            create_download_folder(opt);
            for (int i = 8; i < img_vecs.size() - 3; i++)
            {
                std::string page_chara_content = extract_html_page_character(img_vecs[i]);
                GumboOutput *parsed_res_chara = gumbo_parse(page_chara_content.c_str());
                search_for_img(parsed_res_chara->root, opt);
                gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_chara);
            }
            break;
        case 2:
            create_download_folder(opt);
            for (int i = 8; i < img_vecs.size(); i++)
            {
                std::string page_chara_content = extract_html_page_character(img_vecs[i]);
                GumboOutput *parsed_res_chara = gumbo_parse(page_chara_content.c_str());
                search_for_img(parsed_res_chara->root, opt);
                gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_chara);
            }
            break;
        case 3:
            create_download_folder(opt);
            for (int i = 0; i < const_vecs.size(); i++)
            {
                std::string page_const_content = extract_html_page_character(const_vecs[i]);
                GumboOutput *parsed_res_const = gumbo_parse(page_const_content.c_str());
                search_for_img(parsed_res_const->root, opt);
                gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_const);
            }
            break;
        case 4:
            create_download_folder(opt);
            for (int i = 0; i < intro_vecs.size(); i++)
            {
                std::string page_intro_content = extract_html_page_character(intro_vecs[i]);
                GumboOutput *parsed_res_intro = gumbo_parse(page_intro_content.c_str());
                search_for_img(parsed_res_intro->root, opt);
                gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_intro);
            }
            break;
        case 5:
            create_download_folder(opt);
            for (int i = 0; i < card_vecs.size(); i++)
            {
                std::string page_card_content = extract_html_page_character(card_vecs[i]);
                GumboOutput *parsed_res_card = gumbo_parse(page_card_content.c_str());
                search_for_img(parsed_res_card->root, opt);
                gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_card);
            }
            break;
        case 6:
            create_download_folder(opt);
            break;
        case 7:
            create_download_folder(opt);
            for (int i = 2; i < tgc_vecs.size(); i++)
            {
                std::string page_tgc_content = extract_html_page_character(tgc_vecs[i]);
                GumboOutput *parsed_res_tgc = gumbo_parse(page_tgc_content.c_str());
                search_for_img(parsed_res_tgc->root, opt);
                gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_tgc);
            }
            break;
        case 8:
            create_download_folder(opt);
            for (int i = 2; i < card_bp_vecs.size(); i++)
            {
                std::string page_bp_content = extract_html_page_character(card_bp_vecs[i]);
                GumboOutput* parsed_res_bp = gumbo_parse(page_bp_content.c_str());
                search_for_img(parsed_res_bp->root, opt);
                gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_bp);
            }
            break;
        case 9:
            create_download_folder(opt);
            for (int i = 2; i < tgc_vecs.size(); i++)
            {
                std::string page_tgc_content = extract_html_page_character(tgc_vecs[i]);
                GumboOutput *parsed_res_tgc_dyn = gumbo_parse(page_tgc_content.c_str());
                search_for_video(parsed_res_tgc_dyn->root);
                gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_tgc_dyn);
            }
            break;
        case 0:

            std::cout << "Bye !\n";
            std::cin.ignore();
            std::cin.get();
            close_all(verbose);
            exit(-1);
        default:
            create_download_folder(opt);
            for (int i = 0; i < img_vecs.size(); i++)
            {
                std::string page_chara_content = extract_html_page_character(img_vecs[i]);
                GumboOutput *parsed_res_chara = gumbo_parse(page_chara_content.c_str());
                search_for_img(parsed_res_chara->root, 1);
                gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_chara);
            }
            break;
        }
        // Don't delete it's for debugging !!
        /*std::string page_chara_content = extract_html_page_character(img_vecs[69]);
        GumboOutput* parsed_res_chara = gumbo_parse(page_chara_content.c_str());
        search_for_img(parsed_res_chara->root);
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_chara);*/
        writeLink.close();
        // Download Img
        std::cout << "Downloading Images";
        std::vector<std::string> link_vecs;
        std::string file_name;
        if (opt == 6)
        {
            for (int i = 0; i < ver_vecs.size(); i++)
            {
                std::string link_ver = ver_vecs[i];
                link_ver.erase(link_ver.size() - 18);
                file_name = ver_vecs[i];
                file_name.erase(0, 60);
                file_name.erase(file_name.size() - 34);
                downloads_images(link_ver, dir + file_name);
            }
            std::cout << "Images succesfully downloaded !";
        }
        else
        {
            link_vecs = get_img_links();
            std::sort(link_vecs.begin(), link_vecs.end());
            auto iter = std::unique(link_vecs.begin(), link_vecs.end());
            link_vecs.erase(iter, link_vecs.end());
            for (int i = 0; i < link_vecs.size(); i++)
            {   
                file_name = link_vecs[i];
                file_name.erase(0, 60);
                if (file_name.back() == '/') {
                    file_name.erase(file_name.size() - 17);
                }
                else {
                    file_name.erase(file_name.size() - 16);
                }
                downloads_images(link_vecs[i], dir + file_name);
            }
            std::cout << "Images succesfully downloaded !";
        }
        readLink.close();
        // Check if file is closed properly
        close_all(verbose);
        // Exit gracefully and return memory to OS
        return 0;
    }
}