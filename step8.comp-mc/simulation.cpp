#include <alps/params/convenience_params.hpp>
#include "simulation.hpp"

#include <cmath>

int MySimulation::inside_square(double x, double y) {
    // Let it be just a 1x1 square centered at (0,0)
    return int((x<=0.5 && x>=-0.5) && (y<=0.5 && y>=-0.5));
}

int MySimulation::inside_circle(double x, double y) {
    // Let it be 1 if we hit a circle of radius 0.5, centered at (0,0)
    return int(x*x+y*y <= 0.25);
}
    
double MySimulation::objective_function(double x, double y, double gamma) {
    // (x^2+y^2)^(gamma/2)
    return std::pow(x*x+y*y, gamma/2);
}

MySimulation::MySimulation(const parameters_type& params, std::size_t seed_offset)
    : alps::mcbase(params,seed_offset), istep_(0)
{
    maxcount_=params["count"];
    verbose_=params["verbose"];
    burnin_=params["burn-in"];
    stepsize_=params["step"];
    gamma_=params["gamma"];

    // Initialize slots for the measurements (named observables)
    measurements << my_vector_accumulator_type("objective")
                 << my_scalar_accumulator_type("area");
}


void MySimulation::update() {
    // Step for the square-sampling chain
    {
        double xstep=2*(random()-0.5)*stepsize_;
        double ystep=2*(random()-0.5)*stepsize_;
        double x1=chain_sqr_.x+xstep;
        double y1=chain_sqr_.y+ystep;
        // Simple-case Metropolis: always accept if we are within the square
        if (inside_square(x1,y1)==1) {
            chain_sqr_.set(x1,y1);
        }
    }
    // Step for the circle-sampling chain
    {
        double xstep=2*(random()-0.5)*stepsize_;
        double ystep=2*(random()-0.5)*stepsize_;
        double x1=chain_cir_.x+xstep;
        double y1=chain_cir_.y+ystep;
        // Simple-case Metropolis: always accept if we are within the circle
        if (inside_circle(x1,y1)==1) {
            chain_cir_.set(x1,y1);
        }
    }
    ++istep_;
}
        
void MySimulation::measure() {
    bool is_past_burnin=(istep_>=burnin_);
    if (verbose_) {
        std::cout << istep_ << " " << is_past_burnin
                  << " " << chain_sqr_.x << " " << chain_sqr_.y
                  << " " << chain_cir_.x << " " << chain_cir_.y
                  << "\n";
    }
    if (!is_past_burnin) return;

    measurements["area"] << double(inside_circle(chain_sqr_.x, chain_sqr_.y));

    // compute objective function for each gamma:
    std::vector<double> f(gamma_.size()); // place for objective function values
    for (int i=0; i<f.size(); ++i) {
         f[i]=objective_function(chain_cir_.x,chain_cir_.y,gamma_[i]);
    }
    measurements["objective"] << f;
}

double MySimulation::fraction_completed() const {
    if (maxcount_==0) return 0;
    if (istep_<burnin_) return 0;
    return double(istep_-burnin_)/maxcount_;
}

MySimulation::parameters_type& MySimulation::define_parameters(MySimulation::parameters_type& params) {
    // If the parameters are restored, they are already defined
    if (params.is_restored()) {
        return params;
    }
    alps::mcbase::define_parameters(params);
    return alps::define_convenience_parameters(params)
        .description("Demo Monte Carlo program")
        .define<long>("count", 0, "Number of steps to go (0: till timeout)")
        .define("verbose", "Report steps")
        .define<long>("burn-in", 10000, "Number of steps before taking measurements")
        .define<double>("step", "Maximum size of a trial step")
      .define< std::vector<double> >("gamma", "Gamma parameters");
}

// Saves the state to the hdf5 file
void MySimulation::save(alps::hdf5::archive & ar) const {
    // Most of the save logic is already implemented in the base class
    alps::mcbase::save(ar);
    
    // We just need to add our own internal state
    ar["checkpoint/istep_"] << istep_;
    ar["checkpoint/chain_sqr"] << chain_sqr_;
    ar["checkpoint/chain_cir"] << chain_cir_;
    // The rest of the internal state is saved as part of the parameters
}

// Loads the state from the hdf5 file
void MySimulation::load(alps::hdf5::archive & ar) {
    // Most of the load logic is already implemented in the base class
    alps::mcbase::load(ar);

    // Restore the internal state that came from parameters
    maxcount_=parameters["count"];
    verbose_=parameters["verbose"];
    burnin_=parameters["burn-in"];
    stepsize_=parameters["step"];
    gamma_=parameters["gamma"];

    // Restore the rest of the state from the hdf5 file
    ar["checkpoint/istep_"] >> istep_;
    ar["checkpoint/chain_sqr"] >> chain_sqr_;
    ar["checkpoint/chain_cir"] >> chain_cir_;
}
