#ifndef SUBDIVISION_SPHERE_H
#define SUBDIVISION_SPHERE_H

#include <QVector3D>


//#include <sl/fixed_size_point.hpp>
//#include <sl/fixed_size_vector.hpp>
//#include <sl/triangle_mesh.hpp>

class triangle {

public:
    triangle(std::size_t i0, std::size_t i1, std::size_t i2){
        indices[0] = i0;
        indices[1] = i1;
        indices[2] = i2;
    }
    const std::size_t& operator[]  (std::size_t i) const {
        return indices[i];
    }
    std::size_t& operator[]  (std::size_t i) {
            return indices[i];
    }
protected:

    std::size_t indices[3];

};


//struct vdata_t {};
//struct edata_t {};
//struct tdata_t {};

// Computes a unit sphere with recursive edge subdivision
// starting from icosahedron basic shape and subdividing each triangle in
// four triangles
// Outputs a vertex map containing normalized coordinates
// and a triangle list

class subdivision_sphere
{

public:
    typedef QVector3D point_t;
    typedef QVector3D vector_t;
    //typedef sl::triangle_mesh<vdata_t, edata_t, tdata_t> tmesh_t;
    typedef std::map<std::size_t, point_t> point_map_t;
    typedef triangle triangle_t;
    typedef std::vector<triangle_t> triangle_vector_t;

public:
    subdivision_sphere(std::size_t depth =4);

public:
    const point_map_t& vertex_map() { return vertex_map_;}
    const std::vector<triangle_t>& triangle_array() { return triangle_array_;}

protected:
    static void subdivide(const std::size_t &i1, const std::size_t &i2, const std::size_t& i3,
                          point_map_t &sphere_points, triangle_vector_t& sphere_triangles,
                          const unsigned int depth);
    static void tessellate_sphere(point_map_t &sphere_points,
                                  triangle_vector_t& sphere_triangles,
                                  const unsigned int depth);
protected:
     point_map_t vertex_map_;
     triangle_vector_t triangle_array_;
};

#endif // SUBDIVISION_SPHERE_H
