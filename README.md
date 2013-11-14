telemetry-over-wifi
===================

Code supporting redundant links by allowing telemetry to be sent to the 

ground segment over an IP network.

Currently configured to get data from a client and push it upstream to a server

 \*\* [ by default via UDP, easily configurable for TCP ].


Usage
=====
    make && cd exec

    To spawn the server:
      ./server <port> <inDataStoragePath>

    To spawn the client:
      ./client <targetHostName> <port> <serialdevice> <baudrate> [<dataSourcePath> eg /dev/ttyO3]

      Note: <baudrate> is an unsigned integer value eg 56700


File organization / Hierarchy
=============================

+ All header files [\*.h] are in directory include/

+ All source files [\*.c] are in directory src/

+ All executables created after running 'make' will be in directory exec/

+ Sample logs are in directory logs/
