#include "raylib.h"
#include <cmath>
#include <functional>
#include <stdio.h>
#include <string>
#include <string_view>
#include <vector>

#define RES_PATH(X) "./resources/" X

int window_width = 1200;
int window_height = 800;
int cursor = MOUSE_CURSOR_DEFAULT;

enum class FontMode
{
    NONE,
    BOLD
};

struct CustomFont
{
    float size;
    FontMode mode;
    Font font;
};

std::vector<CustomFont> font_pool;

Font get_font(float size, FontMode mode)
{
    for (const CustomFont &font : font_pool)
    {
        if (font.size == size && font.mode == mode)
        {
            return font.font;
        }
    }
    std::vector<int> codepoints;
    for (int i = 32; i <= 126; i++)
        codepoints.push_back(i);
    for (int i = 160; i <= 255; i++)
        codepoints.push_back(i);

    const char *font_path = "";
    if (mode == FontMode::NONE)
    {
        font_path = RES_PATH("consolas.ttf");
    }
    else if (mode == FontMode::BOLD)
    {
        font_path = RES_PATH("consolas_bold.ttf");
    }

    CustomFont custom_font;
    custom_font.size = size;
    custom_font.mode = mode;
    custom_font.font = LoadFontEx(font_path, size, codepoints.data(), codepoints.size());
    font_pool.push_back(custom_font);
    return custom_font.font;
}

void Frame(Rectangle rect, Color frame_color, float frame_width = 1.0f)
{
    if (frame_width > 0.0f)
    {
        DrawRectangle(rect.x, rect.y, rect.width, frame_width, frame_color);
        DrawRectangle(rect.x, rect.y, frame_width, rect.height, frame_color);
        DrawRectangle(rect.x, rect.y + rect.height - frame_width, rect.width, frame_width, frame_color);
        DrawRectangle(rect.x + rect.width - frame_width, rect.y, frame_width, rect.height, frame_color);
    }
}

bool Button(Rectangle rect, const char *text = "Default", bool selected = false)
{
    const float font_size = 20.0f;
    const Font font = get_font(font_size, FontMode::BOLD);
    const float frame_width = 1.0f;

    const bool hovered = CheckCollisionPointRec(GetMousePosition(), rect);
    const bool clicked = hovered && (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE));

    Color color = BLACK;
    Color font_color = WHITE;
    if (selected || hovered)
    {
        color = WHITE;
        font_color = BLACK;
    }

    if (hovered)
    {
        cursor = MOUSE_CURSOR_POINTING_HAND;
    }

    DrawRectangleRec(rect, color);
    Frame(rect, WHITE, frame_width);

    // Draw Text
    const Vector2 text_size = MeasureTextEx(font, text, font_size, 0);
    Vector2 text_pos;
    text_pos.x = rect.x + rect.width / 2 - text_size.x / 2;
    text_pos.y = rect.y + rect.height / 2 - text_size.y / 2 + 3.0f;
    DrawTextEx(font, text, text_pos, font_size, 0, font_color);
    return clicked;
}

std::vector<std::string_view> compute_lines(const char *text)
{
    std::vector<std::string_view> lines;

    if (!text)
        return lines;

    const char *line_begin = text;

    while (*text)
    {
        if (*text == '\n')
        {
            lines.emplace_back(line_begin, text - line_begin);
            ++text;
            line_begin = text;
        }
        else
        {
            ++text;
        }
    }

    lines.emplace_back(line_begin, text - line_begin);
    return lines;
}

std::vector<std::string_view> compute_bounded_lines(const char *text, size_t num_char_per_line)
{
    std::vector<std::string_view> bounded_lines;
    for (const std::string_view &line : compute_lines(text))
    {
        size_t i = 0;
        while (line.size() - i > num_char_per_line)
        {
            size_t tmp_j = std::min(i + num_char_per_line, line.size() - 1);
            size_t j = tmp_j;
            while (line[j] != ' ' && j > i)
            {
                --j;
            }
            if (j == i)
            {
                bounded_lines.push_back(line.substr(i, tmp_j - i));
                i = tmp_j;
            }
            else
            {
                bounded_lines.push_back(line.substr(i, j - i));
                i = j + 1;
            }
        }
        bounded_lines.push_back(line.substr(i, line.size() - i));
    }
    return bounded_lines;
}

struct TextStyle
{
    float font_size;
    FontMode font_mode;
    Color font_color;
    float text_box_width;
    float right_margin;
};

constexpr TextStyle style_application_title = {
    .font_size = 38.0f,
    .font_mode = FontMode::BOLD,
    .font_color = WHITE,
    .text_box_width = 1920.0f,
    .right_margin = 40.0f};

constexpr TextStyle style_application_subtitle = {
    .font_size = 16.0f,
    .font_mode = FontMode::NONE,
    .font_color = GRAY,
    .text_box_width = 1920.0f,
    .right_margin = 40.0f};

