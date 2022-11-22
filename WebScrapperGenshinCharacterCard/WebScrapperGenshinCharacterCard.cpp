// Suppress warning for fopen (depecration warning: Security SEVERE)
#define _CRT_SECURE_NO_WARNINGS
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define NOMINMAX
// Included library STL
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
#include <curl/curl.h>
#include <curl/easy.h>
// Included library external
#include <cpr/cpr.h>
#include <gumbo.h>
#include <indicators/progress_bar.hpp>
#include <indicators/cursor_control.hpp>

// Preprocessor
#if defined(__linux__)
#include <unistd.h>
#include <bits/stdc++.h>
#elif defined(_WIN32)
#include <windows.h>
#include <Wininet.h>
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Wldap32.lib")
#pragma comment(lib, "Crypt32.lib")
#elif defined(__APPLE__) && defined(__MACH__)
#include <sys/sysctl.h>
#endif

// This is an example static links assets of character card image:
// https://static.wikia.nocookie.net/gensin-impact/images/f/f8/Character_Albedo_Card.png/revision/latest
// This is an example static links assets of character wish image:
// https://static.wikia.nocookie.net/gensin-impact/images/5/51/Character_Albedo_Full_Wish.png/revision/latest
//  This is an example static links assets of character constellation images :
//  https://static.wikia.nocookie.net/gensin-impact/images/8/84/Trifolium_Shape.png/revision/latest
//  This is an example static links assets of character introduction images:
//  https://static.wikia.nocookie.net/gensin-impact/images/b/b1/Character_Albedo_Introduction.png/revision/latest
//  This is an example static links assets of character namecard images:
//  https://static.wikia.nocookie.net/gensin-impact/images/5/55/Item_Albedo_Sun_Blossom.png/revision/latest
//  This is an example character wiki page link:
//  https://genshin-impact.fandom.com/wiki/Albedo
// Tutorial link:
// https://www.webscrapingapi.com/c-web-scraping/

std::string root_url = "https://genshin-impact.fandom.com";
int nb_bar;
double last_progress, progress_bar_adv;
std::ofstream writeChara("FileName.gsct");
std::ofstream writeImgLink("FileImg.gsct");
std::ofstream writeLink("ImgLink.gsct");
std::ofstream writeConst("FileConst.gsct");
std::ofstream writeIntro("FileIntro.gsct");
std::ofstream writeNamecard("FileNamecard.gsct");
std::ifstream readChara("FileName.gsct");
std::ifstream readLink("ImgLink.gsct");
std::ifstream readConst("FileConst.gsct");
std::ifstream readIntro("FileIntro.gsct");
std::ifstream readCard("FileNamecard.gsct");

#if defined(_WIN32)
bool checkInet()
{
    bool con = InternetCheckConnectionA("https://www.google.com", FLAG_ICC_FORCE_CONNECTION, 0);
    return con;
}
#elif defined(__linux__) && defined(__unix__)
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

std::string extract_html_page_category_chara_intro()
{
    cpr::Url url_category = cpr::Url{root_url + "/wiki/Category:Character_Introduction_Cards"};
    cpr::Response res = Get(url_category);
    return res.text;
}

std::string extract_html_page_category_namecard()
{
    cpr::Url url_category = cpr::Url{ root_url + "/wiki/Category:Character_Namecards" };
    cpr::Response res = Get(url_category);
    return res.text;
}

