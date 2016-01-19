#include <iostream>

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
    
    MyMpiSimulation::define_parameters(p)
        .define<int>("time", 5, "Time limit for the computation");
        
    if (comm.rank()==0 && p.help_requested(std::cerr))
        return 1;

    std::cerr << "Creating simulation on rank=" << comm.rank() << std::endl;
    MyMpiSimulation mysim(p,comm);

    std::cerr << "Starting simulation on rank=" << comm.rank() << std::endl;
    mysim.run(alps::stop_callback(int(p["time"])));

    std::cerr << "Simulation finished on rank=" << comm.rank()
              << "\nCollecting results..." << std::endl;
    
    alps::accumulators::result_set results=mysim.collect_results();
    if (comm.rank()==0) {
        const alps::accumulators::result_wrapper& res_raw=results["objective"];
        std::cerr << "Objective: " << res_raw << std::endl;
        
        const alps::accumulators::result_wrapper& res=res_raw*4.; // should get PI with the "default" objective & area

        std::cerr << "\nCount: " << res_raw.count() << std::endl;
        std::cerr << "Mean: " << res.mean<double>() << std::endl;
        std::cerr << "Error: " << res.error<double>() << std::endl;
        std::cerr << "Range: " << res.mean<double>()-res.error<double>()
                  << " ... " << res.mean<double>()+res.error<double>()
                  << std::endl;
        std::cerr << "Autocorrelation length: " << res.autocorrelation<double>() << std::endl;
    }
}
