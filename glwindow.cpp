/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "glwindow.h"
#include <QImage>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLExtraFunctions>
#include <QPropertyAnimation>
#include <QPauseAnimation>
#include <QSequentialAnimationGroup>
#include <QTimer>
#include <QFile>


#include <QMouseEvent>
#include <QKeyEvent>
#ifdef ANDROID
#include <QRotationReading>
#endif

#include <math.h>

GLWindow::GLWindow()
    : m_seg_image(0),
      m_texture_rgb(0),
      m_texture_rgb_empty(0),
      m_texture_rgb_norm(0),

      m_texture_rgb_emptyr(0),

      m_texture_depth(0),
      m_texture_depth_empty(0),
      m_texture_segmentation(0),
      m_program(0),
      m_vbo(0),
      m_ibo(0),
      m_vao(0),
      m_cube_vbo(0),
      m_cube_ibo(0),
      m_cube_vao(0),
      m_obj_vbo(0),
      m_obj_ibo(0),
      m_obj_vao(0),
      m_target(0, 0, -1)
{

#ifdef ANDROID
    m_vertex_shader = QString("/home/tukur/Documents/QT_Projects/spider-viewer/android-build/assets/data/spider.vert");         //assets:/data/spider.vert
    m_fragment_shader = QString("/home/tukur/Documents/QT_Projects/spider-viewer/android-build/assets/data/spider.frag");       // assets:/data/spider.frag
    m_rgb_image = QString("/home/tukur/Documents/QT_Projects/spider-viewer/android-build/assets/data/scene_test.png");          // assets:/data/scene_test.png

    m_rotation_sensor = new QRotationSensor(this);
    connect(m_rotation_sensor, SIGNAL(readingChanged()), this, SLOT(checkReading()));
    m_rotation_sensor->setActive(true);
    m_rotation_sensor->connectToBackend();
    if (! m_rotation_sensor->start() ) {
       qDebug() << "Error: Rotation sensor could not start";
    } else {
       qDebug() << "Started rotation";
       qDebug() << m_rotation_sensor->description();
    }
#else
    m_vertex_shader = QString("spider.vert");
    m_fragment_shader = QString("spider.frag");

    m_rgb_image = QString("VR_app/scene_03280_1000658_full.png");
    m_rgb_image_empty = QString("VR_app/scene_03280_1000658pred.png");
    m_depth_image = QString("VR_app/scene_03280_1000658_full_depth.png");
    m_depth_image_empty = QString("VR_app/scene_03280_1000658_empty_depth.png");
    m_segmentation_image = QString("VR_app/scene_03280_1000658_seg.png");
    m_rgb_image_norm = QString("VR_app/normal.png");


    //m_rgb_image = QString("/home/tukur/Documents/QT_Projects/spider-viewer4/VR_app/scene1/scene1_full.png");
    //m_rgb_image_empty = QString("/home/tukur/Documents/QT_Projects/spider-viewer4/VR_app/scene1/scene1_pred.png");

    //m_rgb_image_emptyr = QString("/home/tukur/Documents/QT_Projects/spider-viewer4/VR_app/scene1/scene1_predr.png"); //

    //m_depth_image = QString("/home/tukur/Documents/QT_Projects/spider-viewer4/VR_app/scene1/scene1_full_depth.png");  //
    //m_depth_image_empty = QString("/home/tukur/Documents/QT_Projects/spider-viewer4/VR_app/scene1/scene1_empty_depth.png");
    //m_segmentation_image = QString("/home/tukur/Documents/QT_Projects/spider-viewer4/VR_app/scene1/scene1_seg.png");


    m_obj_file = QString("VR_app/TableAndChairs-filt.obj");
#endif
    m_eye = QVector3D(0.0,0.0,0.0);
    m_world.setToIdentity();
    //m_world.scale(50.0f);
    //m_world.rotate(180, 1, 0, 0);

    m_mouse_pressed = false;

    QTimer * t = new QTimer(this);
    connect(t,SIGNAL(timeout()),this, SLOT(update()));
    t->start(0);

    m_sphere_radius_cm = 6553.6f;
    m_eye_separation_cm = 7.5f;
    m_stereo_enabled = true;
    m_stereo_norm_enabled = true;
    m_wireframe_enabled = false;

    m_pointcloud_enabled = false;

    m_draw_empty_enabled = false;
    m_draw_norm_enabled = false;

    m_mixed_view_enabled =  false;
    m_mixed_view_norm_enabled = false;
    m_lighting_enabled = false;
    m_depth_inset_enabled =  false;

    m_depth_enabled = false;
    m_draw_cube_enabled = false;
    m_draw_obj_enabled = false;
    m_object_position = QVector3D(0.0f,-153.237,-250.0f);

    m_viewport_width = 1200;
    m_viewport_height = 800;

    m_fov = 80.0f;
    m_near = 0.1f;
    m_far = 10000.0f;
    m_point_size = 3.0f;

    m_picked_object_count = 0;
    m_pick_colors.resize(8);
    for (std::size_t i = 0; i < m_pick_colors.size(); ++i) {
        m_pick_colors[i] = QVector3D(0.0,0.0,0.0);
    }
#if 0
    m_scene_list.push_back( QString("/home/tukur/Documents/QT_Projects/spider-viewer/Exp_results/experiment2/exp2.1_TFHub"));
    m_scene_list.push_back( QString("/home/tukur/Documents/QT_Projects/spider-viewer/Exp_results/experiment2/exp2.2_VGG19_fewSteps_wo_optimization"));
    m_scene_list.push_back( QString("/home/tukur/Documents/QT_Projects/spider-viewer/Exp_results/experiment2/exp2.3_100steps_optimized"));
    m_scene_list.push_back( QString("/home/tukur/Documents/QT_Projects/spider-viewer/Exp_results/experiment2/exp2.4_500steps_optimized"));
    m_scene_list.push_back( QString("/home/tukur/Documents/QT_Projects/spider-viewer/Exp_results/experiment2/exp2.5_1ksteps_optimized"));
    m_scene_list.push_back( QString("/home/tukur/Documents/QT_Projects/spider-viewer/Exp_results/experiment2/exp2.6_10ksteps_optimized"));
//#else
    m_scene_list.push_back( QString("/home/tukur/Documents/QT_Projects/spider-viewer/selected/scene_03280_1000658"));
    m_scene_list.push_back( QString("/home/tukur/Documents/QT_Projects/spider-viewer/selected/scene_03310_141"));
    m_scene_list.push_back( QString("/home/tukur/Documents/QT_Projects/spider-viewer/selected/scene_03380_795"));
    m_scene_list.push_back( QString("/home/tukur/Documents/QT_Projects/spider-viewer/selected/scene_03419_1008755"));
    m_scene_list.push_back( QString("/home/tukur/Documents/QT_Projects/spider-viewer/selected/scene_03427_997175"));
#endif
    m_current_scene = 0;

 }

