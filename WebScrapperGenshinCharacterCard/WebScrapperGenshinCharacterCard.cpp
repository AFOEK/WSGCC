﻿// Suppress warning for fopen, deprecated, and confused min max (depecration warning: Security SEVERE)
#define _CRT_SECURE_NO_WARNINGS
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define NOMINMAX
// Include STL library
#include <cstdio>
#include <string>
#include <cstring>
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
// Include extrenal library
#include <curl/curl.h>
#include <curl/easy.h>
#include <cpr/cpr.h>
#include <gumbo.h>
#include <indicators/progress_bar.hpp>
#include <indicators/cursor_control.hpp>
#include <argparse/argparse.hpp>
#include <termcolor/termcolor.hpp>

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
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <unistd.h>
#endif

// This is an example static link assets of character card image:
// https://static.wikia.nocookie.net/gensin-impact/images/f/f8/Character_Albedo_Card.png/revision/latest
// This is an example static link assets of character wish image:
// https://static.wikia.nocookie.net/gensin-impact/images/5/51/Character_Albedo_Full_Wish.png/revision/latest
// This is an example static link assets of character constellation images :
// https://static.wikia.nocookie.net/gensin-impact/images/8/84/Trifolium_Shape.png/revision/latest
// This is an example static link assets of character introduction images:
// https://static.wikia.nocookie.net/gensin-impact/images/b/b1/Character_Albedo_Introduction.png/revision/latest
// This is an example static link assets of character namecard images:
// https://static.wikia.nocookie.net/gensin-impact/images/5/55/Item_Albedo_Sun_Blossom.png/revision/latest
// This is an example static link assets of battle pass namecard images:
// https://static.wikia.nocookie.net/gensin-impact/images/a/ae/Item_Travel_Notes_Catch_the_Wind.png/revision/latest
// This is an example static link assets of TCG character dynamics card images:
// https://static.wikia.nocookie.net/gensin-impact/images/c/c2/Ganyu_Dynamic_Skin.ogv/revision/latest
// This is an example static link assets of version images:
// https://static.wikia.nocookie.net/gensin-impact/images/6/61/Splashscreen_Welcome_To_Teyvat.png/revision/latest
// This is an example static link assets of TCG character card images:
// https://static.wikia.nocookie.net/gensin-impact/images/8/87/Ganyu_Character_Card.png/revision/latest
// This is an example static link assets of Sticker images:
// https://static.wikia.nocookie.net/gensin-impact/images/c/c9/Icon_Emoji_Paimon%27s_Paintings_04_Albedo_1.png/revision/latest
// This is an example static link assets of Version images:
// https://static.wikia.nocookie.net/gensin-impact/images/5/55/Version_2.6_Wallpaper_1.png/revision/latest
// This is an example static link assets of Vision images:
// https://static.wikia.nocookie.net/gensin-impact/images/f/fd/Vision_Liyue_Geo.png/revision/latest
// This is an example character wiki page link:
// https://genshin-impact.fandom.com/wiki/Albedo
// This is a version wiki page link:
// https://genshin-impact.fandom.com/wiki/Version/Gallery
// This is a vision wiki page link:
// https://genshin-impact.fandom.com/wiki/Vision/Gallery
// This is a Hoyolab Avatar frame
// https://genshin-impact.fandom.com/wiki/HoYoLAB/Gallery
// Tutorial link:
// https://www.webscrapingapi.com/c-web-scraping/

int nb_bar, opt = 99;
bool verbose = true;
std::string app_version = "v0.9.3", root_url = "https://genshin-impact.fandom.com", dir;
double last_progress, progress_bar_adv;

//Create write file streams
std::ofstream writeChara("FileName.gsct");
std::ofstream writeImgLink("FileImg.gsct");
std::ofstream writeLink("ImgLink.gsct");
std::ofstream writeNameCardBP("FileNamecardBP.gsct");
std::ofstream writeDynamicsTGC("FileTGCDynamic.gsct");
std::ofstream writeVer("Version.gsct");
std::ofstream writeTGC("FileTGC.gsct");
std::ofstream writeMedia("Media.gsct");
std::ofstream writeSplhScr("SplashScreen.gsct");
std::ofstream writeAvatarFrm("AvatarFrame.gsct");

//Create read file streams
std::ifstream readChara("FileName.gsct");
std::ifstream readLink("ImgLink.gsct");
std::ifstream readNameCardBP("FileNamecardBP.gsct");
std::ifstream readVer("Version.gsct");
std::ifstream readTGC("FileTGC.gsct");
std::ifstream readDynamicsTGC("FileTGCDynamic.gsct");
std::ifstream readMedia("Media.gsct");
std::ifstream readSplhScr("SplashScreen.gsct");
std::ifstream readAvatarFrm("AvatarFrame.gsct");

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
#elif ((defined(__APPLE__) && defined(__MACH__)) || defined(__ANDROID__))

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

