#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPainter>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QMimeData>
#include <QPixmap>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QFont>
#include <QPushButton>
#include <QFileDialog>
#include "Font.h"

extern SGlobalCVars g_CVars;
extern EState g_GameState;
extern std::queue<EMessage> g_GameQueue;

class CWindow : public QMainWindow
{
    Q_OBJECT
public:
    CWindow();
    ~CWindow();

protected:
    void paintEvent(QPaintEvent *);

    void dragEnterEvent(QDragEnterEvent *aEnterEvent);
    void dragLeaveEvent(QDragLeaveEvent *aLeaveEvent);
    void dragMoveEvent(QDragMoveEvent *aMoveEvent);
    void dropEvent(QDropEvent *aEvent);

private slots:
    void Tick();
    void Save();

private:
    CWindow(const CWindow&);
    CWindow& operator=(const CWindow&);

    void CreateFontImage();
    void FeedFont();

    QTimer *mpTicker;
    QPixmap *pPix;
    QFont *pFont;
    QString mStr;

    QWidget *mpDraw;
    QPushButton *mpExport;

    CFont *mpFontSys;

    __int64 miRepaintTimer;

    int iFontID;
};

#endif // WINDOW_H
