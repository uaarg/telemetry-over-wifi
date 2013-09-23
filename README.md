telemetry-over-wifi
===================

Code supporting redundant links by allowing telemetry to be sent to the ground segment over an IP network.
Currently configured to get data from a client and push it upstream to a server.


Usage
=====
    make && cd exec

    To spawn the server:
      ./server <port> <inDataStoragePath>

    To spawn the client:
      ./client <targetHostName> <port> [<dataSourcePath> eg /dev/ttyO3]
