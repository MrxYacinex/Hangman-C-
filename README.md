# Hangman Game in C

## Overview
This project is a simple implementation of the classic Hangman game written in C. The game challenges players to guess a hidden word one letter at a time. Players must guess correctly within a limited number of attempts to win the game.

## Features
- Word guessing functionality with customizable word lists.
- Display of partially guessed words and incorrect guesses.
- Adjustable difficulty through maximum attempts.
- Interactive text-based interface.

## How to Play
1. The game selects a random word from a predefined list.
2. The player is shown the word as a series of underscores (e.g., `____` for a four-letter word).
3. The player guesses one letter at a time by entering a character.
4. Correct guesses reveal the positions of the guessed letter in the word.
5. Incorrect guesses are tracked, and the player loses an attempt for each incorrect guess.
6. The game ends when the player either:
   - Guesses the entire word (Win).
   - Runs out of attempts (Lose).

## Setup and Installation
1. Clone or download this repository.
2. Open the project folder in your preferred IDE or terminal.
3. Compile the source code using a C compiler. For example, with GCC:
   ```bash
   gcc hangman.c -o hangman
   ```
4. Run the compiled program:
   ```bash
   ./hangman
   ```

## Code Structure
- `hangman.c`: The main source file containing the game logic.
- `words.txt` (optional): A text file containing a list of words for the game, one word per line.

## Customization
- **Adding Words:**
  - Update the word list in the code or provide a `words.txt` file.
- **Changing Difficulty:**
  - Modify the maximum number of attempts in the code.

## Dependencies
This project requires a standard C compiler and the standard C library. No additional dependencies are needed.

## Known Issues
- Words must not contain special characters or spaces.
- Case sensitivity: All words and guesses are treated as lowercase. Ensure the word list is in lowercase.

## Future Enhancements
- Add support for dynamic word lists from external files.
- Implement a graphical version of the game.
- Add multiplayer support.

## License
This project is open-source and available under the [MIT License](LICENSE).

## Acknowledgments
Thanks to classic games like Hangman for inspiring this project!

