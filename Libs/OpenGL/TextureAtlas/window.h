#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QGLWidget>
#include <QTimer>
#include <QAction>
#include <QMenu>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include "atlasscript.h"

class CGl : public QGLWidget
{
public:
    CGl() : isSetup(false) { initializeGL(); }
    ~CGl()
    {
        if (nullptr != mpScript)
        {
            delete mpScript;
        }
    }

    void SetupSprite(std::string aPath)
    {
        if (nullptr == mpScript)
        {
            mpScript = new CAtlasScript();
        }

        isSetup = true;
    }

    void UnloadSprite()
    {
        if (nullptr != mpScript)
        {
            delete mpScript;
        }

        isSetup = false;
    }

protected:
    void paintGL()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        if(isSetup)
        {
        }
    }

    void initializeGL()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        isSetup = false;

        setAutoBufferSwap(true);

        paintGL();
    }

    void resizeGL(int w, int h)
    {
        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glOrtho(0, w, h, 0, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        paintGL();
    }

private:
    bool isSetup;

    CAtlasScript *mpScript;
};

class CWindow : public QMainWindow
{
    Q_OBJECT
public:
    CWindow();
    ~CWindow();

    void SetupSprite(QString aqPath) { mpGl->SetupSprite(aqPath.toStdString()); }
    void UnloadSprite() { mpGl->UnloadSprite(); }

public slots:
    void FileOpen();

protected:
    void dragEnterEvent(QDragEnterEvent *aEnterEvent);
    void dragLeaveEvent(QDragLeaveEvent *aLeaveEvent);
    void dragMoveEvent(QDragMoveEvent *aMoveEvent);
    void dropEvent(QDropEvent *aEvent);

private:
    CGl *mpGl;
    QTimer *mpTick;

    QAction *mpFileOpen;
};

#endif // WINDOW_H
