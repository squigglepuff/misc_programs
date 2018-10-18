/**
 * Tail Logs CLI utility
 *
 * This utility will provide basic "Tail Logs" style functionality on the command-line interface (CLI) for a user to use.
 * The utility will check common cPanel log locations and if the user can read them, it'll present the log as an option to either
 * follow for X seconds or grab X lines from the end of the log. The application will also present a basic search pattern function
 * that allows wildcard and basic TXT searching capabilities to help filter out certain lines.
 *
 * (c) 2016 Travis M Ervin
 * 4 Spaces per TAB ; 120 Columns
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <math.h>
#include <time.h>

// Linux headers.
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/user.h>
#include <sys/time.h>
#include <sys/wait.h>

// MACROs.
#define SZ(x) (sizeof(x)/sizeof(*(x))) // Returns the size of pointer/array.

// Type definitions.
typedef unsigned char byte;       //!< Byte data type - Size: 1 byte (8-bits)
typedef unsigned short word;      //!< Word data type - Size: 2 bytes (16-bits)
typedef unsigned int dword;       //!< DWord data type - Size: 4 bytes (32-bits)
typedef unsigned long long qword; //!< QWord data type - Size: 8 bytes (64-bits)
typedef void* void_ptr;           //!< Void* data type - Size: N bytes (N-bits)
typedef char* string;       //!< String data type - Size: N bytes (N-bits)

/**
 * @brief The Singly linked-list node structure.
 *
 * This data structure is a rudimentary singly linked-list.
 */
struct SListNode
{
    void_ptr mpData; //!< Data referenced by the node.
    struct SListNode* mpNxtNode; //!< The next node in the list.
}ListNode;

// Just incase we're not able to use "bool".
#if !defined(_CXX)
typedef int BOOL;
#define TRUE 1
#define FALSE 0
#endif

// Function prototypes.
void GetLogList(const string asLogLoc, const string asUser);
void PrintHelp();
void PrintVer();
void HandleError();
char* StripLFCR(char* apStr);
BOOL IsGZip(char *apStr);
int StrToInt(string apStr);
byte SearchData(string apPattern);
void_ptr TailLog(char *apPath, int aiTailAmnt, string asPattern, BOOL abFollow);
string EscapeQuotes(string apStr);
dword StrLen(char* apStr);

string g_sVersion = "1.0 r0";    //!< Application version.
char* g_pvFList[65534];
dword g_iGblFIdx;
struct SListNode* g_pLineList;
struct SListNode* g_pListIter;
extern char **environ; // Environment.

/**
 * @brief Main (__crtmain)
 * @param[in] aArgc - Argument count.
 * @param[in] apArgv - Vector containing the arguments.
 * @return integer - 0 => Success.
 */