std::string extract_html_page_category_character()
{
    cpr::Url url_category = cpr::Url{ root_url + "/wiki/Category:Playable_Characters" };
    cpr::Response res = Get(url_category);
    return res.text;
}

std::string extract_html_page_hoyolab_frames()
{
    cpr::Url url_category = cpr::Url{ root_url + "/wiki/HoYoLAB/Gallery" };
    cpr::Response res = Get(url_category);
    return res.text;
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

std::string extract_html_page_category_namecard_bp()
{
    cpr::Url url_category = cpr::Url{ root_url + "/wiki/Category:Battle_Pass_Namecards" };
    cpr::Response res = Get(url_category);
    return res.text;
}

std::string extract_html_page_version()
{
    cpr::Url url_category = cpr::Url{root_url + "/wiki/Version/Gallery"};
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
    //Remove empty vector space 
    auto isEmptyOrBlank = [](const std::string &s)
    {
        return s.find_first_not_of("\t") == std::string::npos;
    };
    vecs.erase(std::remove_if(vecs.begin(), vecs.end(), isEmptyOrBlank), vecs.end());
    //Remove duplicate values
    std::sort(vecs.begin(), vecs.end());
    vecs.erase(unique(vecs.begin(), vecs.end()), vecs.end());
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
        GumboAttribute *imgClass = gumbo_get_attribute(&node->v.element.attributes, "class");
        if (imgLink)
        {
            std::string LinkImg, LinkImgTmp = imgLink->value;

            switch (imgType)
            {
            case 1:
                if ((LinkImgTmp.rfind("_Card.png") != 18446744073709551615UL) && !(LinkImgTmp.rfind("_Introduction_Card.png") != 18446744073709551615UL))
                {   
                    size_t pos_scale_dwn, pos_smart_width;
                    pos_scale_dwn = LinkImgTmp.find("/scale-to-width-down/");
                    pos_smart_width = LinkImgTmp.find("smart/width/");
                    if (pos_scale_dwn != std::string::npos) {
                        LinkImgTmp.erase(pos_scale_dwn);
                    } else if (pos_smart_width != std::string::npos){
                        LinkImgTmp.erase(pos_smart_width);
                    }
                    //LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                    writeLink << LinkImgTmp << "\n";
                    std::cout << termcolor::cyan << LinkImgTmp << "\n" << termcolor::reset;
                    /*std::cout << LinkImgTmp.rfind("_Card.") << "->" << LinkImgTmp << "\n";*/
                }
                break;
            case 2:
                if (LinkImgTmp.rfind("_Wish.png") != 18446744073709551615UL && LinkImgTmp.rfind("_Multi_") == 18446744073709551615UL)
                {
                    size_t pos_scale_dwn, pos_smart_width;
                    pos_scale_dwn = LinkImgTmp.find("/scale-to-width-down/");
                    pos_smart_width = LinkImgTmp.find("smart/width/");
                    if (pos_scale_dwn != std::string::npos) {
                        LinkImgTmp.erase(pos_scale_dwn);
                    } else if (pos_smart_width != std::string::npos){
                        LinkImgTmp.erase(pos_smart_width);
                    }
                    //LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                    writeLink << LinkImgTmp << "\n";
                    std::cout << termcolor::cyan << LinkImgTmp << "\n" << termcolor::reset;
                    /*std::cout << LinkImgTmp.rfind("_Wish.png") << "->" << LinkImgTmp << "\n";*/
                }
                break;
            case 3:
                if (LinkImgTmp.rfind("_Shape") != 18446744073709551615UL)
                {
                    size_t pos_scale_dwn, pos_smart_width;
                    pos_scale_dwn = LinkImgTmp.find("/scale-to-width-down/");
                    pos_smart_width = LinkImgTmp.find("smart/width/");
                    if (pos_scale_dwn != std::string::npos) {
                        LinkImgTmp.erase(pos_scale_dwn);
                    } else if (pos_smart_width != std::string::npos){
                        LinkImgTmp.erase(pos_smart_width);
                    }
                    //LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                    writeLink << LinkImgTmp << "\n";
                    std::cout << termcolor::cyan << LinkImgTmp << "\n" << termcolor::reset;
                    //std::cout << LinkImgTmp.rfind("_Shape") << "->" << LinkImgTmp << "\n";
                }
                break;
            case 4:
                if (LinkImgTmp.rfind("_Introduction_Card.png") != 18446744073709551615UL)
                {
                    size_t pos_scale_dwn, pos_smart_width;
                    pos_scale_dwn = LinkImgTmp.find("/scale-to-width-down/");
                    pos_smart_width = LinkImgTmp.find("smart/width/");
                    if (pos_scale_dwn != std::string::npos) {
                        LinkImgTmp.erase(pos_scale_dwn);
                    } else if (pos_smart_width != std::string::npos){
                        LinkImgTmp.erase(pos_smart_width);
                    }
                    //LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                    writeLink << LinkImgTmp << "\n";
                    std::cout << termcolor::cyan << LinkImgTmp << "\n" << termcolor::reset;
                    /*std::cout << LinkImgTmp.rfind("_Introduction_Card.png") << "->" << LinkImgTmp << "\n";*/
                }
                break;
            case 5:
                if (LinkImgTmp.rfind("Namecard_Background_") != 18446744073709551615UL)
                {
                    size_t pos_scale_dwn, pos_smart_width;
                    pos_scale_dwn = LinkImgTmp.find("/scale-to-width-down/");
                    pos_smart_width = LinkImgTmp.find("smart/width/");
                    if (pos_scale_dwn != std::string::npos) {
                        LinkImgTmp.erase(pos_scale_dwn);
                    } else if (pos_smart_width != std::string::npos){
                        LinkImgTmp.erase(pos_smart_width);
                    }
                    //LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                    writeLink << LinkImgTmp << "\n";
                    std::cout << termcolor::cyan << LinkImgTmp << "\n" << termcolor::reset;
                    /*std::cout << LinkImgTmp.rfind("Namecard_Background_") << "->" << LinkImgTmp << "\n";*/
                }
                break;
            /*Case 6: already reserved for Version wallpaper Images*/
            case 7:
                if (LinkImgTmp.rfind("_Character_Card.png/") != 18446744073709551615UL)
                {
                    size_t pos_scale_dwn, pos_smart_width, pos_cb;
                    pos_scale_dwn = LinkImgTmp.find("/scale-to-width-down/");
                    pos_smart_width = LinkImgTmp.find("smart/width/");
					pos_cb = LinkImgTmp.find("?cb=");
                    if (pos_scale_dwn != std::string::npos) {
                        LinkImgTmp.erase(pos_scale_dwn);
                    } else if (pos_smart_width != std::string::npos){
                        LinkImgTmp.erase(pos_smart_width);
                    }
                    else if (pos_cb != std::string::npos) {
						LinkImgTmp.erase(pos_cb);
                    }
                    //LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                    writeLink << LinkImgTmp << "\n";
                    std::cout << termcolor::cyan << LinkImgTmp << "\n" << termcolor::reset;
                    /*std::cout << LinkImgTmp.rfind("_Character_Card.png/") << "->" << LinkImgTmp << "\n"*/;
                }
                break;
            case 8:
                if (LinkImgTmp.rfind("Namecard_Background_Travel_Notes") != 18446744073709551615UL)
                {
                    size_t pos_scale_dwn, pos_smart_width;
                    pos_scale_dwn = LinkImgTmp.find("/scale-to-width-down/");
                    pos_smart_width = LinkImgTmp.find("smart/width/");
                    if (pos_scale_dwn != std::string::npos) {
                        LinkImgTmp.erase(pos_scale_dwn);
                    } else if (pos_smart_width != std::string::npos){
                        LinkImgTmp.erase(pos_smart_width);
                    }
                    //LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                    writeLink << LinkImgTmp << "\n";
                    std::cout << termcolor::cyan << LinkImgTmp << "\n" << termcolor::reset;
                    /*std::cout << LinkImgTmp.rfind("Namecard_Background_Travel_Notes") << "->" << LinkImgTmp << "\n";*/
                }
                break;
            /*Case 9: already reserved for TGC Dynamics*/
            case 10:
                if (LinkImgTmp.rfind("Icon_Emoji_Paimon%27s_Paintings") != 18446744073709551615UL)
                {
                    size_t find_scale = LinkImgTmp.find("scale-to-width-down");
                    size_t find_cb = LinkImgTmp.find("?cb=");
                    if (find_scale != std::string::npos) {
                        LinkImgTmp.erase(find_scale);
                    }
                    else if (find_cb != std::string::npos) {
                        LinkImgTmp.erase(find_cb);
                    }
                    writeLink << LinkImgTmp << "\n";
                    std::cout << termcolor::cyan << LinkImgTmp << "\n" << termcolor::reset;
                    /*std::cout << LinkImgTmp.rfind("Icon_Emoji_Paimon%27s_Paintings") << "->" << LinkImgTmp << "\n";*/
                }
                break;
            /*Case 11: already reserved for Splash Screen*/
            case 12:
                if (LinkImgTmp.rfind("_Birthday_") != 18446744073709551615UL && LinkImgTmp.rfind("_Shorts.") == 18446744073709551615UL) {
                    size_t find_scale = LinkImgTmp.find("scale-to-width-down");
                    size_t find_cb = LinkImgTmp.find("?cb=");
                    if (find_scale != std::string::npos) {
                        LinkImgTmp.erase(find_scale);
                    }
                    else if (find_cb != std::string::npos) {
                        LinkImgTmp.erase(find_cb);
                    }
                    writeLink << LinkImgTmp << "\n";
                    std::cout << termcolor::cyan << LinkImgTmp << "\n" << termcolor::reset;
                    //std::cout << LinkImgTmp.rfind("_Birthday_") << "->" << LinkImgTmp << "\n";
                }
            /*Case 13: already reserved for HoYoLab Character Avatar Frame*/
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
            if (LinkVidTmp.rfind("_Dynamic_Skin.gif/") != 18446744073709551615UL)
            {
                LinkVidTmp.erase(LinkVidTmp.end() - 18, LinkVidTmp.end());
                writeLink << LinkVidTmp << "\n";
                std::cout << termcolor::cyan << LinkVidTmp << "\n" << termcolor::reset;
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
            if (FileName.rfind("File:") != 18446744073709551615UL)
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

void search_for_a_character(GumboNode* node)
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
            if (ClassName.rfind("category-page__member-link") != 18446744073709551615UL && LinkStr.rfind("/Category:") == 18446744073709551615UL)
            {
                writeMedia << LinkStr << "/Gallery" << "\n";
                //std::cout << ClassName.rfind("category-page__member-link") << "->" << LinkStr << "/Gallery" << "\n";
            }
        }
    }
    GumboVector* child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++)
    {
        search_for_a_character(static_cast<GumboNode*>(child->data[i]));
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
            if (ClassName.rfind("category-page__member-link") != 18446744073709551615UL)
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
            if (ClassName.rfind("category-page__member-link") != 18446744073709551615UL)
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

void search_for_img_version(GumboNode *node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        return;
    }

    if (node->v.element.tag == GUMBO_TAG_IMG)
    {
        GumboAttribute *href = gumbo_get_attribute(&node->v.element.attributes, "src");
        if (href)
        {
            std::string LinkStr = href->value;
            if (LinkStr.rfind("_Wallpaper_") != 18446744073709551615UL && LinkStr.rfind("Overview") == 18446744073709551615UL && LinkStr.rfind("Newsletter") == 18446744073709551615UL && LinkStr.rfind("Trailer") == 18446744073709551615UL)
            {
                writeVer << LinkStr << "\n";
                //std::cout << LinkStr.rfind("/Version_") << "->" << LinkStr << "\n";
            }
        }
    }
    GumboVector *child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++)
    {
        search_for_img_version(static_cast<GumboNode *>(child->data[i]));
    }
}

void search_for_img_char_frame(GumboNode *node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        return;
    }

    if (node->v.element.tag == GUMBO_TAG_IMG)
    {
        GumboAttribute *href = gumbo_get_attribute(&node->v.element.attributes, "src");
        if (href)
        {
            std::string LinkStr = href->value;
            if (LinkStr.rfind("_Avatar_Frame_") != 18446744073709551615UL)
            {
                writeAvatarFrm << LinkStr << "\n";
                //std::cout << LinkStr.rfind("_Avatar_Frame_") << "->" << LinkStr << "\n";
            }
        }
    }
    GumboVector *child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++)
    {
        search_for_img_char_frame(static_cast<GumboNode *>(child->data[i]));
    }
}

