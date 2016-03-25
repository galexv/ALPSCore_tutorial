#include <iostream>

#include <alps/params.hpp>
#include <alps/mc/mcbase.hpp>
#include <alps/mc/stop_callback.hpp>
#include <alps/mc/mpiadapter.hpp>

class MySimulation : public alps::mcbase {
  private:
    int istep_;
    int maxcount_;
    bool verbose_;
  public:
    MySimulation(const parameters_type& params, std::size_t seed_offset=0)
        : alps::mcbase(params,seed_offset),
          istep_(0)   {
        maxcount_=params["count"];
        verbose_=params["verbose"];
    }

    void update() {
        double r=random();
        if (verbose_) {
            std::cout << "Update at step " << istep_ << ", random=" << r << std::endl;
        }
        ++istep_;
    }

    void measure() {
        if (verbose_) {
            std::cout << "Measure at step " << istep_ << std::endl;
        }
    }

    double fraction_completed() const {
        double frac=double(istep_)/maxcount_;
        return frac;
    }

    static parameters_type& define_parameters(parameters_type& params) {
        return alps::mcbase::define_parameters(params)
            .description("Dummy Monte Carlo program")
            .define<int>("count", "Number of steps to go")
            .define("verbose", "Report steps");
    }
        

    int count() {
        return istep_;
    }
    
};

typedef alps::mcmpiadapter<MySimulation> MyMpiSimulation;


int main(int argc, char** argv)
{
    // init MPI, obtain communicator
    alps::mpi::environment mpi_env(argc, argv);
    alps::mpi::communicator comm;

    // Parse the parameters on master...
    alps::params p;
    if (comm.rank()==0) { // on master process...
        alps::params p_master(argc, (const char**)argv);
        p=p_master;
    }
    broadcast(comm, p, 0); // ...broadcast parameters to all.
    
    MyMpiSimulation::define_parameters(p)
        .define<int>("time", 5, "Time limit for the computation");
        
    if (comm.rank()==0 && p.help_requested(std::cerr))
        return 1;

    std::cout << "Creating simulation on rank=" << comm.rank() << std::endl;
    MyMpiSimulation mysim(p,comm);

    std::cout << "Starting simulation on rank=" << comm.rank() << std::endl;
    mysim.run(alps::stop_callback(int(p["time"])));

    std::cout << "Simulation finished on rank=" << comm.rank() << ", ran for " << mysim.count() << " steps." << std::endl;
}