int main(int aArgc, char *apArgv[])
{
    word iArgIdx = 0;
    uid_t iUserID = geteuid();
    struct passwd *pPasswd = getpwent();
    string pUsername = "";
    char* pLogLoc = (string)malloc(65535); // Constant C-Style string to the most common location of logs.
    dword iFileIdx = 0;
    const word ciSelLen = 32;
    string sTmpInput = (string)malloc(ciSelLen);
    int iLogSel = -1;
    BOOL bFollow = FALSE; // Should we follow the log or not?
    int iTlAmnt = 400; // Number of lines/seconds to tail.
    string sPattern = NULL; // The search pattern. Supports basic searching. No regex.

    for (iArgIdx = 1; iArgIdx < aArgc; ++iArgIdx)
    {
        if (!strcmp(apArgv[iArgIdx], "-h") || !strcmp(apArgv[iArgIdx], "--help") || !strcmp(apArgv[iArgIdx], "?"))
        {
            PrintHelp();
            return 0;
        }
        else if (!strcmp(apArgv[iArgIdx], "-v") || !strcmp(apArgv[iArgIdx], "--version"))
        {
            PrintVer();
            return 0;
        }
        else if (!strcmp(apArgv[iArgIdx], "-p") || !strcmp(apArgv[iArgIdx], "--paths"))
        {
            ++iArgIdx;
            if (NULL != apArgv[iArgIdx])
            {
                memset(pLogLoc, 0, 65535);
                memcpy(pLogLoc, apArgv[iArgIdx], strlen(apArgv[iArgIdx]));
                pLogLoc[strlen(pLogLoc)] = '\0';
            }
            else
            {
                fprintf(stderr, "ERROR: You need to pass in a path after \"-p\"!\n");
                return 0;
            }
        }
    }

    fprintf(stdout, "Grabbing the username...");

    // Grab the username and for now, output it.
    for (pPasswd = getpwent(); pPasswd != NULL; pPasswd = getpwent())
    {
        if (pPasswd->pw_uid == iUserID)
        {
            // Got the username!
            pUsername = pPasswd->pw_name;
            break;
        }
    }
    fprintf(stdout, "[DONE]\n");

    // Open the log location and check the elements there and see if we can read them and they're a file.
    fprintf(stdout, "Searching \"%s\" for logs...\n", pLogLoc);
    GetLogList(pLogLoc, pUsername);

    for (iFileIdx = 0; g_pvFList[iFileIdx] != NULL; ++iFileIdx)
    {
        fprintf(stdout, "%d\t%s\n", iFileIdx,g_pvFList[iFileIdx]);
    }
    fprintf(stdout, "Select a log to tail (pick a number or \"q\" to quit): ");
    fgets(sTmpInput, ciSelLen, stdin);

    // Check to see if we're wanting to quit!
    if (strcmp(sTmpInput, "q\n") && strcmp(sTmpInput, "quit\n"))
    {
        iLogSel = StrToInt(sTmpInput);
        fprintf(stdout, "Going to tail %s (#%d)\n\n", g_pvFList[iLogSel], iLogSel);

        fprintf(stdout, "Do you wish to follow the log? (y/N) ");
        fgets(sTmpInput, ciSelLen, stdin);

        if (strcmp(sTmpInput, "q\n") && strcmp(sTmpInput, "quit\n"))
        {
            if (*sTmpInput != 'n' && *sTmpInput != 'N' && !strcmp(sTmpInput, ""))
            {
                bFollow = TRUE;
                fprintf(stdout, "How many seconds would you like to follow? ");
                fgets(sTmpInput, ciSelLen, stdin);

                iTlAmnt = StrToInt(sTmpInput);
                fprintf(stdout, "Will tail for %d seconds.\n\n", iTlAmnt);
            }
            else
            {
                bFollow = FALSE;
                fprintf(stdout, "How many lines would you like to grab? ");
                fgets(sTmpInput, ciSelLen, stdin);

                iTlAmnt = StrToInt(sTmpInput);
                fprintf(stdout, "Will grab %d lines.\n\n", iTlAmnt);
            }

            fprintf(stdout, "Finally, please enter a search pattern (Uses \"grep -E\"): ");
            fgets(sTmpInput, ciSelLen, stdin);

            if (strcmp(sTmpInput, "q\n") && strcmp(sTmpInput, "quit\n"))
            {
                if (!strcmp(sTmpInput, "\n"))
                {
                    sPattern = StripLFCR(".*");
                }
                else
                {
                    sPattern = StripLFCR(sTmpInput);
                }

                fprintf(stdout, "\n\n");

                if (bFollow)
                {
                    fprintf(stdout, "Tailing %s for %d seconds searching for \"%s\"\n", g_pvFList[iLogSel], iTlAmnt, sPattern);
                    TailLog(g_pvFList[iLogSel], iTlAmnt, sPattern, bFollow);
                }
                else
                {
                    fprintf(stdout, "Grabbing %d lines from %s searching for \"%s\"\n", iTlAmnt, g_pvFList[iLogSel], sPattern);
                    TailLog(g_pvFList[iLogSel], iTlAmnt, sPattern, bFollow);
                }
            }
        }
    }

    for (g_iGblFIdx = 0; NULL != g_pvFList[g_iGblFIdx]; ++g_iGblFIdx)
    {
        if (NULL != g_pvFList[g_iGblFIdx])
        {
            free(g_pvFList[g_iGblFIdx]);
        }
    }

    if (NULL != sTmpInput)
    {
        free(sTmpInput);
    }

    if (NULL != pLogLoc)
    {
        free(pLogLoc);
    }

    if (NULL != sPattern)
    {
        free(sPattern);
    }

    fprintf(stdout, "\n\n");
    fprintf(stderr, "Press [ENTER] to quit...");
    fgetc(stdin);
    return 0;
}

