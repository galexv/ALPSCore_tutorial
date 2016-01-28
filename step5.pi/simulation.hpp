#include <alps/mc/mcbase.hpp>
#include <alps/mc/mpiadapter.hpp>

#include <alps/accumulators.hpp>

class MySimulation : public alps::mcbase {
  private:
    long burnin_;
    long maxcount_;
    bool verbose_;
    double stepsize_;
    
    long istep_;
    double x_,y_;
    
  public:
    static bool is_inside_area(double x, double y);
    static double objective_function(double x, double y);

    // typedef alps::accumulators::MeanAccumulator<double> my_accumulator_type;
    typedef alps::accumulators::FullBinningAccumulator<double> my_accumulator_type;
    
    MySimulation(const parameters_type& params, std::size_t seed_offset=0)
        : alps::mcbase(params,seed_offset),
          istep_(0), x_(0.0), y_(0.0)   {
        maxcount_=params["count"];
        verbose_=params["verbose"];
        burnin_=params["burn-in"];
        stepsize_=params["step"];

        // Initialize slots for the measurements (named observables)
        measurements << my_accumulator_type("objective");
    }

    void update();

    void measure();

    double fraction_completed() const;
    
    static parameters_type& define_parameters(parameters_type& params);        
};

typedef alps::mcmpiadapter<MySimulation> MyMpiSimulation;
