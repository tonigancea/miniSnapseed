# miniSnapseed

With this program you can apply commands on `pgm` and `pnm` images.

## Supported commands:
resize x
rotate_left
rotate_right
flip_vert
flip_hor

## How to run:

**pThreads** version:  
`./homework input_file output_file num_threads_used [commands..]`  

**openMp** version:  
`export OMP_NUM_THREADS=num_threads_used` #setting the variable  
`./homework input_file output_file [commands..]`  

**mpi** version:  
`mpirun -np num_machines_used homework input_file output_file [commands..]`

**hybrid** version (openMP + mpi):  
`export OMP_NUM_THREADS=num_threads_used` #setting the variable  
`mpirun -np num_machines_used homework input_file output_file [commands..]`

## Enjoy coding!
