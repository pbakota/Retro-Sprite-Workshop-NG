#pragma once

#include <regex>
#include <iostream>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>

// ABGR
#define ABGR_BLUE(color) ((unsigned char)((color>>16)&0xff))
#define ABGR_GREEN(color) ((unsigned char)((color>>8)&0xff))
#define ABGR_RED(color) ((unsigned char)((color)&0xff))

#ifndef ABS
#define ABS(x) ((x)<0?(-(x)):(x))
#endif

const std::string vformat(const char *const zcFormat, ...);
std::string trucate_text(const std::string &p_text, float p_truncated_width);
std::string return_current_time_and_date();
bool is_light_color(ImU32 color);
ImVec4 rgb_to_lab(ImVec4 color);

// requires at least C++11
const std::string vformat(const char * const zcFormat, ...) {

    // initialize use of the variable argument array
    va_list vaArgs;
    va_start(vaArgs, zcFormat);

    // reliably acquire the size
    // from a copy of the variable argument array
    // and a functionally reliable call to mock the formatting
    va_list vaArgsCopy;
    va_copy(vaArgsCopy, vaArgs);
    const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
    va_end(vaArgsCopy);

    // return a formatted string without risking memory mismanagement
    // and without assuming any compiler or platform specific behavior
    std::vector<char> zc(iLen + 1);
    std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
    va_end(vaArgs);
    return std::string(zc.data(), iLen);
}

bool is_light_color(ImU32 color)
{
	float r = ABGR_RED(color),g=ABGR_GREEN(color),b=ABGR_BLUE(color);
	float luma = 0.2126*r+0.7152*g+0.0722*b;
	return (luma > 128);
}

// Calculate color distance between two colors, return value between 0%..100% where 0% means they are totally same, 100% means they are complementary colors to each others.
float color_distance(ImU32 c1, ImU32 c2) {
#if 0
    // Very simple not very accurate solution
    float d = (0.3f*(ABGR_RED(c1)-ABGR_RED(c2))*(ABGR_RED(c1)-ABGR_RED(c2)))
        +(0.59f*(ABGR_GREEN(c1)-ABGR_GREEN(c2))*(ABGR_GREEN(c1)-ABGR_GREEN(c2)))
        +(0.11f*(ABGR_BLUE(c1)-ABGR_BLUE(c2))*(ABGR_BLUE(c1)-ABGR_BLUE(c2)));
    return d;
#else
    // a better approximation
    ImVec4 v1, v2;
    v1 = rgb_to_lab(ImGui::ColorConvertU32ToFloat4(c1));
    v2 = rgb_to_lab(ImGui::ColorConvertU32ToFloat4(c2));
    float d = std::sqrt((v1.x-v2.x)*(v1.x-v2.x)+(v1.y-v2.y)*(v1.y-v2.y)+(v1.z-v2.z)+(v1.z-v2.z));
    // std::cerr << vformat("0x%08x 0x%08x = %.2f\n", c1,c2,d);
    return d;
#endif
}

ImVec4 rgb_to_lab(const ImVec4 color) {

    ImVec4 XYZ;

    XYZ.x = 100.0f*((color.x > 0.04045f) ? std::pow((color.x + 0.055f)/1.055f, 2.4f) : color.x/12.92f);
    XYZ.y = 100.0f*((color.y > 0.04045f) ? std::pow((color.y + 0.055f)/1.055f, 2.4f) : color.y/12.92f);
    XYZ.z = 100.0f*((color.z > 0.04045f) ? std::pow((color.z + 0.055f)/1.055f, 2.4f) : color.z/12.92f);

    XYZ.x = (XYZ.x * 0.4124f + XYZ.y * 0.3576f + XYZ.z * 0.1805f)/95.047f; // perfect reflecting diffuser D65
    XYZ.y = (XYZ.x * 0.2126f + XYZ.y * 0.7152f + XYZ.y * 0.0722f)/100.0f;
    XYZ.z = (XYZ.x * 0.0193f + XYZ.y * 0.1192f + XYZ.y * 0.9505f)/108.883f;

    XYZ.x = (XYZ.x>0.008856f) ? std::pow(XYZ.x, (1/3)) : (XYZ.x * 7.787f)+(16/116);
    XYZ.y = (XYZ.y>0.008856f) ? std::pow(XYZ.y, (1/3)) : (XYZ.y * 7.787f)+(16/116);
    XYZ.z = (XYZ.z>0.008856f) ? std::pow(XYZ.z, (1/3)) : (XYZ.z * 7.787f)+(16/116);

    float L = (116 * XYZ.y) - 16;
    float a = 500*(XYZ.x-XYZ.y);
    float b = 200*(XYZ.y-XYZ.z);

    return ImVec4(L,a,b,0);
}

std::string trucate_text(const std::string& p_text, float p_truncated_width) {
	std::string truncated_text = p_text;

	const float text_width =
			ImGui::CalcTextSize(p_text.c_str(), nullptr, true).x;

	if (text_width > p_truncated_width) {
		constexpr const char* ELLIPSIS = " ...";
		const float ellipsis_size = ImGui::CalcTextSize(ELLIPSIS).x;

		int visible_chars = 0;
		for (size_t i = 0; i < p_text.size(); i++) {
			const float current_width = ImGui::CalcTextSize(
					p_text.substr(0, i).c_str(), nullptr, true)
												.x;
			if (current_width + ellipsis_size > p_truncated_width) {
				break;
			}

			visible_chars = i;
		}

		truncated_text = (p_text.substr(0, visible_chars) + ELLIPSIS).c_str();
	}

	return truncated_text;
}

std::string return_current_time_and_date()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y.%m.%d %X");
    return ss.str();
}

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s) {
    return rtrim(ltrim(s));
}

std::string replace_string(std::string subject, const std::string& search, const std::string& replace) {
    size_t pos = 0;
    while((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return subject;
}

// /home/user/work/p4tools/src/project1.spr
// /home/user/wo.../project1.spr
std::string shrink_string(const std::string &subject, size_t max_length) {
    const std::string ELLIPSIS = "...";
    if(subject.length() <= max_length) return subject;
    float space_per_part = (max_length - ELLIPSIS.length()) * 0.5f;
    auto before = subject.substr(0, std::ceil(space_per_part));
    auto after = subject.substr(subject.length() - std::floor(space_per_part));
    return before + ELLIPSIS + after;
}

std::vector<std::string> split_string(const std::string &str, const std::string &delim) {
    std::regex del(delim);
    std::sregex_token_iterator it(str.begin(), str.end(), del, -1);
    std::sregex_token_iterator end;

    std::vector<std::string> v;
    while (it != end) {
        v.emplace_back(*it);
        ++it;
    }
    return v;
}

