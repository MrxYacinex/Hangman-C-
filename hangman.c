#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL_ttf.h>

void SDL_RenderDrawCircle(SDL_Renderer *renderer, int x, int y, int radius);

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Function to draw the gallows and hangman based on remaining lives
void draw_hangman(SDL_Renderer *renderer, int lives, int centerX, int baseY, int scale) {
    // Set draw color to white (chalk-like)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    int poleHeight = scale * 400;
    int beamLength = scale * 200;
    int ropeLength = scale * 50;
    int headRadius = scale * 30;
    int bodyLength = scale * 140;
    int armLength = scale * 50;
    int legLength = scale * 100;

    // Draw gallows
    SDL_RenderDrawLine(renderer, centerX - beamLength, baseY, centerX - beamLength, baseY - poleHeight); // Vertical pole
    SDL_RenderDrawLine(renderer, centerX - beamLength, baseY - poleHeight, centerX, baseY - poleHeight); // Horizontal beam
    SDL_RenderDrawLine(renderer, centerX, baseY - poleHeight, centerX, baseY - poleHeight + ropeLength); // Hanging rope

    // Draw hangman based on lives
    if (lives <= 4) SDL_RenderDrawCircle(renderer, centerX, baseY - poleHeight + ropeLength + headRadius, headRadius); // Head
    if (lives <= 3) SDL_RenderDrawLine(renderer, centerX, baseY - poleHeight + ropeLength + headRadius * 2, centerX, baseY - poleHeight + ropeLength + headRadius * 2 + bodyLength); // Body
    if (lives <= 2) SDL_RenderDrawLine(renderer, centerX, baseY - poleHeight + ropeLength + headRadius * 2 + bodyLength / 4, centerX - armLength, baseY - poleHeight + ropeLength + headRadius * 2 + bodyLength / 4 + armLength); // Left arm
    if (lives <= 1) SDL_RenderDrawLine(renderer, centerX, baseY - poleHeight + ropeLength + headRadius * 2 + bodyLength / 4, centerX + armLength, baseY - poleHeight + ropeLength + headRadius * 2 + bodyLength / 4 + armLength); // Right arm
    if (lives <= 0) {
        SDL_RenderDrawLine(renderer, centerX, baseY - poleHeight + ropeLength + headRadius * 2 + bodyLength, centerX - legLength, baseY - poleHeight + ropeLength + headRadius * 2 + bodyLength + legLength); // Left leg
        SDL_RenderDrawLine(renderer, centerX, baseY - poleHeight + ropeLength + headRadius * 2 + bodyLength, centerX + legLength, baseY - poleHeight + ropeLength + headRadius * 2 + bodyLength + legLength); // Right leg
    }
}

void SDL_RenderDrawCircle(SDL_Renderer *renderer, int x, int y, int radius) {
    const int diameter = radius * 2;

    int offsetX = radius - 1;
    int offsetY = 0;
    int d = 1 - offsetX;

    while (offsetX >= offsetY) {
        SDL_RenderDrawPoint(renderer, x + offsetX, y + offsetY);
        SDL_RenderDrawPoint(renderer, x + offsetY, y + offsetX);
        SDL_RenderDrawPoint(renderer, x - offsetY, y + offsetX);
        SDL_RenderDrawPoint(renderer, x - offsetX, y + offsetY);
        SDL_RenderDrawPoint(renderer, x - offsetX, y - offsetY);
        SDL_RenderDrawPoint(renderer, x - offsetY, y - offsetX);
        SDL_RenderDrawPoint(renderer, x + offsetY, y - offsetX);
        SDL_RenderDrawPoint(renderer, x + offsetX, y - offsetY);

        if (d < 0) {
            d += 2 * offsetY + 1;
        } else {
            d += 2 * (offsetY - offsetX) + 1;
            offsetX--;
        }
        offsetY++;
    }
}

void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dstRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int isInside(int x, int y, SDL_Rect *rect) {
    return (x > rect->x && x < rect->x + rect->w && y > rect->y && y < rect->y + rect->h);
}

void resetGame(int *numLives, int *numCorrect, int *randomIndex, int *letterGuessed, char *guessedWord, char **words, int wordCount) {
    *numLives = 5;
    *numCorrect = 0;
    *randomIndex = rand() % wordCount;
    memset(letterGuessed, 0, sizeof(int) * 16);
    memset(guessedWord, 0, sizeof(char) * 16);
}

