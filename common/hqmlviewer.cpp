#include "hqmlviewer.h"

#include <iostream>

#define log(x) std::cout << x << std::endl

HQMLViewer::HQMLViewer(QObject *parent) :
    mRenderer(nullptr)
  , mTimer(nullptr)
  , m_t(0)
  , m_renderer(nullptr)
{
    connect(this, &QQuickItem::windowChanged, this, &HQMLViewer::handleWindowChanged);

    //    connect(this, &HQMLViewer::tChanged, [=]{

    //        window()->update();
    //    });
}

HQMLViewer::~HQMLViewer()
{

}

void HQMLViewer::handleWindowChanged(QQuickWindow *pWin)
{
    if (pWin) {
        connect(pWin, &QQuickWindow::beforeSynchronizing, this, &HQMLViewer::sync, Qt::DirectConnection);
        connect(pWin, &QQuickWindow::sceneGraphInvalidated, this, &HQMLViewer::cleanup, Qt::DirectConnection);
        //! [1]
        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        //! [3]
        pWin->setClearBeforeRendering(false);
    }



    //    if(pWin == nullptr)
    //        return;

    //    connect(window(), &QQuickWindow::beforeSynchronizing, this, &HQMLViewer::onRender, Qt::DirectConnection);
    //    connect(window(), &QQuickWindow::afterRendering, this, [=]{
    ////        std::cout << "start new render" << std::endl;
    //        window()->update();
    //    });//&HQMLViewer::onRender);

    //    window()->setClearBeforeRendering(false);
}

void HQMLViewer::cleanup()
{
    if (m_renderer) {
        delete m_renderer;
        m_renderer = nullptr;
    }
}

void HQMLViewer::sync()
{
    if (!m_renderer) {
        m_renderer = new HQMLRenderer();
        m_renderer->setRenderer(mRenderer);
        connect(window(), &QQuickWindow::beforeRendering, m_renderer, &HQMLRenderer::paint, Qt::DirectConnection);
    }
    m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    m_renderer->setT(m_t);
    m_renderer->setWindow(window());
}

void HQMLViewer::onRender()
{
    if(mRenderer == nullptr)
        return;

    mRenderer->doRender();
    window()->resetOpenGLState();
}

void HQMLViewer::onTick()
{
    if(mRenderer == nullptr)
        return;

    window()->update();
}

void HQMLViewer::setRenderer(harf::render::HIRenderer *renderer)
{
    if(renderer == nullptr)
        return;

    mRenderer = renderer;

    if(m_renderer)
        m_renderer->setRenderer(mRenderer);
    //    if(mTimer == nullptr)
    //    {
    //        mTimer = new QTimer;
    //        mTimer->setInterval(50);
    //        connect(mTimer, &QTimer::timeout, this, &HQMLViewer::onTick);

    //        mTimer->start();
    //    }
}

void HQMLViewer::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);

    if(mRenderer == nullptr)
        return;

    mRenderer->doResizeGeometry(newGeometry.x(), newGeometry.y(), newGeometry.width(), newGeometry.height());
}

void HQMLRenderer::paint()
{
    if(m_window == nullptr)
        return;

    if(mIsFirstTime)
    {
        createOsgContext();
        restoreOsgState();

        mIsFirstTime = false;
    }
    else // restore the osg gl context
        restoreOsgState();

    if(mRenderer != nullptr)
        mRenderer->doRender();

    // we're done with the osg state, restore the Qt one
    saveOsgState();

    return;
}

void HQMLRenderer::createOsgContext()
{
    // create a new shared OpenGL context to be used exclusively by OSG
    _osgContext = new QOpenGLContext();
    _osgContext->setFormat(m_window->requestedFormat());
    _osgContext->setShareContext(QOpenGLContext::currentContext());
    _osgContext->create();
}

void HQMLRenderer::saveOsgState()
{
    _osgContext->doneCurrent();
    _qtContext->makeCurrent(m_window);
}

void HQMLRenderer::restoreOsgState()
{
    _qtContext = QOpenGLContext::currentContext();
    _qtContext->doneCurrent();

    _osgContext->makeCurrent(m_window);
}
