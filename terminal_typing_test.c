/*
 *
 * Title: Terminal Typing Test
 * Description: A command line typing test for the terminal similar to monkeytype.com
 * Author: github.com/deadshxll
 * Source: https://github.com/deadshxll/terminal-typing-test
 */

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>

#define MAX_SIZE 65535

// ANSI Color Codes
// https://gist.github.com/JBlond/2fea43a3049b38287e5e9cefc87b2124
#ifndef ANSI
#define ANSI

#define GRAY    "\033[90m"
#define WHITE   "\033[37m"
#define CYAN    "\033[36m"
#define RED     "\033[31m"
#define BLACK   "\033[30m"

#define RED_UNDERLINE "\033[4;31m"

#define WHITE_BACKGROUND "\033[47m"

#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"

#define WIPE_SCREEN  "\033[2J"
#define CLEAR_SCREEN "\033[H"

#define RESET "\033[0;0;0m"

#endif


long long start_time;

void draw_paragraph(char paragraph[], int pg_len, char typed[], int typed_len);
int generate_paragraph(char out[], char binary_path[], int count);
int error_count(char original[], char typed[]);
long long millis();

int main(int argc, char *argv[]) {

  // Modifying the VirtualTerminalLevel registry key to 1 to allow for ANSI coloring in command prompt
  DWORD val = 1;
  HKEY key;
  if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Console", 0, KEY_SET_VALUE | KEY_WOW64_64KEY, &key) == ERROR_SUCCESS)
  {
    RegSetValueExW(key, L"VirtualTerminalLevel", 0, REG_DWORD, (LPBYTE) &val, sizeof(DWORD));
    RegCloseKey(key);
  }
#pragma comment(lib, "Advapi32.lib")

  int i, c, typed_len, pg_len, fixed_errors, errors, word_count;
  char paragraph[MAX_SIZE];
  char typed[MAX_SIZE];

  start_time = typed_len = pg_len = 0;
  errors = fixed_errors = 0;

  printf(HIDE_CURSOR);
  printf(WIPE_SCREEN);

  if (argc == 2 && atoi(argv[1]))
    word_count = atoi(argv[1]);
  else
    word_count = 15;

  generate_paragraph(paragraph, argv[0], word_count);

  pg_len = strlen(paragraph);
  draw_paragraph(paragraph, pg_len, typed, typed_len);

  while (pg_len > typed_len) {
    c = _getch();

    // 3 = CTRL + c, 4 = CTRL + d
    if (c == 3 || c == 4 || c == EOF) {
      printf(SHOW_CURSOR);
      return 1;
    }

    // 18 = CTRL + r, this will be used for resetting
    if (c == 18)
    {
      typed_len = errors = -1;
      start_time = 0;
      memset(typed, 0, sizeof typed);
      memset(paragraph, 0, sizeof paragraph);
      generate_paragraph(paragraph, argv[0], word_count);
      pg_len = strlen(paragraph);
      printf(WIPE_SCREEN);
    }

    if (start_time <= 0) start_time = millis();

    // Handling backspace
    // Character code '127' is CTRL + backspace
    if (c != '\b' && c != 127) {
      typed[typed_len] = c;

      if (c != paragraph[typed_len]) errors++;

      typed[typed_len + 1] = '\0';
      typed_len++;
    } else if (c == '\b') {
      if (typed_len > 0) {
        typed[typed_len] = '\0';
        typed_len--;
      }
    } else if (c == 127) { // CTRL + backspace requires special logic due to how it behaves
      int hit_char = 0;
      for (i = typed_len; i > 0; i--) {
        int tc = typed[i];
        if (tc != ' ' && !hit_char) {
          hit_char = 1;
          i--;
        }

        if (hit_char == 1) {
          if (tc == ' ' || tc == '\t' || tc == '\n') {
            typed[i + 1] = '\0';
            break;
          }
        }
        typed_len--;
      }
    }

    draw_paragraph(paragraph, pg_len, typed, typed_len);
  }

  printf(WHITE);

  float minutes_elapsed = (millis() - start_time) / 60000.0;
  printf("  WPM\t\t %.1f\n", (strlen(typed) / 5.0) / minutes_elapsed);

  int err_count = error_count(paragraph, typed);
  fixed_errors = errors - err_count;
  printf("  Errors\t %d\n", err_count);
  printf("  Fixed Errors\t %d\n\n", fixed_errors);

  printf(RESET);
  printf(SHOW_CURSOR);
  return 0;
}


