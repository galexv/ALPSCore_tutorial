#include <alps/mc/mcbase.hpp>
#include <alps/mc/mpiadapter.hpp>

#include <alps/accumulators.hpp>

/// A point in 2D
struct point_type {
    double x, y;
    point_type() : x(0), y(0) {}
    void set(double xx, double yy) { x=xx; y=yy; }

    void save(alps::hdf5::archive& ar) const {
        ar["x"]<<x; ar["y"]<<y;
    }

    void load(alps::hdf5::archive& ar) {
        ar["x"]>>x; ar["y"]>>y;
    }
};

class MySimulation : public alps::mcbase {
  private:
    long burnin_;
    long maxcount_;
    bool verbose_;
    double stepsize_;
    double gamma_;
    
    long istep_;
    
    point_type chain_sqr_; // chain sampling the square
    point_type chain_cir_; // chain sampling the circle
    
  public:
    // These we need for the simulation.
    static int inside_square(double x, double y);
    static int inside_circle(double x, double y);
    static double objective_function(double x, double y, double gamma);

    // Accumulator type to collect observables.
    typedef alps::accumulators::FullBinningAccumulator<double> my_accumulator_type;
    
    MySimulation(const parameters_type& params, std::size_t seed_offset=0);

    void update();

    void measure();

    double fraction_completed() const;
    
    static parameters_type& define_parameters(parameters_type& params);

    void save(alps::hdf5::archive&) const;
    void load(alps::hdf5::archive&);
};

typedef alps::mcmpiadapter<MySimulation> MyMpiSimulation;
