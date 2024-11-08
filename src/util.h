#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>

// ABGR
#define ABGR_BLUE(color) ((color>>16)&0xff)
#define ABGR_GREEN(color) ((color>>8)&0xff)
#define ABGR_RED(color) ((color)&0xff)

const std::string vformat(const char *const zcFormat, ...);
std::string trucate_text(const std::string &p_text, float p_truncated_width);
std::string return_current_time_and_date();
bool is_light_color(ImU32 color);

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
    float space_per_part = (max_length - ELLIPSIS.length()) / 2.0f;
    auto before = subject.substr(0, std::ceil(space_per_part));
    auto after = subject.substr(subject.length() - std::floor(space_per_part));
    return before + ELLIPSIS + after;
}