/*
 * Handles drawing the paragraph to the terminal
 *
 * Arguments:
 *   char paragraph[]: The entire paragraph to be typed
 *   int pg_len      : The string length of the paragraph
 *   char typed[]    : What was typed
 *   int typed_len   : The string length of what was typed
 */
void draw_paragraph(char paragraph[], int pg_len, char typed[], int typed_len) {
  int i, sec, min;

  sec = min = 0;

  //system("cls");
  printf(CLEAR_SCREEN);

  if (start_time > 0) {
    sec = (millis() - start_time) / 1000;
    min = sec / 60;
  }

  printf(RESET);
  printf(GRAY);
  printf("\n    %02d:%02d\n\n", min, sec - (min * 60));

  for (i = 0; i < typed_len; i++) {
    if (paragraph[i] == typed[i])
      printf(CYAN);  // Cyan
    else
      printf("%s%s", RED_UNDERLINE, RED);  // Red
    //
    putchar(paragraph[i]);
    printf(RESET);  // Reset
  }

  // Fake cursor
  printf("%s%s", WHITE_BACKGROUND, BLACK);
  putchar(paragraph[typed_len]);

  // Untyped part
  printf(RESET);  // Reset
  printf(GRAY);   // Gray
  for (i = typed_len + 1; i < pg_len; i++) putchar(paragraph[i]);

  printf(RESET);

  printf("\n\n");
}


/*
 * Grabs random words from 'wordlist.txt' and outputs them as a line
 *
 * Arguments:
 *   char out[]        : Variable to write to
 *   char binary_path[]: Path to binary to retrieve the wordlist file
 *   int count         : Amount of words to grab
 * Returns:
 *   int: Word count
 */
int generate_paragraph(char out[], char binary_path[], int count) {
  FILE *fileptr;
  int i, j, c, line_count, size, word_count;
  char line[4096];
  char path[4096];

  i = 0;
  while ((path[i] = binary_path[i]) != '\0')
    i++;

  for (i = strlen(path); i > 0; i--)
  {
    if (path[i] == '\\')
    {
      path[i] = '\0';
      break;
    }
  }
  strcat_s(path, sizeof path, "\\wordlist.txt");

  fopen_s(&fileptr, path, "r");
  line_count = 0;

  while ((c = fgetc(fileptr))) {
    if (c == '\n')
      line_count++;
    else if (feof(fileptr))
      break;
  }

  fseek(fileptr, 0, SEEK_SET);
  srand(time(NULL) + clock());

  int lines[count + 1];
  for (i = 0; i <= count; i++) lines[i] = (rand() % line_count);

  i = 0;
  while (fgets(line, 4096, fileptr)) {
    for (j = 0; j < count; j++) {
      if (lines[j] == i) {
        size = strlen(line);
        line[size - 1] = ' ';

        strcat_s(out, MAX_SIZE, line);
        word_count++;
      }
    }
    i++;
  }

  size = strlen(out);
  out[size - 1] = '\0';

  fclose(fileptr);

  return word_count;
}


/*
 * Returns amount of incorrectly typed characters compared to the original
 * paragraph
 *
 * Arguments:
 *   char original[]: The original string
 *   char typed[]   : The string to compare
 *
 * Returns:
 *   int: Amount of errors
 */
int error_count(char original[], char typed[]) {
  int i, count;

  count = 0;

  for (i = 0; i < strlen(original); i++) {
    if (original[i] != typed[i]) count++;
  }

  return count;
}

/*
 * Gets the current system time in milliseconds
 *
 * Returns:
 *   long long: System time in milliseconds
 */
long long millis()
{
  FILETIME ft;
  ULARGE_INTEGER uli;

  GetSystemTimeAsFileTime(&ft);
  uli.LowPart = ft.dwLowDateTime;
  uli.HighPart = ft.dwHighDateTime;

  long long milliseconds = (uli.QuadPart / 10000LL);

  return milliseconds;
}
