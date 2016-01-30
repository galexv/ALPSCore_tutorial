#include <iostream>
#include <boost/lexical_cast.hpp>

#include <alps/params.hpp>
#include <alps/mc/stop_callback.hpp>

#include "simulation.hpp"



int main(int argc, char** argv)
{
    // init MPI, obtain communicator
    boost::mpi::environment mpi_env(argc, argv);
    boost::mpi::communicator comm;

    // Parse the parameters on master...
    alps::params p;
    if (comm.rank()==0) { // on master process...
        alps::params p_master(argc, (const char**)argv);
        p=p_master;
    }
    broadcast(comm, p, 0); // ...broadcast parameters to all.
    
    MyMpiSimulation::define_parameters(p);
        
    if (comm.rank()==0 && p.help_requested(std::cerr))
        return 1;

    std::cerr << "Creating simulation on rank=" << comm.rank() << std::endl;
    MyMpiSimulation mysim(p,comm);

    // If needed, restore the last checkpoint
    std::string checkpoint_file = p["checkpoint"].as<std::string>()+boost::lexical_cast<std::string>(comm.rank());

    if (p.is_restored()) {
        std::cout << "Restoring checkpoint from " << checkpoint_file
                  << " on rank=" << comm.rank()
                  << std::endl;
        mysim.alps::mcbase::load(checkpoint_file);
    }

    std::cerr << "Starting simulation on rank=" << comm.rank() << std::endl;
    mysim.run(alps::stop_callback(std::size_t(p["timelimit"])));

    std::cerr << "Simulation finished on rank=" << comm.rank()
              << "\nCheckpointing simulation..." << std::endl;
    mysim.alps::mcbase::save(checkpoint_file);
    std::cerr << "Rank=" << comm.rank()
              << ": collecting results..." << std::endl;
    
    alps::accumulators::result_set results=mysim.collect_results();
    if (comm.rank()==0) {
        const alps::accumulators::result_wrapper& objective_raw=results["objective"];
        const alps::accumulators::result_wrapper& area_raw=results["area"];
        std::cerr << "Objective: " << objective_raw << std::endl;
        std::cerr << "Area: " << area_raw << std::endl;
        
        const alps::accumulators::result_wrapper& res=objective_raw*area_raw; // should get PI/4 for gamma==0

        std::cerr << "\nCount: " << objective_raw.count() << std::endl;
        std::cerr << "Mean: " << res.mean<double>() << std::endl;
        std::cerr << "Error: " << res.error<double>() << std::endl;
        std::cerr << "Range: " << res.mean<double>()-res.error<double>()
                  << " ... " << res.mean<double>()+res.error<double>()
                  << std::endl;
        std::cerr << "Autocorrelation length: " << res.autocorrelation<double>() << std::endl;
    
        // Saving to the output file
        std::string output_file = p["outputfile"];
        alps::hdf5::archive ar(output_file, "w");
        ar["/parameters"] << p;
        ar["/simulation/results"] << res;
    }
}
