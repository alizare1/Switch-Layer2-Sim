# Layer 2 Switching Simulator

This program simulates layer 2 switching with Unix pipes and forking new processes. Using an interface the user can add new switched or systems to the network (which are created using `fork` and `exec` syscalls) and connect them with a set of commands. Then the user can try to send or receive files between systems. Switches also have a lookup table that gets filled when they receive packets from different systems. If files are larger than the frame size, they get broken to multiple packets and then joined at the destination.

## STP Algorithm

STP algorithm is implemented to prevent loops in the network. By running the algorithm all switches run a distributed algorithm and broadcast a special message to find a root node and block extra ports to form a spanning tree.

## Commands

```
add_switch <id> <portCount>
add_system <id>
connect <system_id> <switch_id> <port_number>
send <src system> <dst system> <file name>
recv <src system> <dst system> <file name>
connect_switch <s1_id> <s1_port> <s2_id> <s2_port>
run_stp
```

## How to run

```
make clean
make
./sim.out
```