std::string extract_html_page_category_const()
{
    cpr::Url url_category = cpr::Url{root_url + "/wiki/Category:Constellation_Overviews"};
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
            if (imgType == 1)
            {
                if (LinkImgTmp.rfind("_Card") != 18446744073709551615)
                {
                    LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                    writeLink << LinkImgTmp << "\n";
                    std::cout << LinkImgTmp << "\n";
                }
            }
            else if (imgType == 2)
            {
                if (LinkImgTmp.rfind("_Wish") != 18446744073709551615)
                {
                    LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                    writeLink << LinkImgTmp << "\n";
                    std::cout << LinkImgTmp << "\n";
                }
            }
            else if (imgType == 3)
            {
                if (LinkImgTmp.rfind("_Shape") != 18446744073709551615)
                {
                    LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                    writeLink << LinkImgTmp << "\n";
                    std::cout << LinkImgTmp << "\n";
                }
            }
            else if (imgType == 4)
            {
                if (LinkImgTmp.rfind("_Introduction.") != 18446744073709551615)
                {
                    LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                    writeLink << LinkImgTmp << "\n";
                    std::cout << LinkImgTmp << "\n";
                }
            }
            else if (imgType == 5)
            {
                if (LinkImgTmp.rfind("Namecard_Background_") != 18446744073709551615)
                {
                    LinkImgTmp.erase(LinkImgTmp.end() - 41, LinkImgTmp.end());
                    writeLink << LinkImgTmp << "\n";
                    std::cout << LinkImgTmp << "\n";
                }
            }
        }
    }
    GumboVector *child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++)
    {
        search_for_img(static_cast<GumboNode *>(child->data[i]), imgType);
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

void search_for_a_namecard(GumboNode* node)
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
                writeNamecard << LinkStr << "\n";
            }
        }
    }
    GumboVector* child = &node->v.element.children;
    for (unsigned int i = 0; i < child->length; i++)
    {
        search_for_a_namecard(static_cast<GumboNode*>(child->data[i]));
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

std::vector<std::string> extract_character_namecard_link()
{
    std::string line;
    std::vector<std::string> img_links;
    while (std::getline(readCard, line))
    {
        std::istringstream ISS;
        img_links.push_back(line);
    }
    readCard.close();
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

void downloads_images(std::string url, std::string file_name)
{
    indicators::show_console_cursor(false);
    indicators::ProgressBar prog_bar{
        indicators::option::BarWidth{65}, indicators::option::Start{" ["}, indicators::option::Fill{"█"}, indicators::option::Lead{"█"}, indicators::option::Remainder{"-"}, indicators::option::End{"]"},
        indicators::option::PrefixText{file_name}, indicators::option::ShowElapsedTime{true}, indicators::option::ShowRemainingTime{true}};
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
            exit(-1);
        }
    }
    else
    {
        std::cout << (stderr, "Can't initialize cUrl !");
        exit(-1);
    }
    indicators::show_console_cursor(true);
}

