#include <QVBoxLayout>
#include <QKeyEvent>
#include <QMouseEvent>
#include "window.h"

int Negate(unsigned int aValue)
{
    return (aValue - (aValue*2));
}

std::string GetFileName(QString aPath)
{
    std::string path = aPath.toStdString();

    size_t iIdx = path.find_last_of('/');

    if (std::string::npos == iIdx)
    {
        iIdx = path.find_last_of('\\');

        if (std::string::npos == iIdx)
        {
            return "";
        }
    }

    ++iIdx; // This removes the searched character.
    path.erase(path.begin(), path.begin()+iIdx);

    iIdx = path.find_last_of('.');

    if (std::string::npos != iIdx)
    {
        path.erase(path.begin()+iIdx, path.end());
    }

    return path;
}

CWindow::CWindow()
{
    QWidget *pWidget = new QWidget();
    setCentralWidget(pWidget);

    mpDraw = new QWidget();
    mpDraw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mpExport = new QPushButton(tr("Export Font File!"), this);
    mpExport->setGeometry(0, 0, 540, 64);
    mpExport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(mpExport, SIGNAL(clicked()), this, SLOT(Save()));

    QVBoxLayout *pLayout = new QVBoxLayout;
    pLayout->addWidget(mpDraw);
    pLayout->addWidget(mpExport);
    pWidget->setLayout(pLayout);

#ifdef Q_WS_X11
    setWindowTitle(tr("Itsy Bitsy Spider LINUX"));
#endif
#ifdef Q_WS_MACX
    setWindowTitle(tr("Itsy Bitsy Spider OSX"));
#endif
#ifdef Q_WS_WIN32
    setWindowTitle(tr("Itsy Bitsy Spider WIN32"));
#endif

    resize(800, 600);

    mpTicker = new QTimer();
    connect(mpTicker, SIGNAL(timeout()), this, SLOT(Tick()));
    mpTicker->start(1);

    pPix = nullptr;
    pFont = nullptr;

    setMouseTracking(false);
    setAcceptDrops(true);

    g_GameState = EState_Init;

    mpFontSys = new CFont();

    iFontID = 0;

    miRepaintTimer = 0;
}

CWindow::~CWindow()
{
    if (nullptr != mpFontSys)
    {
        delete mpFontSys;
    }

    if (nullptr != pPix)
    {
        delete pPix;
    }

    if (nullptr != pFont)
    {
        delete pFont;
    }

    if (nullptr != mpTicker)
    {
        delete mpTicker;
    }
}

void CWindow::paintEvent(QPaintEvent *)
{
    if (nullptr == pPix)
    {
        pPix = new QPixmap(mpDraw->size());
        pPix->fill(QColor(0, 0, 0, 0));
    }

    QPainter *pPainter = new QPainter();
    if (nullptr != pFont)
    {
        pPainter->begin(pPix);
        pPainter->setRenderHint(QPainter::Antialiasing);
        pPainter->setRenderHint(QPainter::SmoothPixmapTransform);
        pPainter->setRenderHint(QPainter::TextAntialiasing);
        pPainter->setFont(*pFont);
        pPainter->setPen(Qt::white);
        pPainter->drawText(pPix->rect(), Qt::TextWordWrap, mStr);
        pPainter->end();
    }

    pPainter->begin(this);
    pPainter->fillRect(rect(), Qt::black);
    pPainter->setRenderHint(QPainter::Antialiasing);
    pPainter->setRenderHint(QPainter::SmoothPixmapTransform);
    pPainter->setRenderHint(QPainter::TextAntialiasing);
    pPainter->drawPixmap(mpDraw->x(), mpDraw->y(), *pPix);
    pPainter->end();
    delete pPainter;
}

void CWindow::dragEnterEvent(QDragEnterEvent *aEnterEvent)
{
    aEnterEvent->accept();
}

void CWindow::dragLeaveEvent(QDragLeaveEvent *aLeaveEvent)
{
    aLeaveEvent->accept();
}

void CWindow::dragMoveEvent(QDragMoveEvent *aMoveEvent)
{
    aMoveEvent->accept();
}

