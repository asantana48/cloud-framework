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

### Running 
Run initialize once (./initialize)

Run supervise, which will spawn a migration supervisor daemon (./supervise)
Check to make sure process created correctly: (ps aux | grep supervise)
You can change the policies at runtime, which are located in res/policies.xml.
When done, stop supervise (pkill supervise)

Run recall to restore migrated files (./recall)
NOTE that you should probably stop the supervise daemon before doing this.

### Reading the log
journalctl /home/andres/Projects/cloud-framework/supervise 