void search_for_img_splashscr(GumboNode* node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        return;
    }

    if (node->v.element.tag == GUMBO_TAG_IMG)
    {
        GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "src");
        if (href)
        {
            std::string LinkStr = href->value;
            if (LinkStr.rfind("Splashscreen_") != 18446744073709551615UL)
            {
                writeSplhScr << LinkStr << "\n";
                //std::cout << LinkStr.rfind("Splashscreen_") << "->" << LinkStr << "\n";
            }
        }
    }
    GumboVector* child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++)
    {
        search_for_img_splashscr(static_cast<GumboNode*>(child->data[i]));
    }
}

std::vector<std::string> get_img_links()
{
    std::string line;
    std::vector<std::string> img_links;
    while (std::getline(readLink, line))
    {
        //std::cout << "Read line " << line << std::endl;
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
        //std::cout << "Read line " << line << std::endl;
        img_links.push_back(line);
    }
    readChara.close();
    return img_links;
}

std::vector<std::string> extract_character_tgc_link()
{
    std::string line;
    std::vector<std::string> img_links;
    while (std::getline(readTGC, line))
    {
        //std::cout << "Read line " << line << std::endl;
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
        //std::cout << "Read line " << line << std::endl;
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
        //std::cout << "Read line " << line << std::endl;
        img_links.push_back(line);
    }
    readVer.close();
    return img_links;
}

