#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() 
{

    srand(time(NULL));

    char guessWords[][16] = {
        "apple",
        "banana",
        "orange",
        "grape",
        "strawberry",
        "mango"
    };

    int randomIndex = rand() % 6;

    int numLives = 5;
    int numCorrect = 0;
    int oldCorrect = 0;
    int lengthOfWord = strlen(guessWords[randomIndex]);
    int letterGuessed[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    int quit = 0;

    int loopIndex = 0;

    char guess[16];

    printf("Welcome to the Hangman Game!\n");
    printf("The word has %d letters.\n", lengthOfWord);

    while(numCorrect < lengthOfWord) {
        printf("Please enter your guess.\n");
        fgets(guess, 16, stdin);

        if(strncmp(guess, "quit", 4) == 0) {
            quit = 1;
            break;
        }

        guess[strlen(guess) - 1] = 0;

        oldCorrect = numCorrect;

        for(loopIndex = 0; loopIndex < lengthOfWord; loopIndex++) {
            if(letterGuessed[loopIndex] == 1) {
                continue;
            }

            if(strncmp(&guess[0], &guessWords[randomIndex][loopIndex], 1) == 0) {
                letterGuessed[loopIndex] = 1;
                numCorrect++;
            }
        }

        if(oldCorrect == numCorrect) {
            numLives--;
            printf("Sorry, wrong guess.\n");
            if(numLives == 0) {
                break;
            }
        }

        for(loopIndex = 0; loopIndex < lengthOfWord; loopIndex++) {
            if(letterGuessed[loopIndex] == 1) {
                printf("%c", guessWords[randomIndex][loopIndex]);
            } else {
                printf("-");
            }
        }

        printf("\n");
    }
    
}