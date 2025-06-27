#pragma GCC diagnostic ignored "-Wunused-function"

void centredElement(int element, const char* text, float height = 0){

ImVec2 window_size = ImGui::GetWindowSize();
ImVec2 window_pos = ImGui::GetWindowPos();

if(element == 0){
ImVec2 text_size = ImGui::CalcTextSize(text);

// Рассчитываем позицию для центрирования
ImVec2 centered_pos = ImVec2(
    (window_pos.x + (window_size.x - text_size.x) * 0.5f), // Центр по X
    (window_pos.y + (window_size.y - text_size.y - height) * 0.5f)  // Центр по Y
);

// Передвигаем курсор ImGui в центр
ImGui::SetCursorPos(centered_pos);

ImGui::Text("%s", text);
}
if(element == 1){
ImVec2 text_size = ImGui::CalcTextSize(text);

// Рассчитываем позицию для центрирования
ImVec2 centered_pos = ImVec2(
    (window_pos.x + (window_size.x - text_size.x) * 0.5f), // Центр по X
    (window_pos.y + (window_size.y - text_size.y - height) * 0.5f)  // Центр по Y
);

// Передвигаем курсор ImGui в центр
ImGui::SetCursorPos(centered_pos);

ImGui::Button(text);
}
}

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}




void SetupImGuiGreen() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Основной цвет интерфейса - неоново-зелёный
    ImVec4 neonGreen = ImVec4(0.0f, 1.0f, 0.0f, 0.8f);  // RGBA, с полупрозрачностью
    ImVec4 backgroundColor = ImVec4(0.0f, 0.0f, 0.0f, 0.6f); // Тёмный фон, полупрозрачный

    // Устанавливаем цвета элементов
    colors[ImGuiCol_WindowBg]            = backgroundColor;      // Фон окна
    colors[ImGuiCol_Border]              = neonGreen;            // Границы
    colors[ImGuiCol_FrameBg]             = ImVec4(0.0f, 0.5f, 0.0f, 0.7f);  // Фон фреймов
    colors[ImGuiCol_FrameBgHovered]      = neonGreen;            // Ховер по фрейму
    colors[ImGuiCol_FrameBgActive]       = neonGreen;            // Активный фрейм
    colors[ImGuiCol_TitleBg]             = ImVec4(0.0f, 0.3f, 0.0f, 0.8f);  // Фон заголовка окна
    colors[ImGuiCol_TitleBgActive]       = neonGreen;            // Активный заголовок
    colors[ImGuiCol_TitleBgCollapsed]    = backgroundColor;      // Свернутое окно
    colors[ImGuiCol_Button]              = ImVec4(0.0f, 0.8f, 0.0f, 0.7f);  // Кнопка
    colors[ImGuiCol_ButtonHovered]       = neonGreen;            // Ховер по кнопке
    colors[ImGuiCol_ButtonActive]        = neonGreen;            // Активная кнопка
    colors[ImGuiCol_Text]                = neonGreen;            // Текст
    colors[ImGuiCol_CheckMark]           = neonGreen;            // Чекбокс
    colors[ImGuiCol_SliderGrab]          = neonGreen;            // Ползунок
    colors[ImGuiCol_SliderGrabActive]    = neonGreen;            // Активный ползунок
    colors[ImGuiCol_ResizeGrip]          = neonGreen;            // Угловой ресайз
    colors[ImGuiCol_ResizeGripHovered]   = neonGreen;            // Ховер по ресайзу
    colors[ImGuiCol_ResizeGripActive]    = neonGreen;            // Активный ресайз

    style.Alpha = 0.9f;           // Глобальная прозрачность (0.0 - полностью прозрачный, 1.0 - полностью непрозрачный)
}