void GLWindow::checkReading() {
#ifdef ANDROID
    QRotationReading*  r = m_rotation_sensor->reading();
    //qDebug() << "X rotation " << r->x();
    //qDebug() << "Y rotation " << r->y();
    //qDebug() << "Z rotation " << r->z();
    const float K = 5.0f;

    float dphi = -K*(r->x() - m_previous_rotation[0])/ 180.0f * M_PI;
    float dtheta = K*(r->y() - m_previous_rotation[1])/ 180.0f * M_PI;

    float dist = (m_target - m_eye).length();
    QVector3D a = (m_target - m_eye)/dist;
    float phi = atan2f(a[2],a[0]);
    float theta = atan2f(a[1], sqrtf(a[0]*a[0]+a[2]*a[2]));
    phi+= dphi;
    theta+= dtheta;
    theta = std::min( 0.4f*M_PI, std::max(-0.4*M_PI, double(theta)) );
    m_target = QVector3D( cos(phi)*cos(theta), sin(theta), sin(phi)*cos(theta));
    m_previous_rotation = QVector2D(r->x(),r->y());

#endif

}

GLWindow::~GLWindow()
{
    makeCurrent();
    if (m_texture_rgb) delete m_texture_rgb;
    if (m_texture_rgb_empty) delete m_texture_rgb_empty;

    if (m_texture_rgb_norm) delete m_texture_rgb_norm;
    if (m_texture_rgb_emptyr) delete m_texture_rgb_emptyr;

    if (m_texture_depth) delete m_texture_depth;
    if (m_texture_depth_empty) delete m_texture_depth_empty;
    if (m_texture_segmentation) delete m_texture_segmentation;
    if (m_seg_image) delete m_seg_image;
    if (m_program)delete m_program;
    if (m_vbo) delete m_vbo;
    if (m_ibo) delete m_ibo;
    if (m_vao) delete m_vao;
    if (m_cube_vbo) delete m_cube_vbo;
    if (m_cube_ibo) delete m_cube_ibo;
    if (m_cube_vao) delete m_cube_vao;

#ifdef ANDROID
    m_rotation_sensor->stop();
    delete m_rotation_sensor;
#endif
}


QByteArray versionedShaderCode(const QString& filename)
{
    QByteArray versionedSrc;

    if (QOpenGLContext::currentContext()->isOpenGLES())
        versionedSrc.append(QByteArrayLiteral("#version 300 es\n"));
    else
        versionedSrc.append(QByteArrayLiteral("#version 420\n"));


    QFile f(filename);
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray ba = f.readAll();
    qDebug() << "read " << ba.size() << "bytes";

    versionedSrc.append(ba);
    return versionedSrc;
}


void GLWindow::load_scene(const QString& scene_name)
{

    m_rgb_image = scene_name + QString("_full.png");
    m_rgb_image_empty = scene_name + QString("pred.png");
    m_rgb_image_norm = scene_name + QString("normal.png");
    m_rgb_image_emptyr = scene_name + QString("predr.png");

    //initializeTexture(m_texture_rgb,m_rgb_image);
    if (m_texture_rgb) {
        delete m_texture_rgb;
        m_texture_rgb = 0;
    }

    QImage img(m_rgb_image);
    Q_ASSERT(!img.isNull());
    qDebug() << "read image in  " << m_rgb_image;
    m_texture_rgb = new QOpenGLTexture(img.mirrored());



    if (m_texture_rgb_empty) {
        delete m_texture_rgb_empty;
        m_texture_rgb_empty = 0;
    }

    QImage img_empty(m_rgb_image_empty);
    Q_ASSERT(!img.isNull());
    qDebug() << "read image in  " << m_rgb_image_empty;
    m_texture_rgb_empty = new QOpenGLTexture(img_empty.mirrored());





    if (m_texture_rgb_norm) {
        delete m_texture_rgb_norm;
        m_texture_rgb_norm = 0;
    }

    QImage img_norm(m_rgb_image_norm);
    Q_ASSERT(!img.isNull());
    qDebug() << "read image in  " << m_rgb_image_norm;
    m_texture_rgb_norm = new QOpenGLTexture(img_norm.mirrored());





    //************load scene
    if (m_texture_rgb_emptyr) {
        delete m_texture_rgb_empty;
        m_texture_rgb_empty = 0;
    }

    QImage img_emptyr(m_rgb_image_emptyr);
    Q_ASSERT(!img.isNull());
    qDebug() << "read image in  " << m_rgb_image_emptyr;
    m_texture_rgb_emptyr = new QOpenGLTexture(img_empty.mirrored());
    //***************

}