/**
 * @brief Print Help function
 *
 * This function prints a basic multi-line help message for the user so they can grasp at basic usage of the application.
 *
 * @return void
 */
void PrintHelp()
{
    fprintf(stdout, "Basic Syntax:\n\ttaillogs [options]\n\nOptions:\n\t-p, --paths <path>\t-\tUse this path instead of the default.\n\t-h, --help\t\t-\tPrint this help.\n\t-v, --version\t\t-\tPrint the version.\n\n");
    PrintVer();
}

/**
 * @brief Print Version function
 *
 * This function prints a basic multi-line version message for the user.
 *
 * @return void
 */
void PrintVer()
{
    fprintf(stdout, "Tail Logs CLI Version: %s\n© 2016 Travis M Ervin\nWritten in C'97 Using Qt Creator and CMake\n", g_sVersion);
}

/**
 * @brief Get Log List
 *
 * This function opens the given location and checks all files at said location. If the supplied user can read the file in question, the absolute path of the file
 * will be appended to the list to be returned. The user MUST have read access on the file(s) found. This function is a recursive function which'll call itself
 * when it encounters a directory the user supplied can execute.
 *
 * @param[in] asLogLoc
 * @param[in] asUser
 * @return String array - Contains a list of C-Style strings which are the absolute paths to the logs in question.
 */
