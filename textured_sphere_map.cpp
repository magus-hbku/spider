#include "textured_sphere_map.h"
#include "subdivision_sphere.h"

#ifndef M_PI
#define M_PI 3.1415
#endif

#include <math.h>


textured_sphere_map::textured_sphere_map() {
    // vertex coordinates plus texture coordinates
    vertex_size_ = 5;
    vertex_buffer_size_ = 0;
    index_buffer_size_ = 0;

    vertex_buffer_.clear();
    index_buffer_.clear();
}

void textured_sphere_map::regenerate(std::size_t subdivision,
                                      bool normals_enabled,  bool colors_enabled) {

    typedef subdivision_sphere::point_map_t point_map_t;
    typedef subdivision_sphere::triangle_vector_t triangle_vector_t;

    // vertex coordinates plus texture coordinates plus normal coordinates
    if (normals_enabled) vertex_size_ = 8;
    if (normals_enabled && colors_enabled) vertex_size_ = 11;

    // Create unit subdivision sphere
    subdivision_sphere s(subdivision);
    vertex_buffer_size_ = s.vertex_map().size()*vertex_size_;
    index_buffer_size_ = s.triangle_array().size()*3;
    vertex_buffer_.resize(int(vertex_buffer_size_));
    index_buffer_.resize(int(index_buffer_size_));

    // Fill vertex buffer
    for (point_map_t::const_iterator it = s.vertex_map().begin(); it != s.vertex_map().end(); ++it ) {
        std::size_t index = it->first;
        GLfloat *p = vertex_buffer_.data() + index * vertex_size_;
        subdivision_sphere::point_t v = it->second;
        *p++ = v[0];
        *p++ = v[1];
        *p++ = v[2];
        *p++ = 0.5f + 0.5f*atan2f(v[2],v[0])/M_PI; // s tex coord
        *p++ = 0.5f + atan2f(v[1],sqrtf(v[0]*v[0]+v[2]*v[2]))/M_PI; // t tex coord
        if (normals_enabled) {
            *p++ = -v[0];
            *p++ = -v[1];
            *p++ = -v[2];
        }
        if (normals_enabled && colors_enabled) {
            *p++ = 1.0f;
            *p++ = 1.0f;
            *p++ = 1.0f;
        }
    }
    // Fill index buffer
    GLint* iptr = index_buffer_.data();
    for (triangle_vector_t::const_iterator it = s.triangle_array().begin();
         it != s.triangle_array().end(); ++it ) {
        const subdivision_sphere::triangle_t& t  = *it;
        *iptr++ = GLint(t[0]);
        *iptr++ = GLint(t[1]);
        *iptr++ = GLint(t[2]);
    }

}
