#include <QHBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QMimeData>
#include "window.h"

CWindow::CWindow()
{
    QWidget *pW = new QWidget();
    setCentralWidget(pW);
//    pW->setAcceptDrops(true);

    mpGl = new CGl();
    mpGl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    mpGl->setAcceptDrops(true);

    QHBoxLayout *pLay = new QHBoxLayout();
    pLay->addWidget(mpGl);
    pW->setLayout(pLay);

    setWindowTitle(tr("Sprite Test"));
    resize(800, 600);

    mpTick = new QTimer(this);
    connect(mpTick, SIGNAL(timeout()), mpGl, SLOT(updateGL()));

    mpTick->start(FPS_NTSC);

    mpFileOpen = new QAction(tr("Open Sprite..."), this);
    mpFileOpen->setShortcut(QKeySequence::Open);
    connect(mpFileOpen, SIGNAL(triggered()), this, SLOT(FileOpen()));

    QMenu *pFileMenu = menuBar()->addMenu(tr("File"));
    pFileMenu->addAction(mpFileOpen);

    setAcceptDrops(true);

    setWindowIcon(QIcon(tr(":/icons/coin.ico")));
}

CWindow::~CWindow()
{
    if (nullptr != mpGl)
    {
        delete mpGl;
    }

    if (nullptr != mpFileOpen)
    {
        delete mpFileOpen;
    }
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

#if defined(_OS_WINDOWS)
        path.remove(0, 1);
#endif

        UnloadSprite();
        SetupSprite(path);
    }
}

void CWindow::FileOpen()
{
    QString fPath = QFileDialog::getOpenFileName(this, tr("Open File..."), QDir::currentPath(), tr("*.sprite"));

    if (fPath.isEmpty())
    {
        return;
    }
    else
    {
        UnloadSprite();
        SetupSprite(fPath);
    }
}