constexpr TextStyle style_page_title = {
    .font_size = 28.0f,
    .font_mode = FontMode::BOLD,
    .font_color = WHITE,
    .text_box_width = 1920.0f,
    .right_margin = 40.0f};

constexpr TextStyle style_profile_path = {
    .font_size = 20.0f,
    .font_mode = FontMode::NONE,
    .font_color = GRAY,
    .text_box_width = 1920.0f,
    .right_margin = 40.0f};

constexpr TextStyle style_profile_name = {
    .font_size = 20.0f,
    .font_mode = FontMode::BOLD,
    .font_color = WHITE,
    .text_box_width = 1920.0f,
    .right_margin = 40.0f};

constexpr TextStyle style_profile_package = {
    .font_size = 16.0f,
    .font_mode = FontMode::BOLD,
    .font_color = WHITE,
    .text_box_width = 1920.0f,
    .right_margin = 40.0f};

// Just used to draw text from string view
void DrawTextView(Font font, const std::string_view &text, Vector2 position, float fontSize, float spacing, Color tint)
{
    float textOffsetX = 0.0f;
    float textOffsetY = 0.0f;

    float scaleFactor = fontSize / font.baseSize;

    for (size_t i = 0; i < text.size();)
    {
        int codepointByteCount = 0;
        int codepoint = GetCodepointNext(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        if (codepoint == '\n')
        {
            textOffsetY += (fontSize + 2.0f);
            textOffsetX = 0.0f;
        }
        else
        {
            if ((codepoint != ' ') && (codepoint != '\t'))
            {
                DrawTextCodepoint(font, codepoint, {position.x + textOffsetX, position.y + textOffsetY}, fontSize, tint);
            }

            if (font.glyphs[index].advanceX == 0)
            {
                textOffsetX += ((float)font.recs[index].width * scaleFactor + spacing);
            }
            else
            {
                textOffsetX += ((float)font.glyphs[index].advanceX * scaleFactor + spacing);
            }
        }

        i += codepointByteCount;
    }
}

float TextBox(Vector2 position, const char *text, const TextStyle &style)
{
    const Font font = get_font(style.font_size, style.font_mode);
    const Vector2 letter_size = MeasureTextEx(font, "W", style.font_size, 0);
    const float letter_width = letter_size.x;
    const float letter_height = letter_size.y;

    const float imposed_width = window_width - position.x - style.right_margin;
    const float box_width = std::min(style.text_box_width, imposed_width);
    size_t num_char_per_line = std::floor(box_width / letter_width);
    if (num_char_per_line == 0)
    {
        num_char_per_line = 1000000;
    }

    std::vector<std::string_view> displayed_text = compute_bounded_lines(text, num_char_per_line);

    const float x = position.x;
    float y = position.y;
    float max_width = 0.0f;

    for (const std::string_view &line : displayed_text)
    {
        max_width = std::max(max_width, line.size() * letter_width);
        if (CheckCollisionPointRec(GetMousePosition(), {x, y, line.size() * letter_width, letter_height}))
        {
            cursor = MOUSE_CURSOR_IBEAM;
        }
        DrawTextView(font, line, {x, y}, style.font_size, 0, style.font_color);
        y += letter_height;
    }

    const float textbox_height = displayed_text.size() * letter_height;
    // Frame({x, position.y, max_width, textbox_height}, PINK); // debug rect
    return textbox_height;
}

std::string runCommand(const char *command)
{
    FILE *pipe = popen(command, "r");
    char buffer[4096];
    std::string result;

    while (fgets(buffer, sizeof(buffer), pipe))
    {
        result += buffer;
    }

    pclose(pipe);
    return result;
}

struct ProfileStruct
{
    std::string folder_path;
    std::string name;
    std::vector<std::string> packages;
};

std::vector<ProfileStruct> profiles;

float draw_category_page_profiles(float y)
{
    const float initial_y = y;
    y += 20.0f;
    y += TextBox({40.0f, y}, "Profiles", style_page_title);

    y -= 20.0f;
    for (std::vector<ProfileStruct>::iterator it = profiles.begin(); it != profiles.end(); ++it)
    {
        ProfileStruct &profile = *it;
        y += 40.0f;
        y += TextBox({40.0f, y}, profile.name.c_str(), style_profile_name);
        y += 10.0f;
        y += TextBox({40.0f, y}, profile.folder_path.c_str(), style_profile_path);
        y += 10.0f;

        if (profile.packages.size() > 0)
        {
            if (profile.packages.size() == 1)
            {
                y += TextBox({40.0f, y}, "Installed package:", style_profile_name);
            }
            else
            {
                y += TextBox({40.0f, y}, "Installed packages:", style_profile_name);
            }
            y += 10.0f;
            for (std::vector<std::string>::iterator pack_it = profile.packages.begin(); pack_it != profile.packages.end(); ++pack_it)
            {
                const std::string &package_name = *pack_it;
                if (Button({40.0f, y - 4.0f, 20.0f, 20.0f}, "x"))
                {
                    const std::string command = "guix package -p " + profile.folder_path + "-q -r " + package_name;
                    runCommand(command.c_str());
                    profile.packages.erase(pack_it);
                }

                y += TextBox({70.0f, y}, ("- " + package_name).c_str(), style_profile_package);
                y += 10.0f;
            }
            y += 10.0f;
        }

        if (Button({40.0f, y, 100.0f, 50.0f}, "Delete"))
        {
            const std::string command = "rm " + profile.folder_path + " " + profile.folder_path + "-*-link";
            runCommand(command.c_str());
            profiles.erase(it);
        }
        y += 50.0f;
    }

    return initial_y - y;
}

float draw_category_page_packages(float y)
{
    const float initial_y = y;
    y += 20.0f;
    y += TextBox({40.0f, y}, "Packages", style_page_title);
    return initial_y - y;
}

float draw_category_page_store(float y)
{
    const float initial_y = y;
    y += 20.0f;
    y += TextBox({40.0f, y}, "Store", style_page_title);
    return initial_y - y;
}

static std::function<float(float)> draw_current_page = draw_category_page_profiles;

bool operator==(const std::function<float(float)> &a, const std::function<float(float)> &b)
{
    return *a.target<float (*)(float)>() == *b.target<float (*)(float)>();
}

float NavBar(float y)
{
    const float button_height = 50.0f;
    const float button_width = window_width / 3.0f;
    Rectangle rect = {0.0f, y, button_width, button_height};

    if (Button(rect, "Profiles", draw_current_page == draw_category_page_profiles))
    {
        draw_current_page = draw_category_page_profiles;
    }
    rect.x += button_width;

    if (Button(rect, "Packages", draw_current_page == draw_category_page_packages))
    {
        draw_current_page = draw_category_page_packages;
    }
    rect.x += button_width;

    if (Button(rect, "Store", draw_current_page == draw_category_page_store))
    {
        draw_current_page = draw_category_page_store;
    }

    return button_height;
}

std::vector<std::string> get_installed_package(const std::string profile_path)
{
    std::vector<std::string> installed_package;
    const std::string command = "guix package -p " + profile_path + " --list-installed";
    const std::string packages_str = runCommand(command.c_str());

    const char *packages_raw = packages_str.data();
    size_t end = 0;
    while (packages_raw[end])
    {
        const size_t name_begin = end;
        size_t name_end = 0;
        while (packages_raw[end] != '\n' && packages_raw[end] != '\0')
        {
            if (name_end == 0 && packages_raw[end] == '\t')
            {
                name_end = end;
            }
            ++end;
        }

        installed_package.push_back(packages_str.substr(name_begin, name_end - name_begin));
        ++end;
    }

    return installed_package;
}

void init_profiles()
{
    profiles.clear();
    const std::string profiles_str = runCommand("guix package --list-profiles");
    const char *profiles_raw = profiles_str.data();
    size_t begin = 0;
    size_t end = 0;
    while (profiles_raw[end])
    {
        size_t name_pos = 0;
        while (profiles_raw[end] != '\n' && profiles_raw[end] != '\0')
        {
            if (profiles_raw[end] == '/')
            {
                name_pos = end + 1;
            }
            ++end;
        }

        const std::string path = profiles_str.substr(begin, end - begin);
        const std::string name = profiles_str.substr(name_pos, end - name_pos);
        profiles.push_back({path, name, get_installed_package(path)});
        end++;
        begin = end;
    }
}

int main()
{
    InitWindow(window_width, window_height, "GuiGuix");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    std::string guix_version = runCommand("guix --version | head --lines=1");
    bool guix_exists = true;
    if (guix_version.substr(0, 4) != "guix")
    {
        guix_version = "GUIX COMMAND NOT FOUND";
        guix_exists = false;
    }

    init_profiles();

    while (!WindowShouldClose())
    {
        if (IsWindowResized())
        {
            window_width = GetScreenWidth();
            window_height = GetScreenHeight();
        }

        cursor = MOUSE_CURSOR_DEFAULT;

        BeginDrawing();

        ClearBackground(BLACK);

        float y = 0.0f;
        y += 40.0f;
        y += TextBox({40.0f, y}, "GuiGuix", style_application_title);
        y += 5.0f;
        y += TextBox({40.0f, y}, guix_version.c_str(), style_application_subtitle);

        if (!guix_exists)
        {
            EndDrawing();
            continue;
        }

        y += NavBar(y);
        draw_current_page(y);

        SetMouseCursor(cursor);

        EndDrawing();
    }

    for (const CustomFont &custom_font : font_pool)
    {
        UnloadFont(custom_font.font);
    }

    CloseWindow();

    return 0;
}