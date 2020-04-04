#ifndef HQMLVIEWER_H
#define HQMLVIEWER_H

#include <QtQuick/QQuickItem>
#include <QQuickWindow>
#include <QTimer>

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#include <arengine/render/hirenderer.h>

class HQMLRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    HQMLRenderer() :
        m_t(0)
      , m_program(0)
      , m_window(0)
      , mRenderer(nullptr)
      , _osgContext(nullptr)
      , _qtContext(nullptr)
    {

    }

    ~HQMLRenderer()
    {
        delete m_program;
    }

    void setT(qreal t) { m_t = t; }
    void setViewportSize(const QSize &size) { m_viewportSize = size; }
    void setWindow(QQuickWindow *window) {
        m_window = window;
    }

    void setRenderer(harf::render::HIRenderer *renderer)
    {
        mRenderer = renderer;
    }

public slots:
    void paint();

private:
    void createOsgContext();
    bool mIsFirstTime = true;
    QSize m_viewportSize;
    qreal m_t;
    QOpenGLShaderProgram *m_program;
    QQuickWindow *m_window;
    harf::render::HIRenderer* mRenderer;


    // Pointer to QOpenGLContext to be used by OSG.
    QOpenGLContext* _osgContext;
    // Pointer to QOpenGLContext to be restored after OSG context
    QOpenGLContext* _qtContext;

    void saveOsgState();
    void restoreOsgState();
};

class HQMLViewer : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)

public:
    HQMLViewer(QObject* parent = nullptr);
    virtual ~HQMLViewer();

    void setRenderer(harf::render::HIRenderer *renderer);

    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    qreal t() const
    {
        return m_t;
    }

public slots:
    void setT(qreal t)
    {
//        qWarning("Floating point comparison needs context sanity check");
        if (qFuzzyCompare(m_t, t))
            return;

        m_t = t;
        emit tChanged(m_t);
        if (window())
            window()->update();
    }

    void cleanup();
    void sync();
signals:
    void tChanged(qreal t);

private slots:
    void handleWindowChanged(QQuickWindow *pWin);
    void onRender();
    void onTick();

private:
    harf::render::HIRenderer* mRenderer;
    QTimer* mTimer;
    qreal m_t;
    HQMLRenderer* m_renderer;
};

#endif // HQMLVIEWER_H
