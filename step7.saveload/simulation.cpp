#include <alps/params/convenience_params.hpp>
#include "simulation.hpp"

#include <cmath>

bool MySimulation::is_inside_area(double x, double y) {
    // Let it be just a 1x1 square centered at (0,0)
    return ((x<=0.5 && x>=-0.5) &&
            (y<=0.5 && y>=-0.5));
}

double MySimulation::objective_function(double x, double y) {
    // Let it be 1 if we hit a circle of radius 0.5, centered at (0,0)
    return (x*x+y*y <= 0.25);
}
    

MySimulation::MySimulation(const parameters_type& params, std::size_t seed_offset)
    : alps::mcbase(params,seed_offset), istep_(0), x_(0.0), y_(0.0)
{
    maxcount_=params["count"];
    verbose_=params["verbose"];
    burnin_=params["burn-in"];
    stepsize_=params["step"];

    // Initialize slots for the measurements (named observables)
    measurements << my_accumulator_type("objective");
}


void MySimulation::update() {
    double xstep=2*(random()-0.5)*stepsize_;
    double ystep=2*(random()-0.5)*stepsize_;
    double x1=x_+xstep;
    double y1=y_+ystep;
    // Simple (not very efficient) way to make sure we are inside the area:
    if (!is_inside_area(x_,y_)) { // if we are now outside...
        x_=x1;
        y_=y1;
        return; // ...make a step and return without updating step count
    }
    // Simple-case Metropolis: always accept if we are within the area, always reject otherwise
    if (is_inside_area(x1,y1)) {
        x_=x1;
        y_=y1;
    }
    ++istep_;
}
        
void MySimulation::measure() {
    bool is_past_burnin=(istep_>=burnin_);
    if (verbose_) {
        std::cout << istep_ << " " << is_past_burnin << " " << x_ << " " << y_ << "\n";
    }
    if (!is_past_burnin) return;

    measurements["objective"] << objective_function(x_,y_);
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
        .define<double>("step", "Maximum size of a trial step");
}

// Saves the state to the hdf5 file
void MySimulation::save(alps::hdf5::archive & ar) const {
    // Most of the save logic is already implemented in the base class
    alps::mcbase::save(ar);
    
    // We just need to add our own internal state
    ar["checkpoint/istep_"] << istep_;
    ar["checkpoint/x"] << x_;
    ar["checkpoint/y"] << y_;
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

    // Restore the rest of the state from the hdf5 file
    ar["checkpoint/istep_"] >> istep_;
    ar["checkpoint/x"] >> x_;
    ar["checkpoint/y"] >> y_;
}
