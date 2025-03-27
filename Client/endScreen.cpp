#include "endScreen.h"


bool EndScreen::isActiveEND = true;

EndScreen::EndScreen(SDL_Renderer* rendererSdl)
	: rendererSdl(rendererSdl),backgroundTexture(nullptr),
	exitButtonTexture(nullptr),
	exitButtonHoverTexture(nullptr)
{
}

EndScreen::~EndScreen()
{
    clean();
}
void EndScreen::init()
{
    if (!initialized) {
        backgroundTexture = loadTexture("assets/gameOver.png");
        exitButtonTexture = loadTexture("assets/Return1.png");
        exitButtonHoverTexture = loadTexture("assets/Return2.png");

        if (!backgroundTexture || !exitButtonTexture || !exitButtonHoverTexture) {
            std::cerr << "Failed to load textures!" << std::endl;
        }

        if (TTF_Init() == -1) {
            std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
            return;
        }

        int windowWidth, windowHeight;
        SDL_GetRendererOutputSize(rendererSdl, &windowWidth, &windowHeight);

        int buttonWidth = 200;
        int buttonHeight = 50;

        exitButtonRect = { 10, 10, 50, 50 };

        initialized = true;
    }
}
std::unique_ptr<SDL_Texture, SDLDeleterES> EndScreen::loadTexture(const char* filePath)
{
    SDL_Surface* surface = IMG_Load(filePath);
    if (!surface) {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
        return nullptr;
    }
    auto texture = std::unique_ptr<SDL_Texture, SDLDeleterES>(
        SDL_CreateTextureFromSurface(rendererSdl, surface)
    );
    SDL_FreeSurface(surface);
    return texture;
}
void EndScreen::handleEvents()
{
    SDL_Event event;
    SDL_PollEvent(&event);
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    switch (event.type) {
    case SDL_QUIT:
        isActiveEND = false;
        Game::isRunning = false;
        break;

    case SDL_MOUSEMOTION:
        if (mouseX >= exitButtonRect.x && mouseX <= exitButtonRect.x + exitButtonRect.w &&
            mouseY >= exitButtonRect.y && mouseY <= exitButtonRect.y + exitButtonRect.h) {
            buttonHovered = true;
        }
        else {
            buttonHovered = false;
        }
        break;

    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
            if (buttonHovered) {
                MainMenu::isActive = true;
                EndScreen::isActiveEND = false;
            }
        }
        break;
    }
}
void EndScreen::render()
{
    SDL_SetRenderDrawColor(rendererSdl, 83, 62, 0, 255);
    SDL_RenderClear(rendererSdl);

    if (backgroundTexture) {
        int windowWidth, windowHeight;
        SDL_GetRendererOutputSize(rendererSdl, &windowWidth, &windowHeight);
        SDL_Rect bgRect = { 0, 0, windowWidth, windowHeight };
        SDL_RenderCopy(rendererSdl, backgroundTexture.get(), nullptr, &bgRect);
    }

    if (buttonHovered) {
        SDL_RenderCopy(rendererSdl, exitButtonHoverTexture.get(), nullptr, &exitButtonRect);
    }
    else {
        SDL_RenderCopy(rendererSdl, exitButtonTexture.get(), nullptr, &exitButtonRect);
    }

    SDL_RenderPresent(rendererSdl);
}
void EndScreen::clean()
{
    backgroundTexture.reset();
    exitButtonTexture.reset();
    exitButtonHoverTexture.reset();
}