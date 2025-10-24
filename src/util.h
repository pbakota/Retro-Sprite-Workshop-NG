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
ImVec4 rgb_to_lab(const ImVec4 color);
ImVec4 rgb_to_hsv(const ImVec4 rgb);
ImU32 brighten_color(ImU32 color, float delta_v);

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

ImU32 grayscale_color(ImU32 color){
	  float r = ABGR_RED(color),g=ABGR_GREEN(color),b=ABGR_BLUE(color);
    int gray = static_cast<int>(0.2126*r+0.7152*g+0.0722*b)&0xff;
    return 0xff << 24 | (gray << 16) | (gray << 8) | gray;
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

ImVec4 rgb_to_hsv(const ImVec4 rgb) {
    ImVec4 hsv = {0.0f, 0.0f, 0.0f, 0.0f};
    
    float r = rgb.x;
    float g = rgb.y;
    float b = rgb.z;
    float max_val, min_val, delta;

    // Preserve Alpha (w)
    hsv.w = rgb.w; 

    max_val = fmaxf(r, fmaxf(g, b));
    min_val = fminf(r, fminf(g, b));
    delta = max_val - min_val;

    // V (Value) -> hsv.z
    hsv.z = max_val;

    // S (Saturation) -> hsv.y
    if (max_val == 0.0f) {
        hsv.y = 0.0f;
        hsv.x = 0.0f; // H is 0 for black/gray
        return hsv;
    }
    hsv.y = delta / max_val;

    // H (Hue) -> hsv.x
    if (delta == 0.0f) {
        hsv.x = 0.0f;
    } else {
        if (max_val == r) {
            hsv.x = 60.0f * fmodf((g - b) / delta, 6.0f);
        } else if (max_val == g) {
            hsv.x = 60.0f * ((b - r) / delta + 2.0f);
        } else { // max_val == b
            hsv.x = 60.0f * ((r - g) / delta + 4.0f);
        }
        if (hsv.x < 0.0f) {
            hsv.x += 360.0f;
        }
    }
    
    return hsv;
}

ImVec4 hsv_to_rgb(const ImVec4 hsv) {
    ImVec4 rgb = {0.0f, 0.0f, 0.0f, 0.0f};
    
    float h = hsv.x;
    float s = hsv.y;
    float v = hsv.z;
    float r, g, b;
    int i;
    float f, p, q, t;

    // Preserve Alpha (w)
    rgb.w = hsv.w; 

    if (s == 0.0f) {
        // Grayscale: R, G, B are all equal to V
        rgb.x = rgb.y = rgb.z = v;
        return rgb;
    }

    h /= 60.0f; 
    i = (int)floorf(h);
    f = h - i;

    p = v * (1.0f - s);
    q = v * (1.0f - s * f);
    t = v * (1.0f - s * (1.0f - f));

    // Assign R (x), G (y), B (z) based on the color sector
    switch (i % 6) {
        case 0: r = v; g = t; b = p; break; 
        case 1: r = q; g = v; b = p; break; 
        case 2: r = p; g = v; b = t; break; 
        case 3: r = p; g = q; b = v; break; 
        case 4: r = t; g = p; b = v; break; 
        case 5: r = v; g = p; b = q; break; 
        default: r = g = b = v; break;
    }
    
    rgb.x = r;
    rgb.y = g;
    rgb.z = b;
    
    return rgb;
}

// Dim a color by a factor, e.g. 0.5f will dim the color to half of its original brightness
ImU32 dim_color(ImU32 color, float factor) {
    float r = ABGR_RED(color),g=ABGR_GREEN(color),b=ABGR_BLUE(color);
    int dimR = static_cast<int>(r * factor)&0xff;
    int dimG = static_cast<int>(g * factor)&0xff;
    int dimB = static_cast<int>(b * factor)&0xff;
    return 0xff << 24 | (dimB << 16) | (dimG << 8) | dimR;
}

ImU32 brighten_color(ImU32 color, float delta_v) {
    float r = ABGR_RED(color),g=ABGR_GREEN(color),b=ABGR_BLUE(color);

    // 1. Convert RGB to HSV
    ImVec4 hsv = rgb_to_hsv(ImVec4(r/255.0f,g/255.0f,b/255.0f, 1.0f));

    // 2. Brighten: Increase V (stored in hsv.z) and clamp at 1.0
    hsv.z = fminf(hsv.z + delta_v, 1.0f);
    if (hsv.z == 1.0f) {
        // Decrease Saturation (hsv.y) by the same factor (delta_v)
        // Clamp Saturation at 0.0 (fully white/gray)
        hsv.y = fmaxf(hsv.y - delta_v, 0.0f);
    }
    // 3. Convert modified HSV back to RGB
    ImVec4 rgb = hsv_to_rgb(hsv);

    int briR = static_cast<int>(255.0f*rgb.x)&0xff;
    int briG = static_cast<int>(255.0f*rgb.y)&0xff;
    int briB = static_cast<int>(255.0f*rgb.z)&0xff;
    return 0xff << 24 | (briB << 16) | (briG << 8) | briR;
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
      p_text.substr(0, i).c_str(), nullptr, true).x;
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
    // matches any characters that need to be escaped in RegEx
    const std::regex specialChars { R"([-[\]{}()*+?.,\^$|#\s])" };
    const std::string sanitized_delim = std::regex_replace(delim, specialChars, R"(\$&)" );

    std::regex del(sanitized_delim);
    std::sregex_token_iterator it(str.begin(), str.end(), del, -1);
    std::sregex_token_iterator end;

    std::vector<std::string> v;
    while (it != end) {
        v.emplace_back(*it);
        ++it;
    }
    return v;
}
