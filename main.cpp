#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")

int main() {

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(1750, 1000)), "Mandelbrot set");
    sf::RectangleShape screen(sf::Vector2f(1750, 1000));
    
#ifdef _WIN32
    HWND hwnd = window.getNativeHandle();
    BOOL dark = TRUE;
    COLORREF titleBarColor = 0x00000000;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
    DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &titleBarColor, sizeof(titleBarColor));
#endif

    window.setVerticalSyncEnabled(true);

    double zoom = 1.0;
    sf::Vector2f offset(0.0f, 0.0f);
    double edgeParam = 4.0;
    sf::Vector2f initial_z(0, 0);
    int iterCount = 1000;
    float complexPlaneRuleQ1 = 2.f;
    float complexPlaneRuleQ2 = 1.f;

    sf::Clock fpsClock;
    sf::Shader shader;
    sf::Font font;
    float fps = 0.f;

    font.openFromFile("0xProtoNerdFont-Regular.ttf");

    sf::Text overlay(font, "", 13);
    overlay.setFillColor(sf::Color::White);
    overlay.setOutlineColor(sf::Color::Black);
    overlay.setOutlineThickness(1.f);
    overlay.setPosition(sf::Vector2f(10.f, 10.f));


    while (window.isOpen()) {

        while (const std::optional<sf::Event> event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* mouseWheel = event->getIf<sf::Event::MouseWheelScrolled>()) {

                float factor = (mouseWheel->delta > 0) ? 1.1f : 0.9f;

                sf::Vector2u size = window.getSize();
                sf::Vector2i mousePos = mouseWheel->position;
                sf::Vector2f before = {
                    static_cast<float>((mousePos.x / (float) size.x - 0.5f)*3.5  / zoom + offset.x),
                    static_cast<float>((mousePos.y / (float) size.y - 0.5f)*2.f / zoom + offset.y)
                };
                zoom *= factor;
                sf::Vector2f after = {
                    static_cast<float>((mousePos.x / (float) size.x - 0.5f)*3.5 / zoom + offset.x),
                    static_cast<float>((mousePos.y / (float) size.y - 0.5f)*2.f / zoom + offset.y)
                };
                offset.x += before.x - after.x;
                offset.y -= before.y - after.y;
            }
        }

        const float panSpeed = 0.01f / zoom;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) offset.x += panSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) offset.x -= panSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) offset.y += panSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) offset.y -= panSpeed;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))   edgeParam += 0.1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) edgeParam -= 0.1;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) initial_z.x += 0.002f / zoom;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F)) initial_z.x -= 0.002f / zoom;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::T)) initial_z.y += 0.002f / zoom;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::G)) initial_z.y -= 0.002f / zoom;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Y)) complexPlaneRuleQ1 += 0.01f/ zoom;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::H)) complexPlaneRuleQ1 -= 0.01f/ zoom;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::U)) complexPlaneRuleQ2 += 0.01f/ zoom;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::J)) complexPlaneRuleQ2 -= 0.01f/ zoom;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) iterCount += 2;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) iterCount -= 2;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Backspace)) {
            zoom = 1.0;
            offset = {0.0f, 0.0f};
            edgeParam = 4.0;
            initial_z = {0, 0};
            iterCount = 1000;
            complexPlaneRuleQ1 = 2.f;
            complexPlaneRuleQ2 = 1.f;
        };

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F2)) {
            sf::Texture screenshot = sf::Texture(window.getSize());
            screenshot.update(window);
            screenshot.copyToImage().saveToFile("screenshot.png");
        }

        shader.setUniform("u_resolution", screen.getSize());
        shader.setUniform("u_initial_z", sf::Glsl::Vec2(initial_z));
        shader.setUniform("u_offset",     sf::Glsl::Vec2(offset));
        shader.setUniform("u_zoom",       (float)zoom);
        shader.setUniform("u_edgeParam", (float)edgeParam);
        shader.setUniform("u_iter",   iterCount);
        shader.setUniform("u_complexPlaneRuleQ1",   complexPlaneRuleQ1);
        shader.setUniform("u_complexPlaneRuleQ2",   complexPlaneRuleQ2);

        fps = 1.f / fpsClock.restart().asSeconds();

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(3)
           << "FPS:                    " << std::setprecision(1) << fps        << "\n"
           << "zoom:                   " << std::setprecision(6) << zoom        << "\n"
           << "(WASD) offset:           " << offset.x << ", " << offset.y        << "\n"
           << "(UP DOWN) edgeParam:     " << edgeParam                           << "\n"
           << "(x: FR y: TG) initial_z: " << initial_z.x << ", " << initial_z.y << "\n"
           << "(QE) iterCount:          " << iterCount                           << "\n"
           << "(HY) complexPlaneRuleQ1: " << complexPlaneRuleQ1 << "\n"
           << "(JU) complexPlaneRuleQ2: " << complexPlaneRuleQ2 << "\n"
           << "(Backspace): reset " << "\n"
           << "(F2): screenshot ";

        overlay.setString(ss.str());

        window.clear();

        shader.loadFromFile("mandelbrotShader.frag", sf::Shader::Type::Fragment);
        window.draw(screen, &shader);
        window.draw(overlay);

        window.display();
    }
}
