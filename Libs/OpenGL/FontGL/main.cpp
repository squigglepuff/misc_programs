#include <QApplication>
#include "window.h"

SGlobalCVars g_CVars;
EState g_GameState;
std::queue<EMessage> g_GameQueue;
std::vector<std::string> g_GameLog;
std::map<std::string, int> g_KeyCodes;

void Log(std::string aSys, std::string aMsg, std::string aSndr);

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    CWindow wnd;
    wnd.show();

    return app.exec();
}

void Log(std::string aSys, std::string aMsg, std::string aSndr)
{
    // Will be similar to ("{"<< aSys<< "}["<< clock()<< "] "<< aMsg<< "<"<< aSndr<< ">") when finished.
    size_t iMaxSize = 16;
    char *pTime = new char[iMaxSize];
    snprintf(pTime, iMaxSize, "%ld", clock());

    pTime[iMaxSize - 1] = '\0';

    std::string sLogLine = "{";
    sLogLine.append(aSys);
    sLogLine.append("}[");
    sLogLine.append(pTime);
    sLogLine.append("] ");
    sLogLine.append(aMsg);
    sLogLine.append("<");
    sLogLine.append(aSndr);
    sLogLine.append(">");

    g_GameLog.push_back(sLogLine);

    delete[] pTime;
}