void GLWindow::initializeGL()
{

    m_sphere_map.regenerate(8, true, true);

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    //initializeTexture(m_texture_rgb,m_rgb_image);
    if (m_texture_rgb) {
        delete m_texture_rgb;
        m_texture_rgb = 0;
    }

    QImage img(m_rgb_image);
    Q_ASSERT(!img.isNull());
    qDebug() << "read RGB full image in  " << m_rgb_image;
    m_texture_rgb = new QOpenGLTexture(img.mirrored());

    if (m_texture_rgb_empty) {
        delete m_texture_rgb_empty;
        m_texture_rgb_empty = 0;
    }

    QImage img_empty(m_rgb_image_empty);
    Q_ASSERT(!img_empty.isNull());
    qDebug() << "read RGB norm image in  " << m_rgb_image_empty;
    m_texture_rgb_empty = new QOpenGLTexture(img_empty.mirrored());



    if (m_texture_rgb_norm) {
        delete m_texture_rgb_norm;
        m_texture_rgb_norm = 0;
    }

    QImage img_norm(m_rgb_image_norm);
    Q_ASSERT(!img_norm.isNull());
    qDebug() << "read RGB norm image in  " << m_rgb_image_norm;
    m_texture_rgb_norm = new QOpenGLTexture(img_norm.mirrored());




    //******* initialize
    if (m_texture_rgb_emptyr) {
        delete m_texture_rgb_emptyr;
        m_texture_rgb_emptyr = 0;
    }

    QImage img_emptyr(m_rgb_image_emptyr);
    Q_ASSERT(!img_empty.isNull());
    qDebug() << "read RGB empty image in  " << m_rgb_image_emptyr;
    m_texture_rgb_emptyr = new QOpenGLTexture(img_empty.mirrored());
    //********



    // Depth images
    if (m_texture_depth) {
        delete m_texture_depth;
        m_texture_depth = 0;
    }

    QImage img_depth(m_depth_image);
    Q_ASSERT(!img_depth.isNull());
    qDebug() << "read depth full image in  " << m_depth_image;

    m_texture_depth = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_texture_depth->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

    // given some `width`, `height` and `data_ptr`
    m_texture_depth->setSize(img_depth.width(), img_depth.height(), 1);
    m_texture_depth->setFormat(QOpenGLTexture::D16);
    m_texture_depth->allocateStorage();
    m_texture_depth->setData(QOpenGLTexture::Depth, QOpenGLTexture::UInt16, (const void *)img_depth.mirrored().bits());
    m_texture_depth->create();

    if (m_texture_depth_empty) {
        delete m_texture_depth_empty;
        m_texture_depth_empty = 0;
    }

    QImage img_depth_empty(m_depth_image_empty);
    Q_ASSERT(!img_depth_empty.isNull());
    qDebug() << "read depth empty image in  " << m_depth_image_empty;
    m_texture_depth_empty = new QOpenGLTexture(img_depth_empty.mirrored());

    // Load segmentation image
    if (m_seg_image) {
        delete m_seg_image;
        m_seg_image = 0;
    }

    m_seg_image = new QImage(m_segmentation_image);
    Q_ASSERT(!m_seg_image->isNull());
    qDebug() << "read segmentation image in  " << m_segmentation_image;
    if (m_texture_segmentation) {
        delete m_texture_segmentation;
        m_texture_segmentation = 0;
    }
    m_texture_segmentation = new QOpenGLTexture(m_seg_image->mirrored());


    if (m_program) {
        delete m_program;
        m_program = 0;
    }
    m_program = new QOpenGLShaderProgram;
    // Prepend the correct version directive to the sources. The rest is the
    // same, thanks to the common GLSL syntax.

    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, versionedShaderCode(m_vertex_shader));
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, versionedShaderCode(m_fragment_shader));
    m_program->link();

    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_camMatrixLoc = m_program->uniformLocation("camMatrix");
    m_worldMatrixLoc = m_program->uniformLocation("worldMatrix");
    m_lightPosLoc = m_program->uniformLocation("lightPos");
    m_enableLightingLoc = m_program->uniformLocation("enableLighting");
    m_depthEnabledLoc = m_program->uniformLocation("depthEnabled");
    m_textureEnabledLoc = m_program->uniformLocation("textureEnabled");
    m_depthMaxLoc = m_program->uniformLocation("depthMax");
    m_pickedObjectCount = m_program->uniformLocation("pickedObjectCount");
    m_pickColors = m_program->uniformLocation("pickColor");
    m_blendFactorLoc = m_program->uniformLocation("blendFactor");
    m_depthColorLoc = m_program->uniformLocation("depthColor");

    // Create a VAO. Not strictly required for ES 3, but it is for plain OpenGL.

    if (m_vao) {
        delete m_vao;
        m_vao = 0;
    }
    m_vao = new QOpenGLVertexArrayObject;
    if (m_vao->create())
        m_vao->bind();

    if (m_vbo) {
        delete m_vbo;
        m_vbo = 0;
    }
    m_program->bind();
    m_vbo = new QOpenGLBuffer;
    m_vbo->create();
    m_vbo->bind();
    m_vbo->allocate(m_sphere_map.vertex_buffer(), int( m_sphere_map.vertex_buffer_size() *  sizeof(GLfloat)));
    qDebug() << "after vbo creation";
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glEnableVertexAttribArray(2);
    f->glEnableVertexAttribArray(3);

    // Parameterize with respect to sphere tessellation
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), 0);
    f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                             reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    f->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                             reinterpret_cast<void *>(5 * sizeof(GLfloat)));
    f->glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                             reinterpret_cast<void *>(8 * sizeof(GLfloat)));

    m_ibo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_ibo->create();
    m_ibo->bind();
    m_ibo->allocate(m_sphere_map.index_buffer(), int(m_sphere_map.index_buffer_size() * sizeof(GLint)));
    qDebug() << "after ibo creation";
    m_ibo->release();
    m_vbo->release();

    m_program->release();
    f->glEnable(GL_DEPTH_TEST);
    f->glEnable(GL_CULL_FACE);
    m_vao->release();
    init_cube();
    init_obj();
}

void GLWindow::resizeGL(int w, int h)
{
    m_viewport_width = w;
    m_viewport_height = h;

    m_proj.setToIdentity();
    m_proj.perspective(m_fov, GLfloat(w) / h, m_near, m_far);
}


