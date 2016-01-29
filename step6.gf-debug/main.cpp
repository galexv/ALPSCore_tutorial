#include <alps/gf/gf.hpp>

// Dummy function to pretend we actually use the GF
void process(const alps::gf::itime_sigma_gf&);
double get(); 

#define SIZE 10000

int main(int argc, char** argv)
{
    namespace g=alps::gf;
    // GF over 100x100 grid
    g::itime_sigma_gf my_gf1(g::itime_mesh(10, SIZE), g::index_mesh(SIZE));

    // Mesh index types
    typedef g::itime_sigma_gf::mesh1_type::index_type index1_type;
    typedef g::itime_sigma_gf::mesh2_type::index_type index2_type;

    // Fill the GF
    for (index1_type it=index1_type(0); it<SIZE; ++it) {
        for (index2_type i=index2_type(0); i<SIZE; ++i) {
            my_gf1(it,i)=get();
        }
    }
    // Deliberate mistake
    my_gf1(index1_type(-1), index2_type(-1))=0;

    // Pretend we do something
    process(my_gf1);
}