std::vector<std::string> extract_media_link()
{
    std::string line;
    std::vector<std::string> img_links;
    while (std::getline(readMedia, line))
    {
        //std::cout << "Read line " << line << std::endl;
        img_links.push_back(line);
        
    }
    readMedia.close();
    return img_links;
}

std::vector<std::string> extract_character_namecard_bp_link()
{
    std::string line;
    std::vector<std::string> img_links;
    while (std::getline(readNameCardBP, line))
    {
        //std::cout << "Read line " << line << std::endl;
        img_links.push_back(line);
    }
    readNameCardBP.close();
    return img_links;
}

std::vector<std::string> extract_splash_screen_link()
{
    std::string line;
    std::vector<std::string> img_links;
    while (std::getline(readSplhScr, line))
    {
        //std::cout << "Read line " << line << std::endl;
        img_links.push_back(line);
    }
    readSplhScr.close();
    return img_links;
}

std::vector<std::string> extract_avatar_frm_link()
{
    std::string line;
    std::vector<std::string> img_links;
    while (std::getline(readAvatarFrm, line))
    {
        //std::cout << "Read line " << line << std::endl;
        img_links.push_back(line);
    }
    readAvatarFrm.close();
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
    std::cerr << "\r";
    std::cerr << std::flush;
    return 0;
}