void GLWindow::pick(const QPoint& p){

    const float pi = 3.14159f;
    if (m_picked_object_count == int(m_pick_colors.size())) {
        qDebug() << "Warning: maximum number of pick objects reached" << endl;
        return;
    }
    // Ray intersection with m_seg_image
    float cx = 0.25f;
    float cy = 0.5f;
    float K = m_fov *  pi / 180.f;
    float dphi = (  p.x()/float(width()) - cx) * 0.5f*float(width())/ float(height()) * K;
    float dtheta = ( p.y()/float(height()) - cy ) * K;
    float dist = (m_target - m_eye).length();
    QVector3D a = (m_target - m_eye)/dist;
    float phi = atan2f(a[2],a[0]);
    float theta = atan2f(a[1], sqrtf(a[0]*a[0]+a[2]*a[2]));
    phi += dphi;
    theta -= dtheta;
    if (phi < -pi) phi += 2.0f*pi;
    if (phi > pi ) phi -= 2.0f*pi;
    if (2.0f * theta < -pi) theta += pi;
    if (2.0f * theta > pi ) theta -= pi;

    qDebug() << "Pick phi,theta " << phi * (180.0f / pi) << " , " << theta * (180.0f / pi);


    int u = int ( (0.5f*phi/pi + 0.5f)* float(m_seg_image->width()));
    int v = int ( (theta/pi + 0.5f)* float(m_seg_image->height()));
    qDebug() << "Pick uv " << u << " , " << v;
    QColor col = m_seg_image->mirrored().pixelColor(u,v);
    QVector3D c(float(col.red())/255.0f,float(col.green()/255.0f),float(col.blue())/255.0f);
    if ( std::find(m_pick_colors.begin(), m_pick_colors.end(), c)  != m_pick_colors.end() ){
        qDebug() << "Warning: object already picked " << endl;
        return;
    }
    m_pick_colors[m_picked_object_count] = c;
    qDebug() << "Msg: picking object " <<  m_picked_object_count << "  with color " << c;
    m_picked_object_count++;
}

void GLWindow::disable_picking() {
  m_picked_object_count = 0;
  for (std::size_t i = 0; i < m_pick_colors.size(); ++i) {
      m_pick_colors[i] = QVector3D(0.0,0.0,0.0);
  }
  qDebug() << " Disable picking " << endl;
}

void GLWindow::draw_scene(const QVector3D& eye, const QVector3D& target, bool empty, bool depth_inset, bool clear ) {

    // Now use QOpenGLExtraFunctions instead of QOpenGLFunctions as we want to
    // do more than what GL(ES) 2.0 offers.
    //qDebug() << "Start paintgl";
    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();

    f->glClearColor(0, 0, 0, 1);
    if (clear) f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(depth_inset) f->glClear(GL_DEPTH_BUFFER_BIT);

#ifndef ANDROID
    if (!QOpenGLContext::currentContext()->isOpenGLES()) {
        if (m_wireframe_enabled ) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
       }
      else {
         glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
         }
    }


    if (!QOpenGLContext::currentContext()->isOpenGLES()) {
        if (m_pointcloud_enabled ) {
            glPointSize(m_point_size);
            glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
       }
      else {
         glPolygonMode( GL_FRONT_AND_BACK, GL_TRIANGLES);
         }
    }

#endif
    QOpenGLTexture* tex = empty ? m_texture_rgb_empty : m_texture_rgb;

    //QOpenGLTexture* tex_norm = norm ? m_texture_rgb_norm : m_texture_rgb;                   //???

    QOpenGLTexture* tex_depth = empty ? m_texture_depth_empty : m_texture_depth;
    m_vao->bind();
    m_program->bind();
    m_vbo->bind();
    m_ibo->bind();

    tex->bind(0);
    tex_depth->bind(1);
    m_texture_segmentation->bind(2);
    m_texture_rgb->bind(3);

    m_world.setToIdentity();
    QMatrix4x4 camera;
    camera.lookAt(eye,  target, QVector3D(0, 1, 0));
    m_program->setUniformValue(m_projMatrixLoc, m_proj);
    m_program->setUniformValue(m_depthEnabledLoc, (m_depth_enabled?1:0));
    m_program->setUniformValue(m_textureEnabledLoc, 1);
    m_program->setUniformValue(m_depthMaxLoc, m_sphere_radius_cm);
    m_program->setUniformValue(m_depthColorLoc, depth_inset ? 1 : 0);
    m_program->setUniformValue(m_camMatrixLoc, camera);
    m_program->setUniformValue(m_worldMatrixLoc, m_world);
    m_program->setUniformValue(m_lightPosLoc, QVector3D(0, 200.0, 0.0f));
    m_program->setUniformValue(m_enableLightingLoc, m_lighting_enabled? 1: 0);

    m_program->setUniformValue(m_pickedObjectCount, m_picked_object_count);
    m_program->setUniformValueArray(m_pickColors, (const GLfloat*)(&m_pick_colors[0][0]), int(m_pick_colors.size())*3, 3);
    m_program->setUniformValue(m_blendFactorLoc, empty ? 1.0f : 0.5f);


    //m_program->setUniformValue(m_blendFactorLoc, norm ? 1.0f : 0.5f);                       //???


    // Now call a function introduced in OpenGL 3.1 / OpenGL ES 3.0. We
    // requested a 3.3 or ES 3.0 context, so we know this will work.

    //glBindVertexArray( vao );
    f->glDrawElements( GL_TRIANGLES, GLsizei( m_sphere_map.index_buffer_size() ), GL_UNSIGNED_INT, nullptr );

    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    //f->glDrawElements( GL_TRIANGLES, m_sphere_map.index_buffer_size(), GL_UNSIGNED_INT, nullptr );

    //glBindVertexArray( 0 );

    tex->release(0);
    tex_depth->release(1);
    m_texture_segmentation->release(2);
    m_texture_rgb->release(3);
    m_ibo->release();
    m_vbo->release();
    m_program->release();
    m_vao->release();

    // The position of the object is a new field, called m_object_position
    if (empty && m_draw_cube_enabled && m_depth_enabled) draw_cube(camera, QVector3D(50.0,-100.0,-400.0f), QVector3D(0.0,1.0,0.0), 0.0f, QVector3D(50.0f, 50.0f, 50.0f) );
    if (empty && m_draw_obj_enabled && m_depth_enabled) draw_obj(camera, m_object_position, QVector3D(0.0,1.0,0.0), 0.0f, QVector3D(1.0f, 1.0f, 1.0f) );

    //f->glDrawArraysInstanced(GL_TRIANGLES, 0, m_logo.vertexCount(), 32 * 36);

}












