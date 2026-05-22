# HSDES-LAB05: Parallel programming on PULP using the OpenMP programming model

The goals of this lab session are:
1) Parallelize a simple "hello world" program, with a focus on data sharing clauses
2) Parallelize a matrix multiplication kernel using the for directive (alternatives: static schedule with default chunk, static schedule with unary chunck, dynamic schedule)
3) Parallelize an algoritm with data dependencies (alternatives: critical sections, reduction clause)
4) Parallelize a program using task parallelism (alternatives: check the thread id, sections directive)


## Getting Started
Install the OpenMP runtime in the PULP SDK
~~~~~shell
source /pulp/sourceme.sh
cd /pulp/pulp-sdk
git stash
git pull origin main
make build
~~~~~

In case of errors related to certificates, use this command to set the system clock:
~~~~~shell
sudo ntpdate -u time.nist.gov
~~~~~

Open a shell and clone the repository with the sample code.
~~~~~shell
git clone https://github.com/EEESlab/HSDES-LAB05-PULP_Parallel_OpenMP.git
cd HSDES-LAB05-PULP_Parallel_OpenMP
cd hello-omp
make clean all run CORES=1
~~~~~


### References
* [PULP builtins](https://greenwaves-technologies.com/manuals/BUILD/PULP-OS/html/group__groupBuiltinsAPI.html)
* [PMISIS APIs](https://greenwaves-technologies.com/manuals/BUILD/PMSIS_API/html/index.html)
* [PMISIS BSP](https://greenwaves-technologies.com/manuals/BUILD/PMSIS_BSP/html/index.html)
