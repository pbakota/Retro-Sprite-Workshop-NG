#pragma once

struct AdvancedSettings
{
    const char *TITLE = "Advanced Settings - Code Generator";
    char lineCommentSymbol[8] = ";";
    char byteArrayType[8] = "byt";
    char constantDeclaration[128] = "{{NAME}} = {{VALUE}}";
    char codePreview[1024] = "; This is a commented line\n"
        "\n"
        "NUMBER_OF_ITEMS = 6\n"
        "image_bytes\n"
        "\t\tbyt $00, $00, $FF\n";

    bool show(bool *open) {
        if(!*open)
            return false;

        bool completed = false;

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        ImGui::SetNextWindowSize(ImVec2(640.0f,310.0f));
        ImGui::OpenPopup(TITLE, ImGuiPopupFlags_None);
        if(ImGui::BeginPopupModal(TITLE, open, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            ImGui::SeparatorText("Assembly Language Compatibility");
            if (ImGui::BeginTable("#advancedSettings", 2, ImGuiTableFlags_NoBordersInBody))
            {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);

                ImGui::TableNextRow();
                ImGui::TableNextColumn(); ImGui::TextUnformatted("Line Comment Symbol");
                ImGui::TableNextColumn(); ImGui::PushID(0); ImGui::InputText("", lineCommentSymbol, IM_ARRAYSIZE(lineCommentSymbol)); ImGui::PopID();

                ImGui::TableNextRow();
                ImGui::TableNextColumn(); ImGui::TextUnformatted("Byte Array Type");
                ImGui::TableNextColumn(); ImGui::PushID(1); ImGui::InputText("", byteArrayType, IM_ARRAYSIZE(byteArrayType)); ImGui::PopID();

                ImGui::TableNextRow();
                ImGui::TableNextColumn(); ImGui::TextUnformatted("Constant Declaration");
                ImGui::TableNextColumn(); ImGui::PushID(2); ImGui::SetNextItemWidth(-1); ImGui::InputText("", constantDeclaration, IM_ARRAYSIZE(constantDeclaration)); ImGui::PopID();

                ImGui::TableNextRow();
                ImGui::TableNextColumn(); ImGui::TextUnformatted("Code Preview");

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TableNextColumn(); ImGui::PushID(3); ImGui::SetNextItemWidth(-1); ImGui::InputTextMultiline("", codePreview, IM_ARRAYSIZE(codePreview)); ImGui::PopID();

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TableNextColumn();
                ImGui::Dummy(ImVec2(0, 20.0f));
                ImGui::PushID(4); if(ImGui::Button("OK", ImVec2(100,20))) {
                    completed = true;
                    *open = false;
                } ImGui::PopID(); ImGui::SameLine();
                ImGui::PushID(5); if(ImGui::Button("Cancel", ImVec2(100,20))) {
                    *open = false;
                } ImGui::PopID();
                ImGui::EndTable();
            }
            ImGui::EndPopup();
        }

        return completed;
    }
};