void GLWindow::draw_scene_norm(const QVector3D& eye, const QVector3D& target, bool norm, bool depth_inset, bool clear ) {

    // Now use QOpenGLExtraFunctions instead of QOpenGLFunctions as we want to
    // do more than what GL(ES) 2.0 offers.
    //qDebug() << "Start paintgl";
    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();

    f->glClearColor(0, 0, 0, 1);
    if (clear) f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(depth_inset) f->glClear(GL_DEPTH_BUFFER_BIT);

#ifndef ANDROID
    if (!QOpenGLContext::currentContext()->isOpenGLES()) {
        if (m_wireframe_enabled ) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
       }
      else {
         glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
         }
    }


    if (!QOpenGLContext::currentContext()->isOpenGLES()) {
        if (m_pointcloud_enabled ) {
            glPointSize(3.0f);
            glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
       }
      else {
         glPolygonMode( GL_FRONT_AND_BACK, GL_TRIANGLES);
         }
    }

#endif
    //QOpenGLTexture* tex = empty ? m_texture_rgb_empty : m_texture_rgb;

    QOpenGLTexture* tex_norm = norm ? m_texture_rgb_norm : m_texture_rgb;                   //???
    QOpenGLTexture* tex_depth =  m_texture_depth;
    m_vao->bind();
    m_program->bind();
    m_vbo->bind();
    m_ibo->bind();

    tex_norm->bind(0);
    tex_depth->bind(1);
    m_texture_segmentation->bind(2);
    m_texture_rgb->bind(3);

    m_world.setToIdentity();
    QMatrix4x4 camera;
    camera.lookAt(eye,  target, QVector3D(0, 1, 0));
    m_program->setUniformValue(m_projMatrixLoc, m_proj);
    m_program->setUniformValue(m_depthEnabledLoc, (m_depth_enabled?1:0));
    m_program->setUniformValue(m_textureEnabledLoc, 1);
    m_program->setUniformValue(m_depthMaxLoc, m_sphere_radius_cm);
    m_program->setUniformValue(m_depthColorLoc, depth_inset ? 1 : 0);
    m_program->setUniformValue(m_camMatrixLoc, camera);
    m_program->setUniformValue(m_worldMatrixLoc, m_world);
    m_program->setUniformValue(m_lightPosLoc, QVector3D(0, 200.0, 0.0f));
    m_program->setUniformValue(m_enableLightingLoc, m_lighting_enabled? 1: 0);

    m_program->setUniformValue(m_pickedObjectCount, m_picked_object_count);
    m_program->setUniformValueArray(m_pickColors, (const GLfloat*)(&m_pick_colors[0][0]), int(m_pick_colors.size())*3, 3);
    //m_program->setUniformValue(m_blendFactorLoc, empty ? 1.0f : 0.5f);


    m_program->setUniformValue(m_blendFactorLoc, norm ? 1.0f : 0.5f);                       //???


    // Now call a function introduced in OpenGL 3.1 / OpenGL ES 3.0. We
    // requested a 3.3 or ES 3.0 context, so we know this will work.

    //glBindVertexArray( vao );
    f->glDrawElements( GL_TRIANGLES, GLsizei( m_sphere_map.index_buffer_size() ), GL_UNSIGNED_INT, nullptr );

    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    //f->glDrawElements( GL_TRIANGLES, m_sphere_map.index_buffer_size(), GL_UNSIGNED_INT, nullptr );

    //glBindVertexArray( 0 );

    //tex->release(0);
    //tex_depth->release(1);
    m_texture_segmentation->release(2);
    m_texture_rgb->release(3);
    m_ibo->release();
    m_vbo->release();
    m_program->release();
    m_vao->release();
    if (norm && m_draw_cube_enabled && m_depth_enabled) draw_cube(camera, QVector3D(50.0,-100.0,-400.0f), QVector3D(0.0,1.0,0.0), 0.0f, QVector3D(50.0f, 50.0f, 50.0f) );
    if (norm && m_draw_obj_enabled && m_depth_enabled) draw_obj(camera, QVector3D(0.0,-100.0,-450.0f), QVector3D(0.0,1.0,0.0), 0.0f, QVector3D(100.0f, 100.0f, 100.0f) );

    //f->glDrawArraysInstanced(GL_TRIANGLES, 0, m_logo.vertexCount(), 32 * 36);

}



void GLWindow::init_obj(const QVector3D& c) {
    std::vector<float> v;
    std::vector<float> vn;
    std::vector<int> idx;

    std::vector<float> vert;
    import_obj(m_obj_file, v,vn,idx);

    qDebug() << " Obj -- vert: " << int(v.size())/3 << " , tri: " << int(idx.size())/3;
    for (std::size_t i = 0; i < v.size(); i+=3) {
       // vert
       for (std::size_t j = 0; j < 3; ++j) {
         vert.push_back(v[i+j]);
       }
       // tex coors
       vert.push_back( 0.0f);
       vert.push_back( 0.0f);
       // normal
       for (std::size_t j = 0; j < 3; ++j) {
         vert.push_back(vn[i+j]);
       }
       // color
       for (std::size_t j = 0; j < 3; ++j) {
         vert.push_back(c[j]);
       }
    }

    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();

    if (m_obj_vao) {
        delete m_obj_vao;
        m_obj_vao = 0;
    }
    m_obj_vao = new QOpenGLVertexArrayObject;
    if (m_obj_vao->create())
        m_obj_vao->bind();

    m_program->bind();
    m_obj_vbo = new QOpenGLBuffer;
    m_obj_vbo->create();
    m_obj_vbo->bind();
    m_obj_vbo->allocate(&vert[0], int( vert.size() *  sizeof(GLfloat)));

    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glEnableVertexAttribArray(2);
    f->glEnableVertexAttribArray(3);

    // Parameterize with respect to sphere tessellation
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), 0);
    f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                             reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    f->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                             reinterpret_cast<void *>(5 * sizeof(GLfloat)));
    f->glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                             reinterpret_cast<void *>(8 * sizeof(GLfloat)));


    qDebug() << "after obj vbo creation";

    m_obj_ibo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_obj_ibo->create();
    m_obj_ibo->bind();
    m_obj_ibo->allocate(&idx[0], int(idx.size() * sizeof(GLint)));
    m_obj_index_count = idx.size();
    qDebug() << "after obj ibo creation";
    m_obj_ibo->release();
    m_obj_vbo->release();

    m_program->release();
    m_obj_vao->release();

}


