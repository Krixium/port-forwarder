# Port Forwarder

Forwards ports.

## Configuration

To configure the ports that are forwarded, edit the entries in the fowarder.conf file.
There should be one entry per line in the following format: `ipIncoming:portIncoming -> ipOutgoing:portOutgoing`.

`ipIncoming` - This is the IP that the port forwarded will listen for for incoming connections. Connections to `portIncoming` from any other host will be denied.

`portIncoming` - This is the port that the port forwarded will listen on for `ipIncoming`. There **cannot** be two lines in the configuration file with the same `ipIncoming`.

`ipOutgoing` - This is the destination address that all data from host `ipIncoming` on port `portIncoming` will be forwarded to.

`portOutgoing` - This is the destination port that all data from host `ipIncoming` on port `portIncoming` will be forwarded to.

## Usage

    ./forwarder.out