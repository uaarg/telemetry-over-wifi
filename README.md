telemetry-over-wifi
===================

Code supporting redundant links by allowing telemetry to be sent to the ground segment over an IP network.


Usage
=====
    make && cd exec

    To spawn a server:
      ./server <port> <pathToDataSource>

    To spawn the client:
      ./client <targetHostName> <port>