// Read obj file containing meshes

void GLWindow::import_obj(const QString &fname, std::vector<float>& v, std::vector<float>& vn,
                          std::vector<int>& idx) {

    v.clear();
    vn.clear();
    idx.clear();

    QFile f(fname);
    f.open(QIODevice::ReadOnly);
    if (!f.isOpen())
        return;

    QTextStream stream(&f);
    for (QString line = stream.readLine();  !line.isNull();
         line = stream.readLine()) {
        /* process information */
        if (line.startsWith("#")) { continue; } // Skipping comments
        else if (line.startsWith("v ")) {
            QStringList s = line.split(" ");
            v.push_back( s[1].toFloat() );
            v.push_back( s[2].toFloat() );
            v.push_back( s[3].toFloat() );
        } else if (line.startsWith("vn ")) {
            QStringList s = line.split(" ");
            vn.push_back( s[1].toFloat() );
            vn.push_back( s[2].toFloat() );
            vn.push_back( s[3].toFloat() );
        } else if (line.startsWith("f ")){
            QStringList s = line.split(" ");
            for (int i = 1; i < s.size(); ++i) {
                QStringList fattr = s[i].split("//");
                idx.push_back(fattr[0].toInt()-1);
            }
        }
    };
    f.close();
    qDebug() << " Imported obj file " << m_obj_file;
}





void GLWindow::init_cube() {

    // Variables for the cube data
    const float K = 1.0f/sqrtf(3.0f);

    float _vertex[] = { -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, -K, -K, K, 1.0f,   0,   0,
                         1.0f, -1.0f, 1.0f, 0.0f, 0.0f, K, -K, K, 1.0f, 1.0f,   0,
                         1.0f, 1.0f, 1.0f, 0.0f, 0.0f, K, K, K, 0, 1.0f,   0,
                        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -K, K, K, 0, 1.0f,   0,
                        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -K, -K, -K,  0,   0, 1.0f,
                        1.0f, -1.0f, -1.0f, 0.0f, 0.0f, K, -K, -K, 1.0f,   0,   0,
                        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, K, K, -K, 1.0f, 1.0f,   0,
                        -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -K, K, -K, 0, 1.0f,   0
                      };


    // Element Indicies for the Cube
    unsigned int _index[] =
    {
        0, 1, 2, 2, 3, 0,
        3, 2, 6, 6, 7, 3,
        7, 6, 5, 5, 4, 7,
        4, 0, 3, 3, 7, 4,
        0, 1, 5, 5, 4, 0,
        1, 5, 6, 6, 2, 1
    };
    const unsigned _indexSize = 36;
    const unsigned _vertexSize = 88;


    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();

    if (m_cube_vao) {
        delete m_cube_vao;
        m_cube_vao = 0;
    }
    m_cube_vao = new QOpenGLVertexArrayObject;
    if (m_cube_vao->create())
        m_cube_vao->bind();



    m_program->bind();
    m_cube_vbo = new QOpenGLBuffer;
    m_cube_vbo->create();
    m_cube_vbo->bind();
    m_cube_vbo->allocate(_vertex, int( _vertexSize *  sizeof(GLfloat)));

    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glEnableVertexAttribArray(2);
    f->glEnableVertexAttribArray(3);

    // Parameterize with respect to sphere tessellation
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), 0);
    f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                             reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    f->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                             reinterpret_cast<void *>(5 * sizeof(GLfloat)));
    f->glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                             reinterpret_cast<void *>(8 * sizeof(GLfloat)));


    qDebug() << "after cube vbo creation";

    m_cube_ibo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_cube_ibo->create();
    m_cube_ibo->bind();
    m_cube_ibo->allocate(_index, int(_indexSize * sizeof(GLint)));
    qDebug() << "after cube ibo creation";
    m_cube_ibo->release();
    m_cube_vbo->release();

    m_program->release();
    m_cube_vao->release();
}

void GLWindow::draw_cube(const QMatrix4x4& camera, const QVector3D& pos, const QVector3D& rot_axis, float rot_angle,  const QVector3D& scale) {

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    m_cube_vao->bind();
    m_program->bind();
    // bind buffer storing positions
    m_cube_vbo->bind();
    m_cube_ibo->bind();


    m_world.setToIdentity();

    //model.rotate(rot_angle, rot_axis);
    m_world.translate(pos);
    m_world.scale(scale);

    m_program->setUniformValue(m_textureEnabledLoc, 0);
    m_program->setUniformValue(m_depthEnabledLoc, 0);
    m_program->setUniformValue(m_worldMatrixLoc, m_world);
    m_program->setUniformValue(m_enableLightingLoc , 1);

    f->glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr );


    m_cube_ibo->release();
    m_cube_vbo->release();
    m_world.setToIdentity();
    m_program->release();
    m_cube_vao->release();
}


void GLWindow::draw_obj(const QMatrix4x4& camera, const QVector3D& pos, const QVector3D& rot_axis, float rot_angle,  const QVector3D& scale) {

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    m_obj_vao->bind();
    m_program->bind();
    // bind buffer storing positions
    m_obj_vbo->bind();
    m_obj_ibo->bind();


    m_world.setToIdentity();

    //model.rotate(rot_angle, rot_axis);
    m_world.translate(pos);
    m_world.scale(scale);

    m_program->setUniformValue(m_textureEnabledLoc, 0);
    m_program->setUniformValue(m_depthEnabledLoc, 0);
    m_program->setUniformValue(m_worldMatrixLoc, m_world);
    m_program->setUniformValue(m_enableLightingLoc , 1);

    f->glDrawElements( GL_TRIANGLES, m_obj_index_count, GL_UNSIGNED_INT, nullptr );


    m_obj_ibo->release();
    m_obj_vbo->release();
    m_world.setToIdentity();
    m_program->release();
    m_obj_vao->release();
}