void GetLogList(const string asLogLoc, const string asUser)
{
    // Variable setup.
    DIR* pDir = NULL;
    struct dirent* pDirEnt = NULL;
    struct stat sStat;
    const word ciMaxFiles = 65534;
    word iFileIdx = 0;
    char** pvFileList = (char**)malloc(ciMaxFiles);
    char* pTmpPath = (char*)malloc(ciMaxFiles);
    char** pIter = NULL;
    dword iFType = 0;
    dword iPerms = 0;
    BOOL bNeedsTail = FALSE; // Only set if the path needs a trailing "/".

    // Sanity checking.
    assert(asLogLoc != NULL);
    assert(asUser != NULL);

    if (!strcmp(asLogLoc, ""))
    {
        fprintf(stderr, "WARN: You're not allowed to pass an empty path to GetLogList!\n");
    }
    else
    {
        if (!strcmp(asUser, ""))
        {
            fprintf(stderr, "WARN: You're not allowed to pass an empty username to GetLogList!\n");
        }
        else
        {
            // Open the directory and read all it's contents in one swoop.
            pDir = opendir(asLogLoc);

            if (pDir)
            {
                pDirEnt = readdir(pDir);

                while (pDirEnt)
                {
                    if (!strcmp(pDirEnt->d_name, ".") || !strcmp(pDirEnt->d_name, ".."))
                    {
                        pDirEnt = readdir(pDir);
                    }
                    else
                    {
                        pvFileList[iFileIdx] = (string)malloc(strlen(pDirEnt->d_name)+1);
                        memset(pvFileList[iFileIdx], 0, strlen(pDirEnt->d_name)+1);
                        memcpy(pvFileList[iFileIdx], pDirEnt->d_name, strlen(pDirEnt->d_name));
                        ++iFileIdx;
                    }

                    pDirEnt = readdir(pDir);
                }

                closedir(pDir);

                // Before we continue, check to see if we need to add a trailing "/".
                if (asLogLoc[(strlen(asLogLoc)-1)] != '/')
                {
                    // Yup, flag it.
                    bNeedsTail = TRUE;
                }

                // Iterate through the files and stat them.
                for (pIter = pvFileList; *pIter != NULL; ++pIter)
                {
                    // Initial check, we don't want to stat "." or ".."!
                    if (!strcmp(*pIter, ".") || !strcmp(*pIter, ".."))
                    {
                        continue;
                    }

                    // Check to see if it's a GZip file.
                    if (TRUE == IsGZip(*pIter))
                    {
                        // It is, skip it.
                        continue;
                    }

                    // Clean our temp buffer.
                    if (NULL != pTmpPath)
                    {
                        memset(pTmpPath, 0, ciMaxFiles);
                        free(pTmpPath);
                    }

                    pTmpPath = (char*)malloc(ciMaxFiles+1);
                    memset(pTmpPath, 0, ciMaxFiles+1);

                    // Begin appending.
                    strcat(pTmpPath, asLogLoc);

                    if (bNeedsTail)
                    {
                        strcat(pTmpPath, "/"); // FOR SAFETY! Append a "/" in case the initial path doesn't have one.
                    }

                    strcat(pTmpPath, *pIter);
                    pTmpPath[strlen(pTmpPath)] = '\0';

                    // Stat the file in question.
                    if (!stat(pTmpPath, &sStat))
                    {
                        iFType = sStat.st_mode & S_IFMT;

                        if (iFType == S_IFREG || iFType == S_IFLNK)
                        {
                            // Check ownership.
                            if (0 == geteuid() || sStat.st_uid == geteuid())
                            {
                                // We're the owner!
                                iPerms = sStat.st_mode & S_IRWXU;
                            }
                            else if (sStat.st_gid == getgid())
                            {
                                // We're in the group!
                                iPerms = sStat.st_mode & S_IRWXG;
                            }
                            else
                            {
                                // We're part of "others"!
                                iPerms = sStat.st_mode & S_IRWXO;
                            }

                            // Check to make sure we have read permissions!
                            if ((iPerms & S_IRUSR) || (iPerms & S_IRGRP) || (iPerms & S_IROTH))
                            {
                                // We do! Flag the file as "OK".
                                g_pvFList[g_iGblFIdx] = (string)malloc(strlen(pTmpPath)+1);
                                memset(g_pvFList[g_iGblFIdx], 0, strlen(pTmpPath)+1);
                                strcat(g_pvFList[g_iGblFIdx], pTmpPath);
                                ++g_iGblFIdx;
                            }
                        }
                        else if (iFType == S_IFDIR)
                        {
                            // TODO: Add the recursion code for scanning additional directories.
                        }
                    }
                    else
                    {
                        fprintf(stderr, "ERROR: %s {%d}\n", strerror(errno), errno);
                    }
                }
            }
            else
            {
                fprintf(stderr, "ERROR: %s {%d}\n", strerror(errno), errno);
            }
        }
    }

    if (NULL != pTmpPath)
    {
        free(pTmpPath);
    }

    for (pIter = pvFileList; *pIter != NULL; ++pIter)
    {
        if (NULL != *pIter)
        {
            free(*pIter);
        }
    }

    if (NULL != pvFileList)
    {
        free(pvFileList);
    }
}

/**
 * @brief Strip Line-Feed and Carriage-Return
 *
 * This function goes through a given string a strips off all line-feed and carriage-return characters.
 *
 * @param[i] apStr - char* The string to strip.
 * @return char* - Freshly stripped string.
 */
char* StripLFCR(char* apStr)
{
    int iRtnIdx = 0;
    char* pRtn = (char*)malloc(SZ(apStr));
    memset(pRtn, 0, SZ(apStr));

    for (;apStr != NULL && *apStr != 0; ++apStr)
    {
        if (*apStr != '\n' && *apStr != '\r')
        {
            pRtn[iRtnIdx] = *apStr;
            ++iRtnIdx;
        }
    }

    return pRtn;
}

/**
 * @brief IsGZip
 *
 * This function checks a file path to see if it has the ".gz" extension.
 *
 * @param[in] apStr - The string to check.
 * @return BOOL:int - 0 => False/Failure ;; 1 => True/Success
 */
