===================
Short ALPS tutorial
===================

Let's assume that ``ALPSCore`` is installed and ``ALPSCore_DIR`` is
pointing to the installation directory.

1. How ``CMakeLists.txt`` should look to use ``ALPSCore``.
   Catch: compilers! ``CXX=$(which needed_cpp) CC=$(which needed_cc)``
   Catch: if compilers change: ``rm -rf CMake*``
   Catch: if cmake files change, ``cmake .`` in the build directory
   In-source builds are messy.
   Catch: after accidental in-source build, remove the generated files.
   
2. params
   See Doxygen documentation for up-to-date info
   Can use input file, override with command line.
   Usage: 
   ``./demo --help``
   ``./demo --name=value``
   ``./demo --flag``
   Unknown parameters silently ignored.
   Accessing unknown parameters throws an exception.

3. simple simulation class (doing nothing)
   Good idea to indicate a debug or a release build: 
   ``cmake -DCMAKE_BUILD_TYPE=Debug ..`` or
   ``cmake -DCMAKE_BUILD_TYPE=Release ..``.

4. use of mpi 
   * Not many changes compared to the sequential version.
   * Note that the completion is checked only at certain intervals (1
     sec minimum)


5. use of scalar accumulators to do something
   Split program into several files.
   Integral of objective function over an area.
   Trivial Metropolis step to stay inside the area. 
   Area: square
   Objective function: 1 if inside an inscribed circle, 0 otherwise.
   Expected result: pi (if multiplied by 4).

   Notion of accumulators.
   Notion of the results.
   Note that we multiply the result!

   Accumulators:
   * Mean only
   * No binning
   * Log binning 
   * Full binning
   Demonstrate performance (as number of step in 5 seconds) and
   availability of methods: tau, error, mean.
   Play with step length and see how values change.
   Play with the number of processes.

   1. First, try ``MeanAccumulator`` with different steps
   2. Second, try ``NoBinning`` and report the error
   3. Then try ``FullBinning`` and report tau
   4. Revert to ``MeanAccumulator`` and demonstrate the error.

6. NEXT TIME: use of vector accumulators to do something more complex,
   and accumulator operations

7. use of checkpointing

8. ???

9. Proper installation
   * ``install`` target
   * Automatic setting of RPATH