char** loadWordsFromFile(const char* filename, int* wordCount) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open word file");
        exit(EXIT_FAILURE);
    }

    char** words = malloc(sizeof(char*) * 100);
    *wordCount = 0;
    char buffer[100];

    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline character
        words[*wordCount] = strdup(buffer);
        (*wordCount)++;
    }

    fclose(file);
    return words;
}

void freeWords(char** words, int wordCount) {
    for (int i = 0; i < wordCount; i++) {
        free(words[i]);
    }
    free(words);
}

int main() {
    srand(time(NULL));

    int wordCount;
    char** words = loadWordsFromFile("words.txt", &wordCount);

    int randomIndex = rand() % wordCount;
    int lengthOfWord = strlen(words[randomIndex]);
    int letterGuessed[16] = {0};

    int numLives = 5;
    int numCorrect = 0;
    char guessedWord[16] = {0};

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL Initialization Error: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        printf("SDL_ttf Initialization Error: %s\n", TTF_GetError());
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 24);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Hangman Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        printf("SDL Window Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("SDL Renderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int quit = 0;
    int playAgain = 0;
    SDL_Event event;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color chalkboard = {30, 30, 30, 255};

    SDL_Rect playAgainButton = {WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT - 80, 200, 50};

    while (!quit) {
        if (playAgain) {
            resetGame(&numLives, &numCorrect, &randomIndex, letterGuessed, guessedWord, words, wordCount);
            lengthOfWord = strlen(words[randomIndex]);
            playAgain = 0;
        }

        while (numCorrect < lengthOfWord && numLives > 0 && !playAgain) {
            int windowWidth, windowHeight;
            SDL_GetWindowSize(window, &windowWidth, &windowHeight);

            int centerX = windowWidth / 2;
            int baseY = windowHeight - 100;
            int scale = windowHeight / 600;

            for (int i = 0; i < lengthOfWord; i++) {
                if (letterGuessed[i]) {
                    guessedWord[i] = words[randomIndex][i];
                } else {
                    guessedWord[i] = '_';
                }
            }
            guessedWord[lengthOfWord] = '\0';

            SDL_SetRenderDrawColor(renderer, chalkboard.r, chalkboard.g, chalkboard.b, chalkboard.a);
            SDL_RenderClear(renderer);

            draw_hangman(renderer, numLives, centerX, baseY, scale);
            renderText(renderer, font, "Hangman", centerX - 50, 20, white);

            char livesText[32];
            snprintf(livesText, sizeof(livesText), "Lives: %d", numLives);
            renderText(renderer, font, livesText, centerX - 50, baseY + 20, white);

            // Adjust spacing between letters
            int letterSpacing = 24; // Increase spacing
            int wordStartX = centerX - (lengthOfWord * letterSpacing) / 2;

            for (int i = 0; i < lengthOfWord; i++) {
                char letter[2] = {guessedWord[i], '\0'};
                renderText(renderer, font, letter, wordStartX + i * letterSpacing, baseY + 60, white);
            }

            SDL_RenderPresent(renderer);

            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    quit = 1;
                    playAgain = 0;
                    break;
                } else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        quit = 1;
                        playAgain = 0;
                        break;
                    }

                    char guess = event.key.keysym.sym;

                    if (guess >= 'a' && guess <= 'z') {
                        int correct = 0;
                        for (int i = 0; i < lengthOfWord; i++) {
                            if (words[randomIndex][i] == guess && !letterGuessed[i]) {
                                letterGuessed[i] = 1;
                                numCorrect++;
                                correct = 1;
                            }
                        }

                        if (!correct) {
                            numLives--;
                        }
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, chalkboard.r, chalkboard.g, chalkboard.b, chalkboard.a);
        SDL_RenderClear(renderer);

        int windowWidth, windowHeight;
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        int centerX = windowWidth / 2;
        int baseY = windowHeight - 100;
        int scale = windowHeight / 600;

        draw_hangman(renderer, numLives, centerX, baseY, scale);
        if (numCorrect == lengthOfWord) {
            renderText(renderer, font, "You win!", centerX - 50, baseY + 100, white);
        } else if (numLives == 0) {
            renderText(renderer, font, "You lose!", centerX - 50, baseY + 100, white);
            renderText(renderer, font, words[randomIndex], centerX - strlen(words[randomIndex]) * 12 / 2, baseY + 140, white);
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &playAgainButton);
        renderText(renderer, font, "Play Again", playAgainButton.x + 30, playAgainButton.y + 10, (SDL_Color){0, 0, 0, 255});

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
                break;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    quit = 1;
                    break;
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (isInside(event.button.x, event.button.y, &playAgainButton)) {
                    playAgain = 1;
                }
            }
        }
    }

    freeWords(words, wordCount);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