BOOL IsGZip(char* apStr)
{
    if (NULL != apStr && 0 != SZ(apStr))
    {
        dword iStrSz = StrLen(apStr)-1;

        if (apStr[iStrSz] == 'z' && apStr[iStrSz-1] == 'g' && apStr[iStrSz-2] == '.')
        {
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * @brief String to Integer
 *
 * This function converts a string into a decimal number.
 *
 * @param[in] apStr - The string to convert to a number.
 * @return integer
 */
int StrToInt(string apStr)
{
    int iRtn = 0;
    string pIter = apStr;

    for (pIter = apStr;pIter != NULL && *pIter != 0; ++pIter)
    {
        if (*pIter >= '0' && *pIter <= '9')
        {
            if (iRtn <= 0)
            {
                iRtn += (int)(*pIter-'0');
            }
            else
            {
                iRtn *= 10;
                iRtn += (int)(*pIter-'0');
            }
        }
    }

    return iRtn;
}

/**
 * @brief Tail Log
 *
 * This function simply tails a chosen log following the given parameters.
 *
 * @param[in] apPath - The path of the file to be read.
 * @param[in] aiTailAmnt - The amount of lines/seconds to tail.
 * @param[in] asPattern - The pattern to search for.
 * @param[in] abFollow - Should we follow the log? [DEFAULT : FALSE]
 * @return void_ptr - Pointer to the data that matched the pattern after being retrieved. NULL upon failure.
 */
void_ptr TailLog(char* apPath, int aiTailAmnt, string asPattern, BOOL abFollow)
{
    // Variable setup.
    const dword ciMaxRead = 8192; // ~ 8KB.
    FILE* hFD = NULL; // File descriptor for the open log.
    string sTmpStr = (string)malloc(ciMaxRead);
    string sToken = NULL;
    ssize_t iReadAmnt = 0;

    // Used for the timed tail.
    time_t iBeginTime;

    // Used for the line grab.
    dword iLineCount = 0;
    char cTmp = '\0';


    // Sanity check.
    assert(apPath != NULL);
    assert(asPattern != NULL);

    // We want to open the log.
    hFD = fopen(apPath, "r");

    if (hFD != NULL)
    {
        // See to the end of the file minus the max read size.
        if (-1 != fseek(hFD, 0, SEEK_END))
        {
            // Divide up the string and make sure we read in the correct number of lines.
            if (abFollow)
            {
                // Wait for X number of seconds.
                // Grab the current time.
                time(&iBeginTime);
                while ((time(NULL) - iBeginTime) < aiTailAmnt) {}
            }
            else
            {
                // Loop backwards through the file until we we have grabbed X lines.
                // Seek backward by 1 byte.
                fseek(hFD, -(sizeof(byte)), SEEK_END);

                // Read the data out of the log.
                iReadAmnt = fread(&cTmp, 1, 1, hFD);
                while (0 != iReadAmnt && -1 != iReadAmnt)
                {
                    // Check to see if we've hit a new line.
                    if (cTmp == '\n')
                    {
                        // We have, increment the counter.
                        ++iLineCount;

                        if (iLineCount > aiTailAmnt)
                        {
                            // All done, we don't need more lines.
                            break;
                        }
                    }

                    // Seek backward by two bytes.
                    fseek(hFD, -(sizeof(byte)*2), SEEK_CUR);

                    // Attempt to read more data.
                    iReadAmnt = fread(&cTmp, 1, 1, hFD);

                    if (-1 == iReadAmnt)
                    {
                        fprintf(stderr, "ERROR: %s {%d}\n", strerror(errno), errno);
                    }
                }

                if (-1 == iReadAmnt)
                {
                    fprintf(stderr, "ERROR: %s {%d}\n", strerror(errno), errno);
                }

            }

            // Read the data out of the log.
            iReadAmnt = fread(sTmpStr, sizeof(byte), ciMaxRead, hFD);
            while (0 != iReadAmnt && -1 != iReadAmnt)
            {
                // We need to tokenize the current string and push it onto the linked list.
                sToken = strtok(sTmpStr, "\n");

                while (NULL != sToken)
                {
                    if (NULL == g_pListIter)
                    {
                        // Allocate the linked list node.
                        g_pLineList = (struct SListNode*)malloc(sizeof(ListNode));
                        g_pListIter = g_pLineList;
                    }
                    else
                    {
                        // Allocate a new node.
                        g_pListIter->mpNxtNode = (struct SListNode*)malloc(sizeof(ListNode));
                        g_pListIter = g_pListIter->mpNxtNode;
                    }

                    // Fill the node.
                    g_pListIter->mpData = malloc(strlen(sToken));
                    memset(g_pListIter->mpData, 0, strlen(sToken));
                    memcpy(g_pListIter->mpData, sToken, strlen(sToken));
//                    strcat(g_pListIter->mpData, "\n");

                    // Grab next token.
                    sToken = strtok(NULL, "\n");
                }

                // Attempt to read more data.
                iReadAmnt = fread(sTmpStr, sizeof(byte), ciMaxRead, hFD);

                if (-1 == iReadAmnt)
                {
                    fprintf(stderr, "ERROR: %s {%d}\n", strerror(errno), errno);
                }
            }

            if (-1 == iReadAmnt)
            {
                fprintf(stderr, "ERROR: %s {%d}\n", strerror(errno), errno);
            }

            // Close the file.
            fclose(hFD);

            // Pattern search the data!.
            SearchData(asPattern);

            // DEBUG ONLY!
            // DUMP THE DATA!
//            for (g_pListIter = g_pLineList; g_pListIter != NULL; g_pListIter = g_pListIter->mpNxtNode)
//            {
//                fprintf(stdout, "g_pListIter\t=>\t%s\n", (string)g_pListIter->mpData);
//            }
        }
        else
        {
            fprintf(stderr, "ERROR: %s {%d}\n", strerror(errno), errno);
        }
    }
    else
    {
        fprintf(stderr, "ERROR: %s {%d}\n", strerror(errno), errno);
    }

    if (sTmpStr)
    {
        free(sTmpStr);
    }
}

/**
 * @brief Search Data
 *
 * This function performs a popen of /usr/bin/grep with the "-E" flag for extended regex. It should grab any data that comes in on stdout and return it.
 *
 * @param apPattern - The pattern to grep for.
 * @return byte - A return code on the success state of the function.
 */
byte SearchData(string apPattern)
{
    const char* sCmdPath = "/bin/grep";
    const unsigned int ciMaxBuff = 65535;
    int iArgIdx = 0;
    char* svCmdArgs[16];
    int iPipeFD[2];
    fd_set sReadSet;
    struct timeval tVal;
    pid_t iPID = -1;
    int iChldRtn = 0;

    // Reset the iterator.
    g_pListIter = g_pLineList;

    // Setup sCmdArgs.
    memset(svCmdArgs, 0, (sizeof(char*)*16));
    svCmdArgs[0] = (char*)malloc(ciMaxBuff);
    svCmdArgs[1] = (char*)malloc(ciMaxBuff);
    svCmdArgs[2] = (char*)malloc(ciMaxBuff);

    snprintf(svCmdArgs[0], ciMaxBuff, "grep");
    snprintf(svCmdArgs[1], ciMaxBuff, "-E");
    snprintf(svCmdArgs[2], ciMaxBuff, "%s", apPattern);

    // Clean out sReadSet.
    FD_ZERO(&sReadSet);

    // Setup timeout struct.
    tVal.tv_sec = 0; // Seconds
    tVal.tv_usec = 10; // uSeconds

    if (-1 != pipe(iPipeFD))
    {
        // Setup sReadSet.
        FD_SET(iPipeFD[0], &sReadSet);

        // We need to fork then run our execution code.
        iPID = fork();

        if (0 == iPID)
        {
            // Setup the file descriptors as needed.
            dup2(iPipeFD[0], STDIN_FILENO);
            close(iPipeFD[0]);
            close(iPipeFD[1]);

            // Spawn the process and begin feeding data into the write end of the pipe.
            iArgIdx = execve(sCmdPath, svCmdArgs, environ);
            if (-1 == iArgIdx)
            {
                switch(errno)
                {
                case E2BIG:
                    fprintf(stderr, "ERROR: Arg list or ENV to big!\nERROR: %s {%d}\n", strerror(errno), errno);
                    break;
                case EACCES:
                    fprintf(stderr, "ERROR: Was unable to locate file or you're not allowed to execute it!\nERROR: %s {%d}\n", strerror(errno), errno);
                    break;
                case EFAULT:
                    fprintf(stderr, "ERROR: You're not allowed to access that address space!\nERROR: %s {%d}\n", strerror(errno), errno);
                    break;
                case EINVAL:
                    fprintf(stderr, "ERROR: Bad ELF header on %s!\nERROR: %s {%d}\n", sCmdPath, strerror(errno), errno);
                    break;
                case EIO:
                    fprintf(stderr, "ERROR: An I/O error has occurred!\nERROR: %s {%d}\n", strerror(errno), errno);
                    break;
                case EISDIR:
                    fprintf(stderr, "ERROR: Weird, \"%s\" is a directory!\nERROR: %s {%d}\n", sCmdPath, strerror(errno), errno);
                    break;
                case ELIBBAD:
                    fprintf(stderr, "ERROR: Unrecognized ELF!\nERROR: %s {%d}\n", strerror(errno), errno);
                    break;
                case ENAMETOOLONG:
                    fprintf(stderr, "ERROR: Your name is to damn big!\nERROR: %s {%d}\n", strerror(errno), errno);
                    break;
                case ENFILE:
                    fprintf(stderr, "ERROR: We're not allowed to open more files!\nERROR: %s {%d}\n", strerror(errno), errno);
                    break;
                case ENOENT:
                    fprintf(stderr, "ERROR: I couldn't find \"%s\"!\nERROR: %s {%d}\n", sCmdPath, strerror(errno), errno);
                    break;
                case ENOEXEC:
                    fprintf(stderr, "ERROR: I just tried to execute a file that doesn't want to run!\nERROR: %s {%d}\n", strerror(errno), errno);
                    break;
                case ENOMEM:
                    fprintf(stderr, "ERROR: I can't remember anything yo...\nERROR: %s {%d}\n", strerror(errno), errno);
                    break;
                case ENOTDIR:
                    fprintf(stderr, "ERROR: Not a directory?\nERROR: %s {%d}\n", strerror(errno), errno);
                    break;
                case EPERM:
                    fprintf(stderr, "ERROR: Yeah....we all want a LOT of things...\nERROR: %s {%d}\n", strerror(errno), errno);
                    break;
                case ETXTBSY:
                    fprintf(stderr, "ERROR: Hard to open something that's already open :/\nERROR: %s {%d}\n", strerror(errno), errno);
                    break;
                }
            }
        }
        else if (-1 != iPID)
        {
            close(iPipeFD[0]);

            // Begin feeding data.
            while (g_pListIter != NULL && g_pListIter->mpData != NULL)
            {
                // Dump the iterator contents into pipe:[0].
                write(iPipeFD[1], g_pListIter->mpData, strlen(g_pListIter->mpData));
                write(iPipeFD[1], "\n", 1);

                // Increment the iterator.
                g_pListIter = g_pListIter->mpNxtNode;
            }

            // Reset the iterator.
            g_pListIter = g_pLineList;
        }
        else
        {
            fprintf(stderr, "ERROR: %s {%d}\n", strerror(errno), errno);
        }

        // Close the pipe FD.
        FD_ZERO(&sReadSet);
        close(iPipeFD[0]);
        close(iPipeFD[1]);
    }
    else
    {
        fprintf(stderr, "ERROR: %s {%d}\n", strerror(errno), errno);
    }

    // A little house cleaning.
    for (iArgIdx = 0; iArgIdx < SZ(svCmdArgs) && NULL != svCmdArgs[iArgIdx]; ++iArgIdx)
    {
        if (NULL != svCmdArgs[iArgIdx])
        {
            free(svCmdArgs[iArgIdx]);
        }
        else
        {
            break;
        }
    }

    // We need to wait for the grep PID to end before we can do anything else!
    waitpid(iPID, &iChldRtn, 0);

    return 0x0;
}

/**
 * @brief String Length
 *
 * This function returns the number of characters in a string up till the NULL terminator.
 *
 * @param[in] apStr - The string to count.
 * @return DWORD - the length of the string.
 */
dword StrLen(char* apStr)
{
    dword iRtn = 0;
    char* pIter = apStr;

    for (pIter = apStr; pIter != NULL && *pIter != 0; ++pIter, ++iRtn)
    {
        if (pIter == NULL || 0 == *pIter)
        {
            break;
        }
    }

    return iRtn;
}
