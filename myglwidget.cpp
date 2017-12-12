#include "myglwidget.h"
#include <QOpenGLShaderProgram>

uint MyGLWidget::nVerts = 24;

MyGLWidget::MyGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    QSurfaceFormat format;
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(32);

    this->setFormat(format);
    m_transform.translate(0.0f, 0.0f, -5.0f);
    nVerts = 24;
}

MyGLWidget::~MyGLWidget(){
    makeCurrent();
    teardownGL();
}

QSize MyGLWidget::minimumSizeHint() const {return QSize(50,50);}

QSize MyGLWidget::sizeHint() const {return QSize(400,400);}

void MyGLWidget::initializeGL(){
    initializeOpenGLFunctions();
    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    printContextInformation();


    {
      // Create Shader (Do not release until VAO is created)
      m_program = new QOpenGLShaderProgram();
      m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
      m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");
      m_program->link();
      m_program->bind();

      // Cache Uniform Locations
      u_modelToWorld = m_program->uniformLocation("modelToWorld");
      u_worldToView = m_program->uniformLocation("worldToView");


      // Create Buffer (Do not release until VAO is created)
      m_vertex.create();
      m_vertex.bind();
      m_vertex.setUsagePattern(QOpenGLBuffer::DynamicDraw);
      m_vertex.allocate(sizeof(Vertex)*24*33*33*33);
      Vertex cvert[24];
      makeCube(QVector3D(0,0,0),0.1,QVector3D(0,1,0),cvert);
      m_vertex.write(0,cvert,sizeof(cvert));
      nVerts = 24;

      // Create Vertex Array Object
      m_object.create();
      m_object.bind();
      m_program->enableAttributeArray(0);
      m_program->enableAttributeArray(1);
      m_program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
      m_program->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());

      // Release (unbind) all
      m_object.release();
      m_vertex.release();
      m_program->release();
    }
    //Repeat for bounding box
    {
      // Create Shader (Do not release until VAO is created)
      b_program = new QOpenGLShaderProgram();
      b_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
      b_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");
      b_program->link();
      b_program->bind();


      // Create Buffer (Do not release until VAO is created)
      b_vertex.create();
      b_vertex.bind();
      b_vertex.setUsagePattern(QOpenGLBuffer::DynamicDraw);

      Vertex cvert[24] = {
          Vertex(QVector3D(-1,-1,-1),QVector3D(0,0,0)),Vertex(QVector3D(1,-1,-1),QVector3D(1,0,0)),
          Vertex(QVector3D(-1,-1,-1),QVector3D(0,0,0)),Vertex(QVector3D(-1,1,-1),QVector3D(0,1,0)),
          Vertex(QVector3D(-1,-1,-1),QVector3D(0,0,0)),Vertex(QVector3D(-1,-1,1),QVector3D(0,0,1)),

          Vertex(QVector3D(1,1,-1),QVector3D(1,1,0)),Vertex(QVector3D(-1,1,-1),QVector3D(0,1,0)),
          Vertex(QVector3D(1,1,-1),QVector3D(1,1,0)),Vertex(QVector3D(1,-1,-1),QVector3D(1,0,0)),
          Vertex(QVector3D(1,1,-1),QVector3D(1,1,0)),Vertex(QVector3D(1,1,1),QVector3D(1,1,1)),

          Vertex(QVector3D(1,-1,1),QVector3D(1,0,1)),Vertex(QVector3D(-1,-1,1),QVector3D(0,0,1)),
          Vertex(QVector3D(1,-1,1),QVector3D(1,0,1)),Vertex(QVector3D(1,1,1),QVector3D(1,1,1)),
          Vertex(QVector3D(1,-1,1),QVector3D(1,0,1)),Vertex(QVector3D(1,-1,-1),QVector3D(1,0,0)),

          Vertex(QVector3D(-1,1,1),QVector3D(0,1,1)),Vertex(QVector3D(1,1,1),QVector3D(1,1,1)),
          Vertex(QVector3D(-1,1,1),QVector3D(0,1,1)),Vertex(QVector3D(-1,-1,1),QVector3D(0,0,1)),
          Vertex(QVector3D(-1,1,1),QVector3D(0,1,1)),Vertex(QVector3D(-1,1,-1),QVector3D(0,1,0))
      };
      b_vertex.allocate(cvert,sizeof(cvert)); //24 verts in bbox

      // Create Vertex Array Object
      b_object.create();
      b_object.bind();
      b_program->enableAttributeArray(0);
      b_program->enableAttributeArray(1);
      b_program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
      b_program->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());

      // Release (unbind) all
      b_object.release();
      b_vertex.release();
      b_program->release();
    }
}

