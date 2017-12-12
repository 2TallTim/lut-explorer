#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QObject>
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QDebug>
#include <QMouseEvent>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include "transform3d.h"
#include "vertex.h"
#include "lutfileholder.h"

class QOpenGLShaderProgram;


class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    ~MyGLWidget();
    explicit MyGLWidget(QWidget *parent = nullptr);
public slots:
    void initGeom(std::vector<LUTFileHolder::LUTPoint> data);

protected slots:
    void update();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width,int height);
    void teardownGL();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    void printContextInformation();

    double dragLast_x, dragLast_y;
    Transform3D dragStart_trans;

    //OGL State info
    QOpenGLBuffer m_vertex;
    QOpenGLVertexArrayObject m_object;
    QOpenGLShaderProgram * m_program;

    QOpenGLBuffer b_vertex;
    QOpenGLVertexArrayObject b_object;
    QOpenGLShaderProgram * b_program;

    // Shader Information
    int u_modelToWorld;
    int u_worldToView;

    QMatrix4x4 m_projection;
    Transform3D m_transform;

    static uint nVerts;

    void makeCube(QVector3D pos, double sz, QVector3D col, Vertex *buf);
};

#endif // MYGLWIDGET_H
