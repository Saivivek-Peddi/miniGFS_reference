## Homework Assignment 3

We are using threads to simulate concurrent and serial writes from multiple clients in minigfs_client.cpp

### Steps:

- `cd` in to the directory

### Simulate Failure Case:

- Open `Core.h` and **uncomment** the line `#define simulateFailure` on line 19 
to make the `minigfs_secondary_B` replica to go to sleep just before it commits data 
which results in inconsistency when reading data
- Run `make` to compile and produce binaries
- Run `./minigfs_master` to run master
- Similarly, run `./minigfs_primary`, `./minigfs_secondary_A` and `./minigfs_secondary_B` to run all three servers
- Run `./minigfs_client` to write data to all three servers and observe the log printed


### Simulate Serial:Defined Case:

- Open `Core.h` and **comment** the line `#define simulateFailure` on line 19. We don't want any failures now
- Run `make` to compile and produce binaries
- Restart `./minigfs_secondary_B` to run the updated binary without failure case
- Run `./minigfs_client` to write data to all three servers and observe the log printed
- As the main thread sleeps for 0.1 second before starting client2 thread, we can simulate serial writes, which is defined
- ![Output will look like this](/assets/serial-defined.png)


### Simulate Concurrent:Consistent Case:

- Open `minigfs_client.cpp` and **uncomment** the line `#define simulateConcurrent` on line 13.
- Run `make` to compile and produce binaries
- Run `./minigfs_client` to write data to all three servers and observe the log printed
- Here, both client1 thread and client2 thread start concurrently. So, we can simulate concurrent writes, which is consistent but undefined
- Since client1 and client2 are threads running concurrently, there is a chance which can lead to defined case as well. 
Please run `./minigfs_client` multiple times to observe consistent but undefined case
- ![Output will look like this](/assets/concurrent-consistent.png)


### All cases:

- Logs in all the servers are disabled by default for hw3. Open `Core.h` and **comment** `#define myRelease` on line 17 and
run `make` to compile and restart all the servers, if running, to enable logs again