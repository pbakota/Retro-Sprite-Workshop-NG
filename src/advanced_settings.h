#pragma once

struct AdvancedSettings
{
    const char *TITLE = "Advanced Settings - Code Generator";
    const std::string codePreviewTemplate = "[[LINE-COMMENT-SYMBOL]] This is a commented line\n"
        "\n"
        "[[CONSTANT-DECLARATION]]\n"
        "[[LABEL-DECLARATION]]\n"
        "\t\t[[BYTE-ARRAY]] $00, $00, $FF\n";

    char codePreview[256];

    AdvancedSettings() {}

    bool show(SpriteManager *spriteManager, bool *open) {
        if(!*open)
            return false;

        bool completed = false;
        auto preview = codePreviewTemplate;
        preview = replace_string(preview, "[[LINE-COMMENT-SYMBOL]]", spriteManager->lineCommentSymbol);
        preview = replace_string(preview, "[[BYTE-ARRAY]]", spriteManager->byteArrayType);

        // "{{NAME}} = {{VALUE}}"
        auto constant = replace_string(spriteManager->constantDeclaration, "{{NAME}}", "NUMBER_OF_ITEMS");
        constant = replace_string(constant, "{{VALUE}}", "6");
        preview = replace_string(preview, "[[CONSTANT-DECLARATION]]", constant);

        // "{{NAME}}"
        auto label = replace_string(spriteManager->labelDeclaration, "{{LABEL}}", "image_bytes");
        preview = replace_string(preview, "[[LABEL-DECLARATION]]", label);

        strncpy(codePreview, preview.c_str(), sizeof(codePreview));

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        ImGui::SetNextWindowSize(ImVec2(640.0f,330.0f));
        ImGui::OpenPopup(TITLE, ImGuiPopupFlags_None);
        if(ImGui::BeginPopupModal(TITLE, open, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            ImGui::SeparatorText("Assembly Language Compatibility");
            if (ImGui::BeginTable("#advancedSettings", 2, ImGuiTableFlags_NoBordersInBody))
            {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);

                ImGui::TableNextRow();
                ImGui::TableNextColumn(); ImGui::TextUnformatted("Line Comment Symbol");
                ImGui::TableNextColumn(); ImGui::PushID(0); ImGui::InputText("", spriteManager->lineCommentSymbol, IM_ARRAYSIZE(spriteManager->lineCommentSymbol)); ImGui::PopID();

                ImGui::TableNextRow();
                ImGui::TableNextColumn(); ImGui::TextUnformatted("Byte Array Type");
                ImGui::TableNextColumn(); ImGui::PushID(1); ImGui::InputText("", spriteManager->byteArrayType, IM_ARRAYSIZE(spriteManager->byteArrayType)); ImGui::PopID();

                ImGui::TableNextRow();
                ImGui::TableNextColumn(); ImGui::TextUnformatted("Constant Declaration");
                ImGui::TableNextColumn(); ImGui::PushID(2); ImGui::SetNextItemWidth(-1); ImGui::InputText("", spriteManager->constantDeclaration, IM_ARRAYSIZE(spriteManager->constantDeclaration)); ImGui::PopID();

                ImGui::TableNextRow();
                ImGui::TableNextColumn(); ImGui::TextUnformatted("Label Declaration");
                ImGui::TableNextColumn(); ImGui::PushID(3); ImGui::SetNextItemWidth(-1); ImGui::InputText("", spriteManager->labelDeclaration, IM_ARRAYSIZE(spriteManager->labelDeclaration)); ImGui::PopID();

                ImGui::TableNextRow();
                ImGui::TableNextColumn(); ImGui::TextUnformatted("Code Preview");

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TableNextColumn(); ImGui::PushID(4); ImGui::SetNextItemWidth(-1); ImGui::InputTextMultiline("", codePreview, IM_ARRAYSIZE(codePreview), ImVec2(0,0), ImGuiInputTextFlags_ReadOnly); ImGui::PopID();

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TableNextColumn();
                ImGui::Dummy(ImVec2(0, 20.0f));
                ImGui::PushID(5); if(ImGui::Button("OK", ImVec2(100,20))) {
                    completed = true;
                    *open = false;
                } ImGui::PopID(); ImGui::SameLine();
                ImGui::PushID(6); if(ImGui::Button("Cancel", ImVec2(100,20))) {
                    *open = false;
                } ImGui::PopID();
                ImGui::EndTable();
            }
            ImGui::EndPopup();
        }

        return completed;
    }
};