void GLWindow::paintGL()
{
    if (m_view_animation.active()) {
        m_eye = m_view_animation.current_eye();
        m_target = m_view_animation.current_target();
    }
    m_proj.setToIdentity();
    if (! m_stereo_enabled) {
        if (m_mixed_view_enabled) {
           m_proj.perspective(m_fov, 0.5f * m_viewport_width / m_viewport_height , m_near, m_far);
           draw_mixed_view();
        }  else {
           glViewport(0,0,m_viewport_width,m_viewport_height);
           m_proj.setToIdentity();
           m_proj.perspective(m_fov,  m_viewport_width / m_viewport_height , m_near, m_far);
           //QOpenGLTexture* tex = m_draw_empty_enabled ? m_texture_rgb_empty : m_texture_rgb;
           //QOpenGLTexture* tex_depth = m_draw_empty_enabled ? m_texture_depth_empty : m_texture_depth;
           draw_scene(m_eye,m_target, m_draw_empty_enabled, false, true);
        }
    } else if (! m_stereo_norm_enabled) {
        if (m_mixed_view_norm_enabled) {
           m_proj.perspective(m_fov, 0.5f * m_viewport_width / m_viewport_height , m_near, m_far);
           draw_mixed_view_norm();
        }  else {
           glViewport(0,0,m_viewport_width,m_viewport_height);
           m_proj.setToIdentity();
           m_proj.perspective(m_fov,  m_viewport_width / m_viewport_height , m_near, m_far);
           //QOpenGLTexture* tex = m_draw_empty_enabled ? m_texture_rgb_empty : m_texture_rgb;
           //QOpenGLTexture* tex_depth = m_draw_empty_enabled ? m_texture_depth_empty : m_texture_depth;
           draw_scene_norm(m_eye,m_target, m_draw_norm_enabled, false, true);
        }
    } else {
        m_proj.perspective(m_fov, 0.5f * m_viewport_width / m_viewport_height , m_near, m_far);
        draw_stereo();
    }
}



void GLWindow::reset_camera() {

    // add to state
    m_eye = QVector3D(0.0,0.0,0.0);
    m_target = QVector3D(0.0,0.0,-1.0);
}


void GLWindow::mousePressEvent(QMouseEvent *ev) {

   if (ev->button() == Qt::LeftButton) {
        if ( m_mixed_view_enabled && ev->modifiers() == Qt::ControlModifier) {
           pick(ev->pos());
        } else {
           // if the position is inside the inset
           // we set a new flag that is moving_object
           int x  = ev->pos().x();
           int y  = ev->pos().y();
           if (m_depth_inset_enabled && x > (3 * m_viewport_width / 4) && y < m_viewport_height /2 ) {
               m_object_moving = true;
           } else {
               m_mouse_pressed = true;
           }
           m_mouse_start = ev->pos();
       }
   }
   if (ev->button() == Qt::RightButton && m_mixed_view_enabled)  {
       disable_picking();
   }
}


void GLWindow::mouseReleaseEvent(QMouseEvent* ev) {
   if (ev->button() == Qt::LeftButton) {
        m_mouse_pressed = false;
        m_object_moving = false;
    }
}


void GLWindow::mouseMoveEvent(QMouseEvent* ev) {


    // if moving object
    // ev conatins the new pos of the mouse
    // you can compute the difference
    // QPoint delta = ev->pos() - m_mouse_start;
    // this becomes a displacement for the object
    // m_object_position += QVector3D(delta[0],delta[1],0.0f);

    if (m_object_moving) {
        const float K = 0.005f;
        QPoint delta = ev->pos() - m_mouse_start;
        m_object_position += K * QVector3D( delta.x(), 0.0f, delta.y());
        qDebug() << "Object position: " << m_object_position;
    }

    if (m_mouse_pressed) {
        const float pi = 3.1415f;
        const float K = 0.00001f;
        QPoint delta = ev->pos() - m_mouse_start;
        float dphi = K * delta.x();
        float dtheta = K * delta.y();
        float dist = (m_target - m_eye).length();
        QVector3D a = (m_target - m_eye)/dist;
        float phi = atan2f(a[2],a[0]);
        float theta = atan2f(a[1], sqrtf(a[0]*a[0]+a[2]*a[2]));
        phi-= dphi;
        theta+= dtheta;

        theta = std::min( 0.4f*pi, std::max(-0.4f*pi, theta) );

        a = QVector3D( cos(phi)*cos(theta), sin(theta), sin(phi)*cos(theta));
        m_target = a;
        m_eye = m_target - dist * a;
        //qDebug() << "target" << m_target;
    }
}

void GLWindow::wheelEvent(QWheelEvent* ev) {
    // Zooming operation
    float K = 0.0001f;
    if ( m_depth_enabled) K *= 10.0f;
    float delta = K * ev->angleDelta().y();
    float dist = (m_target-m_eye).length();
    dist = std::max(1.0f, std::min(0.3f, dist+delta));
    m_eye += (m_target - m_eye) * delta;
    //qDebug() << "delta" << delta;
}

