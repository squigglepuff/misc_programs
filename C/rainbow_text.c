/**
  * Rainbow text application.
  *
  * This application simply prints a string on the screen using xterm-256color'ing and moves the gradient in a linear fashion.
  *
  *  (c) 2016 Travis M Ervin
  *  4 Spaces per TAB ; 120 Columns
  *
  */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/types.h>
#include <poll.h>

// This MACRO returns the size of a vector/pointer in elements. (Non zero based)
#define SZ(x) (sizeof(x)/sizeof(*(x)))

// Functions.
void SetRawMode();

/* RAINBOW CODE:
 * #!/bin/bash
 * CLR=(196 202 208 214 220 226 190 154 118 82 46 41 36 31 26 21 27 33 39 45 51 87 123 159 195 231 225 219 213 207 201 200 199 198 197)
 * for i in $CLR; do printf "\033[38;5;%dm$i" "$i"; done
*/

const unsigned char g_cvRainbow[] = {196, 202, 208, 214, 220, 226, 190, 154, 118, 82, 46, 41, 36, 31, 26, 21, 27, 33,
                                   39, 45, 51, 87, 123, 159, 195, 231, 225, 219, 213, 207, 201, 200, 199, 198, 197}; //!< Color code gradient to follow.
const char* g_ccMsg = "Hello World! I'm some rainbow text being printed via C code!"; //!< String to be printed.
char* pIter = NULL; //!< String iterator for printing.
unsigned short g_iRnbwIdx = 0; //!< Index of the color code to be printed.
struct termios g_sOrig; //!< Original terminal file descriptor.

/**
 * @brief main (__crtmain)
 *
 * @param[in] argc - Number of command-line arguments.
 * @param[in] argv - Command-line arguments passed.
 * @return int - 0 = succes.
 */
int main(int argc, char *argv[])
{
    clock_t iStart = clock();
    clock_t iCurr = clock();
    char cInput = '\0';
    struct pollfd sFD[1];
    int iReady;

    // We need to set the terminal to RAW mode so it doesn't block key presses.
    SetRawMode();

    // Setup POLL variables.
    sFD[0].fd = STDIN_FILENO;
    sFD[0].events = POLLIN;
    sFD[0].revents = 0;

    // Force set the environment to 256-color.
    if (-1 == setenv("TERM", "xterm-256color", 1) || !strcmp(getenv("TERM"), "dummy") || !strcmp(getenv("TERM"), "dumb"))
    {
        fprintf(stderr, "Can't print colors to a dummy/dumb terminal :(\n");
    }
    else
    {
        if (!strcmp(getenv("TERM"), "xterm-256color"))
        {
            g_iRnbwIdx = 0;

            // Hide the cursor.
            fprintf(stdout, "\033[?25l");

            while(/*0 <= iReady && */(cInput != 'q' || cInput != 'Q'))
            {
                iReady = poll(sFD, 1, 0);

                if (iReady == 1)
                {
                    // Read from stdin, but only a single char.
                    read(STDIN_FILENO, &cInput, 1);

                    if (cInput != 'q' || cInput != 'Q')
                    {
                        break;
                    }
                }

                // See if we're supposed to tick.
                iCurr = clock() / (CLOCKS_PER_SEC/1000);
                if ((iCurr - iStart) > 60)
                {
                    // Yup, tick.
                    if (g_iRnbwIdx >= strlen(g_ccMsg))
                    {
                        g_iRnbwIdx = 0;
                    }

                    for (pIter = g_ccMsg; *pIter != NULL; ++pIter, ++g_iRnbwIdx)
                    {
                        if (g_iRnbwIdx >= SZ(g_cvRainbow))
                        {
                            g_iRnbwIdx = 0;
                        }

                        fprintf(stdout, "\033[38;5;%dm%c", g_cvRainbow[g_iRnbwIdx], *pIter);
                    }

                    fprintf(stdout, "\r"); // Return the cursor to the front of the line.

                    ++g_iRnbwIdx;

                    iStart = clock() / (CLOCKS_PER_SEC/1000);
                }
            }
        }
        else
        {
            fprintf(stderr, "Aww....we need 256 colors so we can be purdy :C\n");
        }
        fprintf(stdout, "\033[0m");
    }

    // Restore the terminal.
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_sOrig);

    // Show the cursor.
    fprintf(stdout, "\033[?25h");

    fprintf(stdout, "\nPress [ENTER] to close...");
    fgetc(stdin);
    return 0;
}

/**
 * @brief SetRawMode
 *
 * This function sets our controlling terminal to RAW mode. Meaning it'll tak any character and dump it either staight into STDIN, STDOUT, or STDERR.
 *
 * @return None (void)
 */
void SetRawMode()
{
    struct termios raw;

    // Get attrs.
    tcgetattr(STDIN_FILENO, &g_sOrig);
    tcgetattr(STDIN_FILENO, &raw);

    // Setup flags.
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON); // We want to make '\0' (NULL) the break byte, translate CR to LF, enable parity checking, strip off eighth bit, and turn XON.
    raw.c_oflag &= ~(OPOST); // Enable implementation defined output processing. This prevents LF and CR from behaving weirdly.
    raw.c_cflag |= (CS8); // Set the character mask size to 8.
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); // Echo input characters, Enable canonical mode, enable implementation-defined input processing (prevents needing to hit ENTER) and generate signals for special chars when needed.

    // Control character processing timeouts.
    raw.c_cc[VMIN] = 5; raw.c_cc[VTIME] = 8;
    raw.c_cc[VMIN] = 0; raw.c_cc[VTIME] = 0;
    raw.c_cc[VMIN] = 2; raw.c_cc[VTIME] = 0;
    raw.c_cc[VMIN] = 0; raw.c_cc[VTIME] = 8;

    // Set the terminal to the new attributes.
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0)
    {
        fprintf(stderr, "Was unable to set RAW mode! :(\n");
        exit(-1);
    }
}
