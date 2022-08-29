#include "subdivision_sphere.h"
#include <map>
#include <vector>


void subdivision_sphere::subdivide(const std::size_t &i1, const std::size_t &i2, const std::size_t& i3,
                                   point_map_t &sphere_points,
                                   triangle_vector_t& sphere_triangles,
                                   const unsigned int depth) {

    typedef triangle_t tri_t;

    point_t v1 = sphere_points[i1];
    point_t v2 = sphere_points[i2];
    point_t v3 = sphere_points[i3];

    const vector_t v12 = ((v1 - point_t()) + (v2 - point_t())).normalized();
    const vector_t v23 = ((v2 - point_t()) + (v3 - point_t())).normalized();
    const vector_t v31 = ((v3 - point_t()) + (v1 - point_t())).normalized();

    std::size_t i12 = sphere_points.size();
    std::size_t i23 = i12 + 1;
    std::size_t i31 = i23 + 1;
    sphere_points[i12] = v12;
    sphere_points[i23] = v23;
    sphere_points[i31] = v31;

    if (depth == 1) {
        sphere_triangles.push_back(tri_t(i1, i12, i31));
        sphere_triangles.push_back(tri_t(i2, i23, i12));
        sphere_triangles.push_back(tri_t(i3, i31, i23));
        sphere_triangles.push_back(tri_t(i12, i23, i31));
        return;
    }

    subdivide(i1, i12, i31, sphere_points, sphere_triangles, depth - 1);
    subdivide(i2, i23, i12, sphere_points, sphere_triangles, depth - 1);
    subdivide(i3, i31, i23, sphere_points, sphere_triangles, depth - 1);
    subdivide(i12, i23, i31, sphere_points, sphere_triangles, depth - 1);
}

void subdivision_sphere::tessellate_sphere(point_map_t &sphere_points,  triangle_vector_t& sphere_triangles,
                                           const unsigned int depth) {

    typedef triangle_t tri_t;

    const double X = 0.525731112119133606;
    const double Z = 0.850650808352039932;

    sphere_points[0] = point_t(-X, 0.0, Z);
    sphere_points[1] = point_t(X, 0.0, Z);
    sphere_points[2] = point_t(-X, 0.0, -Z);
    sphere_points[3] = point_t(X, 0.0, -Z);
    sphere_points[4] = point_t(0.0, Z, X);
    sphere_points[5] = point_t(0.0, Z, -X);
    sphere_points[6] = point_t(0.0, -Z, X);
    sphere_points[7] = point_t(0.0, -Z, -X);
    sphere_points[8] = point_t(Z, X, 0.0);
    sphere_points[9] = point_t(-Z, X, 0.0);
    sphere_points[10] = point_t(Z, -X, 0.0);
    sphere_points[11] = point_t(-Z, -X, 0.0);

    std::vector<tri_t> tdata;
    tdata.push_back(tri_t(0, 4, 1));
    tdata.push_back(tri_t(0, 9, 4));
    tdata.push_back(tri_t(9, 5, 4));
    tdata.push_back(tri_t(4, 5, 8));
    tdata.push_back(tri_t(4, 8, 1));
    tdata.push_back(tri_t(8, 10, 1));
    tdata.push_back(tri_t(8, 3, 10));
    tdata.push_back(tri_t(5, 3, 8));
    tdata.push_back(tri_t(5, 2, 3));
    tdata.push_back(tri_t(2, 7, 3));
    tdata.push_back(tri_t(7, 10, 3));
    tdata.push_back(tri_t(7, 6, 10));
    tdata.push_back(tri_t(7, 11, 6));
    tdata.push_back(tri_t(11, 0, 6));
    tdata.push_back(tri_t(0, 1, 6));
    tdata.push_back(tri_t(6, 1, 10));
    tdata.push_back(tri_t(9, 0, 11));
    tdata.push_back(tri_t(9, 11, 2));
    tdata.push_back(tri_t(9, 2, 5));
    tdata.push_back(tri_t(7, 2, 11));

    for (std::size_t i = 0; i < tdata.size(); i++) {
        subdivide(tdata[i][0], tdata[i][1], tdata[i][2], sphere_points, sphere_triangles, depth);
    }
    //std::cerr << "Geodesic sphere: #verts = " << sphere_points.size() << " ; #tris = " << sphere_triangles.size() << std::endl;
}



subdivision_sphere::subdivision_sphere(std::size_t depth)
{
    vertex_map_.clear();
    triangle_array_.clear();
    tessellate_sphere(vertex_map_,triangle_array_,depth);
}
