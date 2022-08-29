#ifndef TEXTURED_SPHERE_MAP_H
#define TEXTURED_SPHERE_MAP_H

#include <qopengl.h>
#include <QVector>

// Based on subdivision sphere

class textured_sphere_map
{
public:
    textured_sphere_map();
    void regenerate(std::size_t subdivision, bool normals_enabled = false, bool colors_enabled = false);

public:
    const GLfloat *vertex_buffer() const { return vertex_buffer_.constData(); }
    const std::size_t& vertex_buffer_size() const { return vertex_buffer_size_; }
    std::size_t vertex_count() const { return vertex_buffer_size_/vertex_size_; }

    const GLint * index_buffer() const { return index_buffer_.constData(); }
    const std::size_t& index_buffer_size() const { return index_buffer_size_; }

protected:
    QVector<GLfloat> vertex_buffer_;
    std::size_t vertex_size_;
    std::size_t vertex_buffer_size_;

    QVector<GLint> index_buffer_;
    std::size_t index_buffer_size_;

};

#endif // TEXTURED_SPHERE_MAP_H