void MyGLWidget::paintGL(){
    glEnable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0.0f, 1.0f);

    glLineWidth(3);
    glClearDepthf(1.0f);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    //Clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Render bbox

    b_program->bind();
    b_program->setUniformValue(u_worldToView, m_projection);

        b_object.bind();
        b_program->setUniformValue(u_modelToWorld, m_transform.toMatrix());
        glDrawArrays(GL_LINES, 0, 24);
        b_object.release();

        m_object.bind();
        //b_program->setUniformValue(u_modelToWorld, m_transform.toMatrix());
        glDrawArrays(GL_QUADS, 0, nVerts);
        m_object.release();

    b_program->release();



}

void MyGLWidget::update(){
    //Schedule a redraw
    QOpenGLWidget::update();
}

void MyGLWidget::resizeGL(int width, int height){
    m_projection.setToIdentity();

    m_projection.perspective(45.0f, width/float(height), 0.1f, 100.0f);
}

void MyGLWidget::teardownGL(){
    m_object.destroy();
    m_vertex.destroy();
    delete m_program;
}

void MyGLWidget::initGeom(std::vector< LUTFileHolder::LUTPoint > data){
    m_vertex.bind();

    for(int i = 0; i < data.size(); ++i){
        Vertex v[24];
        LUTFileHolder::LUTPoint lp = data[i];
        QVector3D pos = QVector3D(lp.data[3]/4096.0f, lp.data[4]/4096.0f, lp.data[5]/4096.0f);
        pos -= QVector3D(0.5,0.5,0.5); //Center
        pos *= QVector3D(2.0,2.0,2.0);
        QVector3D col = QVector3D(lp.data[0],lp.data[1],lp.data[2]);
        makeCube(pos,0.005,col,v);
        m_vertex.write(sizeof(v) * i, v, sizeof(v) ); //864 bytes per cube
    }

    m_vertex.release();
    nVerts = data.size() * 24;
}

void MyGLWidget::makeCube(QVector3D pos, double sz, QVector3D col,Vertex * buf){

    Vertex VERTEX_FTR = Vertex( QVector3D( sz,  sz,  sz) + pos, col );
    Vertex VERTEX_FTL = Vertex( QVector3D(-sz,  sz,  sz) + pos, col );
    Vertex VERTEX_FBL = Vertex( QVector3D(-sz, -sz,  sz) + pos, col );
    Vertex VERTEX_FBR = Vertex( QVector3D( sz, -sz,  sz) + pos, col );

    // Back Vertices
    Vertex VERTEX_BTR = Vertex( QVector3D( sz,  sz, -sz) + pos, col );
    Vertex VERTEX_BTL = Vertex( QVector3D(-sz,  sz, -sz) + pos, col );
    Vertex VERTEX_BBL = Vertex( QVector3D(-sz, -sz, -sz) + pos, col );
    Vertex VERTEX_BBR = Vertex( QVector3D( sz, -sz, -sz) + pos, col );

    // Create a colored cube
    Vertex cube_verts[24] = {
      // Face 1 (Front)
        VERTEX_FTR, VERTEX_FTL, VERTEX_FBL, VERTEX_FBR,
      // Face 2 (Back)
        VERTEX_BBR, VERTEX_BBL, VERTEX_BTL, VERTEX_BTR,
      // Face 3 (Top)
        VERTEX_FTR, VERTEX_BTR, VERTEX_BTL, VERTEX_FTL,
      // Face 4 (Bottom)
        VERTEX_FBR, VERTEX_FBL, VERTEX_BBL, VERTEX_BBR,
      // Face 5 (Left)
        VERTEX_FBL, VERTEX_FTL, VERTEX_BTL, VERTEX_BBL,
      // Face 6 (Right)
        VERTEX_FTR, VERTEX_FBR, VERTEX_BBR, VERTEX_BTR
    };

    std::memcpy(buf,cube_verts,sizeof(cube_verts));
    return;
}

void MyGLWidget::printContextInformation()
{
  QString glType;
  QString glVersion;
  QString glProfile;

  // Get Version Information
  glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
  glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

  // Get Profile Information
#define CASE(c) case QSurfaceFormat::c: glProfile = #c; break
  switch (format().profile())
  {
    CASE(NoProfile);
    CASE(CoreProfile);
    CASE(CompatibilityProfile);
  }
#undef CASE

  // qPrintable() will print our QString w/o quotes around it.
  qDebug() << qPrintable(glType) << qPrintable(glVersion) << "(" << qPrintable(glProfile) << ")";
}

/******************
 * INPUT HANDLING *
 ******************/

void MyGLWidget::mousePressEvent(QMouseEvent *event){
    dragLast_x = event->x();
    dragLast_y = event->y();
    dragStart_trans = m_transform;
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event){
    double dx = event->x() - dragLast_x;
    double dy = event->y() - dragLast_y;
    Transform3D t = m_transform;
    double rotScale = 0.25; //Magic numbers are bad! Make this related to widget height/width
    t.rotate(dy*rotScale, 1.0f, 0, 0);
    t.rotate(dx*rotScale, 0.0f, 1.0f, 0.0f);
    m_transform = t;
    update();
    dragLast_x = event->x();
    dragLast_y = event->y();
}
