# RemoteOps - Remote System Monitoring and Management Tool

## Student Details
Registration Number: IT24102882
Module: IE3090 - Network Programming

## Project Description
RemoteOps is a client-server network application developed in C using BSD sockets.

The Agent acts as the server and listens for Controller connections. The Controller acts as the client and sends authenticated commands to the Agent.

TCP is used for command communication and file transfer, while UDP is used for periodic monitoring.

## Personalised Values
Registration Number: IT24102882
Agent TCP Port: 9410
Agent Source File: agent_882.c
Controller Source File: controller_882.c
Makefile: Makefile_882
Session ID (SID): 2882
Authentication Token: OPS-2882
Log File: remoteops_IT24102882.log
Storage Directory: ./agentfiles/IT24102882/

## Implemented Features
- TCP Agent and Controller communication
- Authentication using OPS-2882
- SYSINFO command
- LISTPROC command
- Restricted EXEC commands
- PUT file upload
- GET file download
- UDP monitoring START and STOP
- Concurrent Controller handling using pthreads
- Thread-safe activity logging
- Graceful QUIT disconnect

## Supported EXEC Commands
- DATE
- UPTIME
- DISKFREE
- HOSTNAME
- WHOAMI

## Build Instructions

Compile both programs using:

make -f Makefile_882

Clean compiled files using:

make -f Makefile_882 clean

## Running the Application

Start the Agent:

./agent_882

Open another terminal and start the Controller:

./controller_882

The Agent listens on TCP port 9410.

## Authentication

The first Controller command must be:

AUTH OPS-2882

A successful authentication response contains:

OK AUTHENTICATED SID:2882

## File Storage

Files uploaded
