# Design Diary - RemoteOps

Registration Number: IT24102882
Module: IE3090 - Network Programming

## Development Process

I started the RemoteOps project by setting up the CentOS development environment and creating the personalised Agent and Controller source files. I used the personalised TCP port 9410 and first tested a basic TCP connection between the Agent and Controller.

After establishing the connection, I implemented authentication using the token OPS-2882. I then added the SYSINFO command and tested whether the Controller could request system information from the Agent successfully.

The next stage was implementing LISTPROC to display running processes. After that, I implemented the restricted EXEC feature. Instead of allowing arbitrary system commands, only selected commands such as DATE, UPTIME, DISKFREE, HOSTNAME and WHOAMI were allowed.

I then implemented file transfer. PUT was developed and tested first to upload a file from the Controller to the personalised Agent storage directory. GET was then implemented to download the file back to the Controller. I tested the transferred file contents to confirm that the file transfer was working correctly.

UDP monitoring was implemented separately from the TCP command connection. Initially, I tested starting and stopping UDP monitoring. I later improved it so that monitoring messages were sent periodically while monitoring was active.

To support multiple users, I added pthread-based concurrency so that the Agent could handle multiple Controller connections at the same time. I tested this using five simultaneous Controller connections.

I also added thread-safe activity logging using a mutex to protect the shared log file. Authentication and command activity were recorded in remoteops_IT24102882.log with timestamps. Finally, I implemented the QUIT command so that a Controller could disconnect gracefully.

During development, I tested each feature incrementally instead of implementing everything at once. Some areas, especially file transfer, UDP monitoring and concurrent logging, required additional testing. Git commits were used to record the major implementation stages. A Makefile was added at the end to make compilation of both the Agent and Controller easier and consistent.
