/*
	Copyright 2020 Limeoats
	Original project: https://github.com/Limeoats/L2DFileDialog

	Changes by Vladimir Sigalkin
	Original project: https://github.com/Iam1337/ImGui-FileDialog

	Changes by Tom Stanczyk

	Changes by Peter Bakota
	Original project: https://github.com/74Labs/ImGuiFileDialog
*/

#ifndef IMGUI_FILEDIALOG_H
#define IMGUI_FILEDIALOG_H

#include <filesystem>
#include <vector>
#include <imgui.h>

typedef int ImGuiFileDialogType; // -> enum ImGuiFileDialogType_        // Enum: A file dialog type
typedef int ImGuiFileDialogFlags;

enum ImGuiFileDialogType_
{
	ImGuiFileDialogType_OpenFile,
	ImGuiFileDialogType_SaveFile,
	ImGuiFileDialogType_COUNT
};

enum ImGuiFileDialogFlags_
{
	ImGuiFileDialogFlags_None = (0),
	ImGuiFileDialogFlags_FileMustExist = (1<<0),
};

struct ImFileDialogInfo
{
	std::string title;
	ImGuiFileDialogType type;
	ImGuiFileDialogFlags flags;

	std::filesystem::path fileName;
	std::filesystem::path directoryPath;
	std::filesystem::path resultPath;

	bool refreshInfo;
	size_t currentIndex;
	std::vector<std::filesystem::directory_entry> currentFiles;
	std::vector<std::filesystem::directory_entry> currentDirectories;
};

namespace ImGui
{
	IMGUI_API bool FileDialog(bool *open, ImFileDialogInfo *dialogInfo);
}

#endif