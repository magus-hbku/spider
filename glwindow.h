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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWindow>
#include <QMatrix4x4>
#include <QVector3D>
#include <QVector2D>
#include "view_animation.h"

#include "textured_sphere_map.h"

#ifdef ANDROID
#include <QRotationSensor>
#endif

QT_BEGIN_NAMESPACE

class QOpenGLTexture;
class QOpenGLShaderProgram;
class QOpenGLBuffer;
class QOpenGLVertexArrayObject;


QT_END_NAMESPACE

class GLWindow : public QOpenGLWindow
{
    Q_OBJECT


public:
    GLWindow();
    ~GLWindow();

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

protected:
    // Interaction
     void mousePressEvent(QMouseEvent *ev) override;
     void mouseReleaseEvent(QMouseEvent *ev) override;
     void mouseMoveEvent(QMouseEvent *ev) override;
     void wheelEvent(QWheelEvent* ev) override;
     void keyPressEvent(QKeyEvent *ev) override;

     void pick(const QPoint& p);
     void disable_picking();

     void reset_camera();
     void draw_stereo();
     void draw_stereo_norm();
     void draw_scene(const QVector3D& eye, const QVector3D& target, bool depth_inset =  false, bool empty = false, bool clear=true);
     void draw_scene_norm(const QVector3D& eye, const QVector3D& target, bool depth_inset =  false, bool norm = false, bool clear=true);
     void draw_mixed_view();
     void draw_mixed_view_norm();

     //void take_screenshot();
     //void render_text(const QString& text, int x = 0, int y = 20, const QColor& c = Qt::white, const QFont& font = QFont("Helvetica",20));
    void load_scene(const QString& scene_name);

    void draw_cube(const QMatrix4x4& camera, const QVector3D& pos = QVector3D(0.0f,0.0f,-0.75f), const QVector3D& rot_axis  = QVector3D(0.0f,1.0f, 0.0f), float rot_angle = 0.0f, const QVector3D& scale = QVector3D(0.05f, 0.05f, 0.05f) );
    void init_cube();

    void import_obj(const QString& fname, std::vector<float>& v, std::vector<float>& vn, std::vector<int>& idx);
    void init_obj(const QVector3D& c = QVector3D(0.8f, 0.3f, 0.5f));
    void draw_obj(const QMatrix4x4& camera, const QVector3D& pos = QVector3D(0.0f,0.0f,-0.75f), const QVector3D& rot_axis  = QVector3D(0.0f,1.0f, 0.0f), float rot_angle = 0.0f, const QVector3D& scale = QVector3D(0.05f, 0.05f, 0.05f) );

private slots:

     void checkReading();
protected:

    QString m_vertex_shader;
    QString m_fragment_shader;
    QString m_rgb_image;
    QString m_rgb_image_empty;

    QString m_rgb_image_norm;
    QString m_rgb_image_norm_empty;

    QString m_rgb_image_emptyr;
    QString m_depth_image;
    QString m_depth_image_empty;
    QString m_segmentation_image;

    QString m_obj_file;

    textured_sphere_map  m_sphere_map;

    bool m_mouse_pressed;
    bool m_object_moving;
    QPoint m_mouse_start;
    QVector3D m_object_position;

    // Environment
    bool m_stereo_enabled;
    bool m_stereo_norm_enabled;
    bool m_lighting_enabled;
    bool m_wireframe_enabled;
    bool m_pointcloud_enabled;
    bool m_draw_empty_enabled;

    bool m_draw_norm_enabled;

    bool m_mixed_view_enabled;
    bool m_mixed_view_norm_enabled;
    bool m_depth_enabled;
    bool m_normal_enabled;
    bool m_draw_cube_enabled;
    bool m_draw_obj_enabled;
    bool m_depth_inset_enabled;


    float m_eye_separation_cm;
    float m_sphere_radius_cm;

    int m_viewport_width;
    int m_viewport_height;

    float m_fov;
    float m_near;
    float m_far;


#ifdef ANDROID
    QRotationSensor* m_rotation_sensor;
    QVector2D m_previous_rotation;
#endif
    view_animation m_view_animation;

    QImage* m_seg_image;
    std::vector<QVector3D> m_pick_colors;
    int m_picked_object_count;

    QOpenGLTexture *m_texture_rgb;
    QOpenGLTexture *m_texture_rgb_empty;

    QOpenGLTexture *m_texture_rgb_norm;
    QOpenGLTexture *m_texture_rgb_norm_empty;


    QOpenGLTexture *m_texture_depth;
    QOpenGLTexture *m_texture_depth_empty;
    QOpenGLTexture *m_texture_segmentation;


    QOpenGLShaderProgram *m_program;

    QOpenGLBuffer *m_vbo;
    QOpenGLBuffer *m_ibo;
    QOpenGLVertexArrayObject *m_vao;

    QOpenGLBuffer *m_cube_vbo;
    QOpenGLBuffer *m_cube_ibo;
    QOpenGLVertexArrayObject *m_cube_vao;

    QOpenGLBuffer *m_obj_vbo;
    QOpenGLBuffer *m_obj_ibo;
    QOpenGLVertexArrayObject *m_obj_vao;
    std::size_t m_obj_index_count;

    float m_point_size;

    int m_projMatrixLoc;
    int m_camMatrixLoc;
    int m_worldMatrixLoc;
    int m_lightPosLoc;
    int m_enableLightingLoc;
    int m_fillLoc;
    int m_depthEnabledLoc;
    int m_textureEnabledLoc;
    int m_blendFactorLoc;
    int m_normalEnabledLoc;

    int m_depthMaxLoc;
    int m_pickedObjectCount;
    int m_pickColors;

    QMatrix4x4 m_proj;
    QMatrix4x4 m_world;
    QVector3D m_eye;
    QVector3D m_target;

    std::vector<QString> m_scene_list;
    int m_current_scene;

};

#endif