void GLWindow::keyPressEvent(QKeyEvent *ev) {
    switch (ev->key()) {
        case Qt::Key_K: {
            m_view_animation.push_key_frame( m_eye, m_target);
            qDebug() << " Pushed key frame with eye = " << m_eye << " and target " << m_target;
    } break;
    case Qt::Key_D: {
        m_depth_enabled = !m_depth_enabled;
        qDebug() << " Depth " << (m_depth_enabled ? " enabled " : " disabled ");
    } break;
    case Qt::Key_I: {
        m_depth_inset_enabled = !m_depth_inset_enabled;
        qDebug() << " Depth inset " << (m_depth_inset_enabled ? " enabled " : " disabled ");
    } break;
    case Qt::Key_C: {
        m_draw_cube_enabled = !m_draw_cube_enabled;
        qDebug() << " Draw Cube " << (m_draw_cube_enabled ? " enabled " : " disabled ");
    } break;
    case Qt::Key_O: {
        m_draw_obj_enabled = !m_draw_obj_enabled;
        qDebug() << " Draw Obj " << (m_draw_obj_enabled ? " enabled " : " disabled ");
    } break;
    case Qt::Key_B: {
        m_lighting_enabled = !m_lighting_enabled;
        qDebug() << " Lighting " << (m_lighting_enabled ? " enabled " : " disabled ");
    } break;

    case Qt::Key_T: {
            m_view_animation.duration() = ( m_view_animation.duration() + 10000 ) % 70000;
            qDebug() << " Animation duration = " << m_view_animation.duration();
    } break;

    case Qt::Key_A: {
        if ( m_view_animation.active() ) m_view_animation.stop();
        else m_view_animation.start();
        qDebug() << " Animation " << (m_view_animation.active() ? " started " : " stopped ");
    } break;
        case Qt::Key_P: {
        if ( m_view_animation.active() ) m_view_animation.pause();
        else m_view_animation.resume();
        qDebug() << " Animation " << (m_view_animation.active() ? " resumed " : " paused ");
    } break;
    case Qt::Key_M: {
        m_mixed_view_enabled = !m_mixed_view_enabled;
        qDebug() << " Mixed view " << (m_mixed_view_enabled ? " enabled " : " disabled ");

    } break;


    case Qt::Key_V: {
        m_mixed_view_norm_enabled = !m_mixed_view_norm_enabled;
        qDebug() << " Mixed view " << (m_mixed_view_norm_enabled ? " enabled " : " disabled ");

    } break;


    case Qt::Key_L: {
        m_current_scene = (m_current_scene + 1) % m_scene_list.size();
        load_scene(m_scene_list[m_current_scene]);
        qDebug() << " Loaded scene " << m_scene_list[m_current_scene];
    } break;
        case Qt::Key_R: {
        m_view_animation.reset();
        qDebug() << " Animation reset";
    } break;
        case Qt::Key_S: {
           m_stereo_enabled = !m_stereo_enabled;
        } break;

     case Qt::Key_Greater: {
       m_point_size += 1.0f;
       qDebug() << " Current point size: " << m_point_size;
       break;
    }
    case Qt::Key_Less: {
      m_point_size -= 1.0f;
      qDebug() << " Current point size: " << m_point_size;
      break;
   }

        case Qt::Key_X: {
           m_stereo_norm_enabled = !m_stereo_norm_enabled;
        } break;


        case Qt::Key_W: {
          m_wireframe_enabled = !m_wireframe_enabled;
        } break;

        case Qt::Key_F: {
            m_pointcloud_enabled = !m_pointcloud_enabled;
        } break;

        case Qt::Key_E: {
          m_draw_empty_enabled = !m_draw_empty_enabled;
        } break;

        case Qt::Key_N: {
            m_draw_norm_enabled = !m_draw_norm_enabled;
        } break;
    case Qt::Key_Home: {
        reset_camera();
    } break;
        case Qt::Key_Escape: {
            exit(0);
        } break;
     }
}

void GLWindow::draw_stereo() {
    // Separate viewport for the two eyes
    float delta = 0.5f * m_eye_separation_cm / m_sphere_radius_cm;
    QVector3D up = QVector3D(0.0,1.0,0.0);
    QVector3D eye_axis  = QVector3D::crossProduct(m_target - m_eye, up ).normalized();

    QVector3D eye_left = m_eye - delta * eye_axis;

    glViewport(0,0,m_viewport_width/2, m_viewport_height);
    draw_scene(eye_left,m_target, m_draw_empty_enabled, false, true);

    QVector3D eye_right = m_eye + delta * eye_axis;
    glViewport(m_viewport_width/2,0, m_viewport_width/2, m_viewport_height);
    draw_scene(eye_right,m_target, m_draw_empty_enabled, false, false);
}



void GLWindow::draw_stereo_norm() {
    // Separate viewport for the two eyes
    float delta = 0.5f * m_eye_separation_cm / m_sphere_radius_cm;
    QVector3D up = QVector3D(0.0,1.0,0.0);
    QVector3D eye_axis  = QVector3D::crossProduct(m_target - m_eye, up ).normalized();

    QVector3D eye_left = m_eye - delta * eye_axis;

    glViewport(0,0,m_viewport_width/2, m_viewport_height);
    draw_scene_norm(eye_left,m_target, m_draw_norm_enabled, false, true);

    QVector3D eye_right = m_eye + delta * eye_axis;
    glViewport(m_viewport_width/2,0, m_viewport_width/2, m_viewport_height);
    draw_scene_norm(eye_right,m_target, m_draw_norm_enabled, false, false);
}






void GLWindow::draw_mixed_view() {
    // Separate viewport for the two eyes
    glViewport(0,0,m_viewport_width/2, m_viewport_height);
    draw_scene(m_eye,m_target, false, false, true);

    glViewport(m_viewport_width/2,0, m_viewport_width/2, m_viewport_height);
    draw_scene(m_eye,m_target, true, false, false);
    if (m_depth_inset_enabled) {
        //glViewport(m_viewport_width/4,m_viewport_height/2,m_viewport_width/4, m_viewport_height/2);
        //tdraw_scene(m_eye,m_target, false, true, false);
        glViewport(3 * m_viewport_width/4,m_viewport_height/2,m_viewport_width/4, m_viewport_height/2);
        QVector3D from_ceiling = QVector3D(3.68022, 687.653, 223.43);
        QVector3D to_floor = QVector3D(-0.00508986, -0.951045, -0.30901);
        draw_scene(from_ceiling,to_floor, true, true, false);
    }
}



void GLWindow::draw_mixed_view_norm() {
    // Separate viewport for the two eyes
    glViewport(0,0,m_viewport_width/2, m_viewport_height);
    draw_scene_norm(m_eye,m_target, false, false, true);

    glViewport(m_viewport_width/2,0, m_viewport_width/2, m_viewport_height);
    draw_scene_norm(m_eye,m_target, true, false, false);
    if (m_depth_inset_enabled) {
        //glViewport(m_viewport_width/4,m_viewport_height/2,m_viewport_width/4, m_viewport_height/2);
        //tdraw_scene_norm(m_eye,m_target, false, true, false);
        glViewport(3 * m_viewport_width/4,m_viewport_height/2,m_viewport_width/4, m_viewport_height/2);
        draw_scene_norm(m_eye,m_target, true, true, false);
    }
    // Moving enabled flag
    // Draw another viewport on the left
    // glViewport(0,0,   m_viewport_width/4, m_viewport_height/2);
    // draw_scene_from_above --> draw_scene_norm( QVector3D(0.0,1.0,0.0), QVector3D(0.0,0.0,0.0), rest of flags)
}
