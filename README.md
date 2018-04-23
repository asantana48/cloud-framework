# cloud-framework

## Requirements
AWS SDK for C++: core, s3 components
Redis
Redox for Redis

libxml2

## Relevant commands
### Compiling
make initialize
make supervise
make recall

### Linking 
You may need to export the following variable either by hand or in your .bashrc:
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/usr/local/lib64

### Running 
Run initialize once (./initialize)

Run supervise, which will spawn a migration supervisor daemon (./supervise)
Check to make sure process created correctly: (ps aux | grep supervise)
You can change the policies at runtime, which are located in res/policies.xml.
When done, stop supervise (pkill supervise)

### Reading the log
journalctl -f

## AQR information
For easy compilation, put this in your .bashrc or export the variable manually:
export CPATH=/usr/include/AQR

Relevant compilation flags: 
-lAQR -lAQRDebugTraceInterface -lAQRPlatformIndependentInterface -lpthread

Make sure you are root when running the test because only root has the VMs set up.

## Server information
CentOS 7 doesn't have the correct gcc version in the default environment, however we can switch to a different toolset enable by running:

scl enable devtoolset-7 bash

It will then compile.



