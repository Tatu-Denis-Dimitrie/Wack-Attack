#include "loginScreen.h"
import cout;


LoginScreen::LoginScreen(SDL_Renderer* rendererSdl)
	: rendererSdl(rendererSdl), isActive(true), isLoggedIn(false), font(nullptr), inputText(""),
	submitButtonTexture(nullptr), submitButtonHoverTexture(nullptr),
	backgroundTexture(nullptr), inputTextTexture(nullptr), userNameLocked(false)
{
}

LoginScreen::~LoginScreen()
{
	clean();
}

void LoginScreen::init()
{
	if (!initialized) {
		backgroundTexture = loadTexture("assets/background.png");
		submitButtonTexture = loadTexture("assets/Login.png");
		submitButtonHoverTexture = loadTexture("assets/Login2.png");

		if (!backgroundTexture || !submitButtonTexture || !submitButtonHoverTexture) {
			mycerr::display("Failed to load textures!", mycerr::Color::Red);
		}

		if (TTF_Init() == -1) {
			mycerr::display("SDL_ttf initialization failed!", mycerr::Color::Red);
			return;
		}

		font = std::unique_ptr<TTF_Font, SDLDeleterLS>(TTF_OpenFont("assets/OpenSans-Regular.ttf", 24));
		if (!font) {
			std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
			return;
		}

		int windowWidth, windowHeight;
		SDL_GetRendererOutputSize(rendererSdl, &windowWidth, &windowHeight);

		int buttonWidth = 200;
		int buttonHeight = 50;

		inputBoxRect = { (windowWidth - 300) / 2, (windowHeight - 50) / 2, 300, 50 };
		submitButtonRect = { (windowWidth - buttonWidth) / 2, inputBoxRect.y + 70, buttonWidth, buttonHeight };

		initialized = true;
	}
	audioLS.Init();
}

std::unique_ptr<SDL_Texture, SDLDeleterLS> LoginScreen::loadTexture(const char* filePath) {
	SDL_Surface* surface = IMG_Load(filePath);
	if (!surface) {
		mycerr::display("Failed to load texture!", mycerr::Color::Red);
		return nullptr;
	}
	auto texture = std::unique_ptr<SDL_Texture, SDLDeleterLS>(
		SDL_CreateTextureFromSurface(rendererSdl, surface)
	);
	SDL_FreeSurface(surface);
	return texture;
}

void LoginScreen::handleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	switch (event.type) {
	case SDL_QUIT:
		isActive = false;
		Game::isRunning = false;
		break;

	case SDL_MOUSEMOTION:
		if (mouseX >= submitButtonRect.x && mouseX <= submitButtonRect.x + submitButtonRect.w &&
			mouseY >= submitButtonRect.y && mouseY <= submitButtonRect.y + submitButtonRect.h) {
			buttonHovered = true;
		}
		else {
			buttonHovered = false;
		}
		break;

	case SDL_TEXTINPUT:
		if (!userNameLocked && inputText.size() < 20) {
			std::string inputChar(1, event.text.text[0]);
			if (std::regex_match(inputChar, validUsernameRegex)) {
				inputText += event.text.text;
				updateInputTextTexture();
			}
		}
		break;

	case SDL_KEYDOWN:
		if (!userNameLocked && event.key.keysym.sym == SDLK_BACKSPACE && !inputText.empty()) {
			inputText.pop_back();
			updateInputTextTexture();
		}
		else if (event.key.keysym.sym == SDLK_RETURN) {
			audioLS.PlaySoundEffect("assets/duckPlay.mp3");
			submitLogin();
		}
		break;

	case SDL_MOUSEBUTTONDOWN:
		if (event.button.button == SDL_BUTTON_LEFT) {
			if (buttonHovered) {
				submitLogin();
				audioLS.PlaySoundEffect("assets/duckPlay.mp3");
			}
		}
		break;
	}
}

void LoginScreen::updateInputTextTexture()
{
	inputTextTexture.reset();

	if (!inputText.empty()) {
		SDL_Surface* textSurface = TTF_RenderText_Solid(font.get(), inputText.c_str(), { 0, 0, 0 });
		if (!textSurface) {
			mycerr::display("Failed to render text surface!", mycerr::Color::Red);
			return;
		}
		inputTextTexture = std::unique_ptr<SDL_Texture, SDLDeleterLS>(
			SDL_CreateTextureFromSurface(rendererSdl, textSurface)
		);
		SDL_FreeSurface(textSurface);
	}
}

void LoginScreen::submitLogin()
{
	if (inputText.empty()) {
		mycerr::display("Username is empty!", mycerr::Color::Red);
		return;
	}

	auto response = cpr::Post(cpr::Url{ SERVER_URL + "/login" },
		cpr::Body{ "{\"username\":\"" + inputText + "\"}" },
		cpr::Header{ {"Content-Type", "application/json"} });

	if (response.status_code == 200) {
		mycout::display("Login successful!", mycout::Color::Cyan);
		username = inputText;
		isLoggedIn = true;
		isActive = false;
	}
	else {
		mycerr::display("Login failed!", mycerr::Color::Red);
	}
}
std::string LoginScreen::getUsername() const {
	return username;
}

void LoginScreen::render()
{
	SDL_SetRenderDrawColor(rendererSdl, 0, 0, 0, 255);
	SDL_RenderClear(rendererSdl);

	if (backgroundTexture) {
		int windowWidth, windowHeight;
		SDL_GetRendererOutputSize(rendererSdl, &windowWidth, &windowHeight);
		SDL_Rect bgRect = { 0, 0, windowWidth, windowHeight };
		SDL_RenderCopy(rendererSdl, backgroundTexture.get(), nullptr, &bgRect);
	}
	SDL_SetRenderDrawColor(rendererSdl, 169, 169, 169, 255);
	SDL_RenderFillRect(rendererSdl, &inputBoxRect);

	if (inputTextTexture) {
		int textWidth, textHeight;
		SDL_QueryTexture(inputTextTexture.get(), nullptr, nullptr, &textWidth, &textHeight);
		SDL_Rect textRect = { inputBoxRect.x + 10, inputBoxRect.y + (inputBoxRect.h - textHeight) / 2, textWidth, textHeight };
		SDL_RenderCopy(rendererSdl, inputTextTexture.get(), nullptr, &textRect);
	}

	if (buttonHovered) {
		SDL_RenderCopy(rendererSdl, submitButtonHoverTexture.get(), nullptr, &submitButtonRect);
	}
	else {
		SDL_RenderCopy(rendererSdl, submitButtonTexture.get(), nullptr, &submitButtonRect);
	}

	SDL_RenderPresent(rendererSdl);
}

void LoginScreen::clean()
{
	backgroundTexture.reset();
	submitButtonTexture.reset();
	submitButtonHoverTexture.reset();
	inputTextTexture.reset();
	font.reset();
	TTF_Quit();
}