void CWindow::dropEvent(QDropEvent *aEvent)
{
    aEvent->accept();
    if (aEvent->mimeData()->hasUrls())
    {
        QUrl url = aEvent->mimeData()->urls().at(0);

        QString path = url.path();

#ifdef _OS_WINDOWS
        path.remove(0, 1);
#endif

        // Load the font.
        iFontID = QFontDatabase::addApplicationFont(path);

        if (-1 != iFontID)
        {
            CreateFontImage();
        }
    }
}

void CWindow::Tick()
{
    if (60 <= miRepaintTimer)
    {
        g_GameQueue.push(EMessage_UpdateGL);
        miRepaintTimer = 0;
    }

    // Check the queue for any activity.
    if (true != g_GameQueue.empty())
    {
        for (size_t iIdx = 0; iIdx < g_GameQueue.size(); ++iIdx)
        {
            switch(g_GameQueue.front())
            {
                case EMessage_UpdateGL:
                {
                    repaint();
                    g_GameQueue.pop();
                    break;
                }

                case EMessage_UpdateAL:
                {
                    g_GameQueue.pop();
                    break;
                }

                case EMessage_UpdatePhysics:
                {
                    g_GameQueue.pop();
                    break;
                }

                case EMessage_Quit:
                {
                    g_GameState = EState_Exit;
                    g_GameQueue.pop();
                    close();
                    break;
                }
            }
        }
    }

    ++miRepaintTimer;
}

void CWindow::Save()
{
    QString qPath = QFileDialog::getSaveFileName();

    if (!qPath.isEmpty() && nullptr != mpFontSys && nullptr != pPix)
    {
        std::string path = GetFileName(qPath);
        path.append(".png");

        mpFontSys->Save(qPath.toStdString(), path);
        pPix->save(path.data());
    }
}

void CWindow::CreateFontImage()
{
    QStringList families = QFontDatabase::applicationFontFamilies(iFontID);

    pFont = new QFont(families.at(0));
    pFont->setPointSize(48);
    mStr = "";

    size_t iCount = 0;
    for (char iChar = ' '; iChar <= '~'; ++iChar, ++iCount)
    {
        if (19 <= iCount)
        {
            mStr.append('\n');
            iCount = 0;
        }

        mStr.append(iChar);
    }

    // We now need to feed the font information to the system so that is can be created into a .font file!
    FeedFont();
}

void CWindow::FeedFont()
{
    //code
    // K is the default size glyph
    QFontMetrics fMetrics(*pFont);

    SCoords sControl(0, 0, fMetrics.width('K'), 0);
    SCoords sGeometry(0, 0, 0, 0);

    SCharacter sChar;
    size_t iCount = 0;
    for (char iChar = ' '; iChar <= '~'; ++iChar, ++iCount)
    {
        sChar.mcValue = iChar;

        // Kerning.
        if ('K' == iChar)
        {
            sChar.mKerning = sControl;
            sChar.mKerning.miRight = Negate(sChar.mKerning.miRight);
        }
        else
        {
            size_t iWidth = sControl.miRight - fMetrics.width(iChar);

            // We need a special case for q, g, y, and j.
            if ('q' == iChar || 'g' == iChar || 'y' == iChar || 'j' == iChar)
            {
                // SPECIAL CASE //
                size_t iHeight = fMetrics.height() / 2;
                sChar.mKerning = SCoords(0, 0, Negate(iWidth), Negate(iHeight));
            }
            else
            {
                sChar.mKerning = SCoords(0, 0, Negate(iWidth), 0);
            }
        }

        if (19 <= iCount)
        {
            sGeometry.miTop += fMetrics.height();
            sGeometry.miRight = 0;
            sGeometry.miLeft = 0;
            iCount = 0;
        }

        sGeometry.miRight += fMetrics.width(iChar);
        sGeometry.miBottom = sGeometry.miTop + fMetrics.height();
        sChar.mTexCoords = sGeometry;

        mpFontSys->AddCharacter(sChar);

        sGeometry.miLeft += fMetrics.width(iChar);
    }
}