int main()
{
    if (!checkInet())
    {
        std::cout << "Failed to connect to internet, this program need internet to working properly !"<< "\n";
        std::cin.ignore();
        std::cin.get();
        writeChara.close();
        writeConst.close();
        writeImgLink.close();
        writeLink.close();
        writeIntro.close();
        writeNamecard.close();
        readChara.close();
        readLink.close();
        readConst.close();
        readIntro.close();
        readCard.close();
        for (auto const& entry : std::filesystem::directory_iterator{ std::filesystem::current_path().string() }) {
            if (entry.path().extension().string() == ".txt") {
                std::filesystem::remove(entry.path());
            }
        }
        exit(-1);
    }
    else
    {
        // Get character list from /wiki/Category:Character_Cards
        std::cout << "Getting character list from wiki\n";
        std::vector<std::string> const_vecs, img_vecs, temp_chara, temp_const, intro_vecs, temp_vecs, temp_intro, temp_card, card_vecs;
        std::string page_content_chara = extract_html_page_category();
        GumboOutput *parsed_res_chara = gumbo_parse(page_content_chara.c_str());
        search_for_a_name(parsed_res_chara->root);
        writeChara.close();
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_chara);
        // Get character constellation list from /wiki/Category:Constellation_Overviews
        std::string page_content_const = extract_html_page_category_const();
        GumboOutput *parsed_res_const = gumbo_parse(page_content_const.c_str());
        search_for_a_const(parsed_res_const->root);
        writeConst.close();
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_const);
        // Get character intorduction banner list from /wiki/Category:Character_Introduction_Cards
        std::string page_content_intro = extract_html_page_category_chara_intro();
        GumboOutput *parsed_res_intro = gumbo_parse(page_content_intro.c_str());
        search_for_a_intro(parsed_res_intro->root);
        writeIntro.close();
        gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_intro);
        // Get character intorduction banner list from /wiki/Category:Character_Namecards
        std::string page_content_namecard = extract_html_page_category_namecard();
        GumboOutput* parsed_res_namecard = gumbo_parse(page_content_namecard.c_str());
        search_for_a_namecard(parsed_res_namecard->root);
        writeNamecard.close();
        // Get character link based by character category
        temp_chara = extract_character_chara_link();
        img_vecs = sanitize_vecs(temp_chara);
        // Get constellation link based by constellation category
        temp_const = extract_character_const_link();
        const_vecs = sanitize_vecs(temp_const);
        // Get constellation link based by introduction category
        temp_intro = extract_character_intro_link();
        intro_vecs = sanitize_vecs(temp_intro);
        // Get constellation link based by namecard category
        temp_card = extract_character_namecard_link();
        card_vecs = sanitize_vecs(temp_card);
        // Initialize directory for storing images
        std::string dir;
        int opt = 0;
        std::cout << "Getting character link image.\nWhat image do you want ?\n1. Card\n2. Wish\n3. Constellation\n4. Introduction Banner\n5. Namecard\n0. Cancel\n";
        std::cin >> opt;
        switch (opt)
        {
        case 1:
            // Init folder for contain all image file
            dir = "Character Genshin Card Image\\";
            if (std::filesystem::is_directory(dir))
            {
                if (!std::filesystem::is_empty("Character Genshin Card Image"))
                {
                    for (const auto &files : std::filesystem::directory_iterator("Character Genshin Card Image"))
                    {
                        std::cout << "Clearing existing file\n";
                        std::filesystem::remove_all(files.path());
                    }
                }
            }
            else
            {
                std::filesystem::create_directory("Character Genshin Card Image");
                std::cout << "Creating folder\n";
                #if defined(__linux__) && defined(__unix__)
                std::filesystem::permissions("Character Genshin Card Image", std::filesystem::perms::owner_all | std::filesystem::perms::group_read, std::filesystem::perm_options::add);
                #endif
            }

            for (int i = 8; i < img_vecs.size() - 3; i++)
            {
                std::string page_chara_content = extract_html_page_character(img_vecs[i]);
                GumboOutput *parsed_res_chara = gumbo_parse(page_chara_content.c_str());
                search_for_img(parsed_res_chara->root, opt);
                gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_chara);
            }
            break;
        case 2:
            // Init folder for contain all image file
            dir = "Character Genshin Wish Image\\";
            if (std::filesystem::is_directory(dir))
            {
                if (!std::filesystem::is_empty("Character Genshin Wish Image"))
                {
                    for (const auto &files : std::filesystem::directory_iterator("Character Genshin Wish Image"))
                    {
                        std::cout << "Clearing existing file\n";
                        std::filesystem::remove_all(files.path());
                    }
                }
            }
            else
            {
                std::filesystem::create_directory("Character Genshin Wish Image");
                std::cout << "Creating folder\n";
                #if defined(__linux__) && defined(__unix__)
                std::filesystem::permissions("Character Genshin Wish Image", std::filesystem::perms::owner_all | std::filesystem::perms::group_read, std::filesystem::perm_options::add);
                #endif
            }
            for (int i = 8; i < img_vecs.size() - 3; i++)
            {
                std::string page_chara_content = extract_html_page_character(img_vecs[i]);
                GumboOutput *parsed_res_chara = gumbo_parse(page_chara_content.c_str());
                search_for_img(parsed_res_chara->root, opt);
                gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_chara);
            }
            break;
        case 3:
            dir = "Character Genshin Constellation Image\\";
            if (std::filesystem::is_directory(dir))
            {
                if (!std::filesystem::is_empty("Character Genshin Constellation Image"))
                {
                    for (const auto &files : std::filesystem::directory_iterator("Character Genshin Constellation Image"))
                    {
                        std::cout << "Clearing existing file\n";
                        std::filesystem::remove_all(files.path());
                    }
                }
            }
            else
            {
                std::filesystem::create_directory("Character Genshin Constellation Image");
                std::cout << "Creating folder\n";
                #if defined(__linux__) && defined(__unix__)
                std::filesystem::permissions("Character Genshin Constellation Image", std::filesystem::perms::owner_all | std::filesystem::perms::group_read, std::filesystem::perm_options::add);
                #endif
            }
            for (int i = 0; i < const_vecs.size(); i++)
            {
                std::string page_const_content = extract_html_page_character(const_vecs[i]);
                GumboOutput *parsed_res_const = gumbo_parse(page_const_content.c_str());
                search_for_img(parsed_res_const->root, opt);
                gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_const);
            }
            break;
        case 4:
            dir = "Character Genshin Introduction Card Image\\";
            if (std::filesystem::is_directory(dir))
            {
                if (!std::filesystem::is_empty("Character Genshin Introduction Card Image"))
                {
                    for (const auto &files : std::filesystem::directory_iterator("Character Genshin Introduction Card Image"))
                    {
                        std::cout << "Clearing existing file\n";
                        std::filesystem::remove_all(files.path());
                    }
                }
            }
            else
            {
                std::filesystem::create_directory("Character Genshin Introduction Card Image\\");
                std::cout << "Creating folder\n";
                #if defined(__linux__) && defined(__unix__)
                std::filesystem::permissions("Character Genshin Introduction Card Image", std::filesystem::perms::owner_all | std::filesystem::perms::group_read, std::filesystem::perm_options::add);
                #endif
            }
            for (int i = 0; i < intro_vecs.size(); i++)
            {
                std::string page_intro_content = extract_html_page_character(intro_vecs[i]);
                GumboOutput *parsed_res_intro = gumbo_parse(page_intro_content.c_str());
                search_for_img(parsed_res_intro->root, opt);
                gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_intro);
            }
            break;
        case 5:
            dir = "Character Genshin Namecard Background Image\\";
            if (std::filesystem::is_directory(dir))
            {
                if (!std::filesystem::is_empty("Character Genshin Namecard Background Image"))
                {
                    for (const auto& files : std::filesystem::directory_iterator("Character Genshin Namecard Background Image"))
                    {
                        std::cout << "Clearing existing file\n";
                        std::filesystem::remove_all(files.path());
                    }
                }
            }
            else
            {
                std::filesystem::create_directory("Character Genshin Namecard Background Image");
                std::cout << "Creating folder\n";
                #if defined(__linux__) && defined(__unix__)
                std::filesystem::permissions("Character Genshin Namecard Background Image", std::filesystem::perms::owner_all | std::filesystem::perms::group_read, std::filesystem::perm_options::add);
                #endif
            }
            for (int i = 0; i < card_vecs.size(); i++)
            {
                std::string page_card_content = extract_html_page_character(card_vecs[i]);
                GumboOutput* parsed_res_card = gumbo_parse(page_card_content.c_str());
                search_for_img(parsed_res_card->root, opt);
                gumbo_destroy_output(&kGumboDefaultOptions, parsed_res_card);
            }
            break;
        case 0:
            std::cout << "Bye !\n";
            std::cin.ignore();
            std::cin.get();
            writeChara.close();
            writeConst.close();
            writeImgLink.close();
            writeLink.close();
            writeIntro.close();
            writeNamecard.close();
            readChara.close();
            readLink.close();
            readConst.close();
            readIntro.close();
            readCard.close();
            for (auto const& entry : std::filesystem::directory_iterator{ std::filesystem::current_path().string() }) {
                if (entry.path().extension().string() == ".gsct") {
                    std::filesystem::remove(entry.path());
                }
            }
            exit(-1);
        default:
            // Init folder for contain all image file
            dir = "Character Genshin Card Image\\";
            if (std::filesystem::is_directory(dir))
            {
                if (!std::filesystem::is_empty("Character Genshin Card Image"))
                {
                    for (const auto &files : std::filesystem::directory_iterator("Character Genshin Card Image"))
                    {
                        std::cout << "Clearing existing file\n";
                        std::filesystem::remove_all(files.path());
                    }
                }
            }
            else
            {
                std::filesystem::create_directory("Character Genshin Card Image");
                std::cout << "Creating folder\n";
                #if defined(__linux__) && defined(__unix__)
                std::filesystem::permissions("Character Genshin Card Image", std::filesystem::perms::owner_all | std::filesystem::perms::group_read, std::filesystem::perm_options::add);
                #endif
            }
            for (int i = 8; i < img_vecs.size() - 3; i++)
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
        link_vecs = get_img_links();
        for (int i = 0; i < link_vecs.size(); i++)
        {
            file_name = link_vecs[i];
            file_name.erase(0, 60);
            file_name.erase(file_name.size() - 17);
            downloads_images(link_vecs[i], dir + file_name);
        }
        readLink.close();
        if(writeChara.is_open() || writeConst.is_open() || writeImgLink.is_open() || writeLink.is_open() || writeIntro.is_open() || writeNamecard.is_open() || readChara.is_open() || readLink.is_open() || readConst.is_open() || readIntro.is_open() || readCard.is_open()){
            writeChara.close();
            writeConst.close();
            writeImgLink.close();
            writeLink.close();
            writeIntro.close();
            writeNamecard.close();
            readChara.close();
            readLink.close();
            readConst.close();
            readIntro.close();
            readCard.close();
        }
        for (auto const& entry : std::filesystem::directory_iterator{ std::filesystem::current_path().string() }) {
            if (entry.path().extension().string() == ".gsct") {
                std::filesystem::remove(entry.path());
            }
        }
        // Exit gracefully and return memory to OS
        return 0;
    }
}