int download_progress_default_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    std::cout << "\r";
    std::cout << std::flush;
    return CURL_PROGRESSFUNC_CONTINUE;
}

void close_all(bool verbose)
{
    //Close all file
    writeChara.close();
    writeImgLink.close();
    writeLink.close();
    writeNameCardBP.close();
    writeDynamicsTGC.close();
    writeVer.close();
    writeTGC.close();
    writeMedia.close();
    writeSplhScr.close();
    readChara.close();
    readLink.close();
    readNameCardBP.close();
    readVer.close();
    readTGC.close();
    readDynamicsTGC.close();
    readMedia.close();
    readSplhScr.close();
    //Delete all the temporary file
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

void downloads_images(std::string url, std::string file_name, CURL *curl)
{
    static bool cursor_init = false;
    if (!cursor_init) {
        indicators::show_console_cursor(false);
        cursor_init = true;
    }
    static indicators::ProgressBar prog_bar{
        indicators::option::BarWidth{65},
        indicators::option::Start{" ["},
        indicators::option::Fill{"█"},
        indicators::option::Lead{"█"},
        indicators::option::Remainder{"-"},
        indicators::option::End{"]"},
        indicators::option::ShowElapsedTime{true},
        indicators::option::ShowRemainingTime{true},
        indicators::option::Stream{std::cerr}};

    prog_bar.set_option(indicators::option::PrefixText{ file_name });
    prog_bar.set_progress(0);
    FILE *f;
    CURLcode res;
    curl = curl_easy_init();
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
        if (res != CURLE_OK) {
            std::cerr << termcolor::bold << termcolor::bright_red << "cUrl Error: " << curl_easy_strerror(res) << std::endl << termcolor::reset;
        }
        else {
            long http_res = 0;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_res);
            if (http_res != 200) {
                std::cerr << termcolor::bold << termcolor::bright_red << "HTTP Error: " << http_res << std::endl << termcolor::reset;
            }
            else {
                std::cout << termcolor::bright_green << "Downloaded: " << file_name << termcolor::reset << std::endl;
            }
        }
        prog_bar.mark_as_completed();
        fclose(f);
    }
    else
    {
        std::cerr << "Can't create file !\n";
        close_all(verbose);
        exit(-1);
    }
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
            dir = "Genshin Version Wallpaper\\";
        #else
            dir = "Genshin Version Wallpaper/";
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
    case 10:
        #if defined(_WIN32)
                dir = "HoYoLab Genshin Sticker\\";
        #else
                dir = "HoYoLab Genshin Sticker/";
        #endif
        break;
    case 11:
        #if defined(_WIN32)
                dir = "Genshin Version Splash Screen\\";
        #else
                dir = "Genshin Version Splash Screen/";
        #endif
        break;
    case 12:
        #if defined(_WIN32)
            dir = "Genshin Character Birthday Art\\";
        #else
            dir = "Genshin Character Birthday Art/";
        #endif
        break;
    case 13:
        #if defined(_WIN32)
            dir = "HoYoLab Character Avatar Frame\\";
        #else
            dir = "HoYoLab Character Avatar Frame/";
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
    if (!std::__fs::filesystem::exists(dir)) {
        std::__fs::filesystem::create_directory(dir);
        std::cout << termcolor::bright_yellow << "Creating folder " << dir << "\n" << termcolor::reset;
    }
    else {
        if (std::_fs::filesystem::is_directory(dir))
        {
            std::cout << termcolor::bright_yellow << "Folder already exist\n" << termcolor::reset;
            if (!std::__fs::filesystem::is_empty(dir))
            {
                for (const auto& files : std::__fs::filesystem::directory_iterator(dir))
                {
                    std::cout << termcolor::yellow << "Clearing existing file\n" << termcolor::reset;
                    std::__fs::filesystem::remove_all(files.path());
                }
            }
        }
    }
    #else
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directory(dir);
        std::cout << termcolor::bright_yellow << "Creating folder " << dir << "\n" << termcolor::reset;
            #if defined(__linux__) && defined(__unix__)
        std::filesystem::permissions(dir, std::filesystem::perms::owner_all | std::filesystem::perms::group_read, std::filesystem::perm_options::add);
            #endif
    }
    else {
        if (std::filesystem::is_directory(dir))
        {  
            std::cout << termcolor::bright_yellow << "Folder already exist\n" << termcolor::reset;
            if (!std::filesystem::is_empty(dir))
            {
                for (const auto& files : std::filesystem::directory_iterator(dir))
                {
                    std::cout << termcolor::yellow << "Clearing existing file\n" << termcolor::reset;
                    std::filesystem::remove_all(files.path());
                }
            }
        }
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
            argparse::ArgumentParser program("wsgcc");
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
    
                std::cerr << termcolor::bright_red << err.what() << termcolor::reset << std::endl;
                std::cerr << program;
                std::cin.ignore();
                std::cin.get();
                close_all(verbose);
                std::exit(-1);
            }
        } else {

            std::cout << termcolor::bright_cyan << "No command line args supplied\n" << termcolor::reset;
        }
        // Init variables
        std::cout << termcolor::bright_magenta << "Getting character list from wiki\n";
        std::vector<std::string> splh_scr_vecs, temp_splh_scr, img_vecs, ver_vecs, tgc_vecs, card_bp_vecs, media_vecs, avatar_frm_vecs, temp_avatar_frm, temp_chara, temp_ver, temp_tgc, temp_card_bp, temp_media;
        
        // Get character list from /wiki/Category:Character_Cards
        std::string page_content_chara = extract_html_page_category();
        GumboOutput *parsed_res_chara = gumbo_parse(page_content_chara.c_str());
        search_for_a_name(parsed_res_chara->root);
        writeChara.close();
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_chara);
        
        // Get version images from /wiki/Version/Gallery
        std::cout << termcolor::blue << "Getting Version list from wiki\n";
        std::string page_content_version = extract_html_page_version();
        GumboOutput *parsed_res_version = gumbo_parse(page_content_version.c_str());
        search_for_img_version(parsed_res_version->root);
        writeVer.close();
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_version);
        
        // Get TGC character card list from /wiki/Category:Genius_Invokation_TCG_Character_Cards
        std::cout << termcolor::magenta << "Getting TCG Card list from wiki\n";
        std::string page_content_TGC = extract_html_page_category_TGC();
        GumboOutput *parsed_res_TGC = gumbo_parse(page_content_TGC.c_str());
        search_for_a_TGC(parsed_res_TGC->root);
        writeTGC.close();
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_TGC);
        
        // Get TGC character card list from /wiki/Category:Battle_Pass_Namecards
        std::cout << termcolor::cyan << "Getting Battle Pass Namecard list from wiki\n" << termcolor::reset;
        std::string page_content_BP_namecard = extract_html_page_category_namecard_bp();
        GumboOutput* parsed_res_BP_namecard = gumbo_parse(page_content_BP_namecard.c_str());
        search_for_a_namecard_bp(parsed_res_BP_namecard->root);
        writeNameCardBP.close();
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_BP_namecard);

        //Get Sticker character list from /wiki/Category:Playable_Characters
        std::cout << termcolor::bright_red << "Getting Playable Character list from wiki\n" << termcolor::reset;
        std::string page_content_chara_sticker = extract_html_page_category_character();
        GumboOutput* parsed_res_chara_sticker = gumbo_parse(page_content_chara_sticker.c_str());
        search_for_a_character(parsed_res_chara_sticker->root);
        writeMedia.close();
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_chara_sticker);

        // Get vision images from /wiki/Version/Gallery -> Splash Screen
        std::cout << termcolor::bright_cyan << "Getting Version Splash Screen list from wiki\n" << termcolor::reset;
        std::string page_content_splash_screen = extract_html_page_version();
        GumboOutput* parsed_res_splash_screen = gumbo_parse(page_content_splash_screen.c_str());
        search_for_img_splashscr(parsed_res_splash_screen->root);
        writeSplhScr.close();
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_splash_screen);

        // Get version images from /wiki/HoYoLAB/Gallery
        std::cout << termcolor::bright_blue << "Getting Character Avatar Frame list from wiki\n"<< termcolor::reset;
        std::string page_content_char_frame = extract_html_page_hoyolab_frames();
        GumboOutput *parsed_res_char_frame = gumbo_parse(page_content_char_frame.c_str());
        search_for_img_char_frame(parsed_res_char_frame->root);
        writeAvatarFrm.close();
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_char_frame);

        // Get character link based by character category
        temp_chara = extract_character_chara_link();
        img_vecs = sanitize_vecs(temp_chara);
        // Get version link based by version images
        temp_ver = extract_version_link();
        ver_vecs = sanitize_vecs(temp_ver);
        // Get namecard link based by tgc category
        temp_tgc = extract_character_tgc_link();
        tgc_vecs = sanitize_vecs(temp_tgc);
        // Get namecard link based by BP namecard category
        temp_card_bp = extract_battle_pass_namecard_link();
        card_bp_vecs = sanitize_vecs(temp_card_bp);
        // Get chracter media link based by Characters Gallery
        temp_media = extract_media_link();
        media_vecs = sanitize_vecs(temp_media);
        // Get version link based by splash screen images
        temp_splh_scr = extract_splash_screen_link();
        splh_scr_vecs = sanitize_vecs(temp_splh_scr);
        // Get Avatar Frame link based by Avatar Frame images
        temp_avatar_frm = extract_avatar_frm_link();
        avatar_frm_vecs = sanitize_vecs(temp_avatar_frm);

        //Main process
        char opts;
        do {
            //Init cUrl
            std::cout << termcolor::reverse << termcolor::bold << "Init cUrl\n";
            CURL* curl = curl_easy_init();
            if (!curl) {
                std::cerr << "Failed to initialized cUrl !\n";
                close_all(verbose);
                return -1;
            }

            if (opt == 99) {
                std::cout << termcolor::bold << termcolor::red << "Getting character link image." << termcolor::reset << "\nWhat image do you want ? \n1. Card\n2. Wish\n3. Constellation\n4. Introduction Banner\n5. Namecard\n6. Version Wallpapers\n7. Character TGC Card\n8. Battle Pass Namecard\n9. Dynamics Character TGC Card\n10. HoYoLab Paimon's Painting Sticker\n11. Version Splash Screen\n12. Birthday cards\n13. HoYoLab Character Frame\n0. Cancel\n";
                std::cin >> opt;
            }
            switch (opt)
            {
            case 1:
                create_download_folder(opt);
                for (std::size_t i = 8; i < img_vecs.size() - 3; i++)
                {
                    std::string page_chara_content = extract_html_page_character(img_vecs[i]);
                    GumboOutput* parsed_res_chara = gumbo_parse(page_chara_content.c_str());
                    search_for_img(parsed_res_chara->root, opt);
                    gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_chara);
                }
                break;
            case 2:
                create_download_folder(opt);
                for (std::size_t i = 0; i < media_vecs.size(); i++)
                {
                    std::string page_chara_content = extract_html_page_character(media_vecs[i]);
                    GumboOutput* parsed_res_chara = gumbo_parse(page_chara_content.c_str());
                    search_for_img(parsed_res_chara->root, opt);
                    gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_chara);
                }
                break;
            case 3:
                create_download_folder(opt);
                for (std::size_t i = 0; i < media_vecs.size(); i++)
                {
                    std::string page_const_content = extract_html_page_character(media_vecs[i]);
                    GumboOutput* parsed_res_const = gumbo_parse(page_const_content.c_str());
                    search_for_img(parsed_res_const->root, opt);
                    gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_const);
                }
                break;
            case 4:
                create_download_folder(opt);
                for (std::size_t i = 0; i < media_vecs.size(); i++)
                {
                    std::string page_intro_content = extract_html_page_character(media_vecs[i]);
                    GumboOutput* parsed_res_intro = gumbo_parse(page_intro_content.c_str());
                    search_for_img(parsed_res_intro->root, opt);
                    gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_intro);
                }
                break;
            case 5:
                create_download_folder(opt);
                for (std::size_t i = 0; i < media_vecs.size(); i++)
                {
                    std::string page_card_content = extract_html_page_character(media_vecs[i]);
                    GumboOutput* parsed_res_card = gumbo_parse(page_card_content.c_str());
                    search_for_img(parsed_res_card->root, opt);
                    gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_card);
                }
                break;
            case 6:
                create_download_folder(opt);
                //Version image link alrady been stored and ready to be downloaded
                break;
            case 7:
                create_download_folder(opt);
                for (std::size_t i = 2; i < tgc_vecs.size(); i++)
                {
                    std::string page_tgc_content = extract_html_page_character(tgc_vecs[i]);
                    GumboOutput* parsed_res_tgc = gumbo_parse(page_tgc_content.c_str());
                    search_for_img(parsed_res_tgc->root, opt);
                    gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_tgc);
                }
                break;
            case 8:
                create_download_folder(opt);
                for (std::size_t i = 2; i < card_bp_vecs.size(); i++)
                {
                    std::string page_bp_content = extract_html_page_character(card_bp_vecs[i]);
                    GumboOutput* parsed_res_bp = gumbo_parse(page_bp_content.c_str());
                    search_for_img(parsed_res_bp->root, opt);
                    gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_bp);
                }
                break;
            case 9:
                create_download_folder(opt);
                for (std::size_t i = 2; i < tgc_vecs.size(); i++)
                {
                    std::string page_tgc_content_dyn = extract_html_page_character(tgc_vecs[i]);
                    GumboOutput* parsed_res_tgc_dyn = gumbo_parse(page_tgc_content_dyn.c_str());
                    search_for_video(parsed_res_tgc_dyn->root);
                    gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_tgc_dyn);
                }
                break;
            case 10:
                create_download_folder(opt);
                for (std::size_t i = 0; i < media_vecs.size(); i++)
                {
                    std::string page_card_content = extract_html_page_character(media_vecs[i]);
                    GumboOutput* parsed_res_sticker = gumbo_parse(page_card_content.c_str());
                    search_for_img(parsed_res_sticker->root, opt);
                    gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_sticker);
                }
                break;
            case 11:
                create_download_folder(opt);
                //Splash Screen image link already been stored and ready to be downloaded
                break;
            case 12:
                create_download_folder(opt);
                for (std::size_t i = 0; i < media_vecs.size(); i++)
                {
                    std::string page_card_content = extract_html_page_character(media_vecs[i]);
                    GumboOutput* parsed_res_hbd = gumbo_parse(page_card_content.c_str());
                    search_for_img(parsed_res_hbd->root, opt);
                    gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_hbd);
                }
                break;
            case 13:
                create_download_folder(opt);
                break;
            case 0:
                std::cout << termcolor::bright_blue << termcolor::on_bright_white << "Bye !" << termcolor::reset;
                std::cin.ignore();
                std::cin.get();
                close_all(verbose);
                exit(-1);
            default:
                create_download_folder(opt);
                for (std::size_t i = 0; i < img_vecs.size(); i++)
                {
                    std::string page_chara_content = extract_html_page_character(img_vecs[i]);
                    GumboOutput* parsed_res_chara = gumbo_parse(page_chara_content.c_str());
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
            std::cout << termcolor::bold << termcolor::bright_magenta << "Downloading Images\n" << termcolor::reset;
            std::vector<std::string> link_vecs;
            std::string file_name;
            size_t pos;
            if (opt == 6 || opt == 11 || opt == 13)
            {
                switch (opt) {
                case 6:
                    for (int i = 0; i < ver_vecs.size(); i++)
                    {
                        std::string links = ver_vecs[i];
                        pos = links.find("/scale-to-width-down/");
                        if (pos != std::string::npos) {
                            links.erase(pos);
                        }
                        file_name = links;
                        file_name.erase(0, 60);
                        pos = file_name.find("/revision/latest");
                        if (pos != std::string::npos) {
                            file_name.erase(pos);
                        }
                        downloads_images(links, dir + file_name, curl);
                    }
                    break;
                case 11:
                    for (int i = 0; i < splh_scr_vecs.size(); i++)
                    {
                        std::string links = splh_scr_vecs[i];
                        pos = links.find("/scale-to-width-down/");
                        if (pos != std::string::npos) {
                            links.erase(pos);
                        }
                        file_name = links;
                        file_name.erase(0, 60);
                        pos = file_name.find("/revision/latest");
                        if (pos != std::string::npos) {
                            file_name.erase(pos);
                        }
                        downloads_images(links, dir + file_name, curl);
                    }
                    break;
                case 13:
                    for (int i = 0; i < avatar_frm_vecs.size(); i++)
                    {
                        std::string links = avatar_frm_vecs[i];
                        pos = links.find("/scale-to-width-down/");
                        if (pos != std::string::npos) {
                            links.erase(pos);
                        }
                        file_name = links;
                        file_name.erase(0, 60);
                        pos = file_name.find("/revision/latest");
                        if (pos != std::string::npos) {
                            file_name.erase(pos);
                        }
                        downloads_images(links, dir + file_name, curl);
                    }
                    break;
                default:
                    break;
                }
                std::cout << termcolor::bold << termcolor::bright_green << "Images succesfully downloaded !\n" << termcolor::reset;
            }
            else
            {
                link_vecs = get_img_links();
                std::sort(link_vecs.begin(), link_vecs.end());
                auto iter = std::unique(link_vecs.begin(), link_vecs.end());
                link_vecs.erase(iter, link_vecs.end());
                for (size_t i = 0; i < link_vecs.size(); i++)
                {
                    file_name = link_vecs[i];
                    file_name.erase(0, 60);
                    if (file_name.back() == '/') {
                        file_name.erase(file_name.size() - 17);
                    }
                    else {
                        file_name.erase(file_name.size() - 16);
                    }
                    downloads_images(link_vecs[i], dir + file_name, curl);
                }
                std::cout << termcolor::bold << termcolor::bright_green <<"\nImages succesfully downloaded !\n" << termcolor::reset;
            }
            // Check if file is closed properly
            readLink.close();
            curl_easy_cleanup(curl);
            close_all(verbose);
            curl = nullptr;
			writeLink.open("ImgLink.gsct", std::ios::trunc);
			writeLink.close();
			link_vecs.clear();

            std::cout << termcolor::cyan << termcolor::bold << "Want to download other images ? (Yy/Nn)" << termcolor::reset <<std::endl;
            std::cin >> opts;
            opt = 99;
        
        }while (opts == 'Y' || opts == 'y');
        
        // Exit gracefully and return memory to OS
        return 0;
    }
}