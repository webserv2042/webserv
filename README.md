*This project has been created as part of the 42 curriculum by dahmane, shtounek, mdodevsk*

# Description
An HTTP web server written in C++98. This server is created based on a configuration file (located at ./server.conf).
It can handle multiple servers and clients while remaining non-blocking.
-html
-cgi
-maybe cookies

# Instructions
This program must run on Linux OS.
To launch the program, you must execute it with the configuration file as argument (./server.conf).

# Resources
For this project, the following documentation has been used:
- for all the epoll functions: https://man7.org/linux/man-pages/man7
- for the standard functions of C++98: https://www.geeksforgeeks.org

AI has also been used in this project, in order to get specific help and information on:
- The basic structure of a web server
- How HTTP requests/responses interact
- The way functions works internally (such as epoll_create, epoll_wait, etc...)
- An efficient and readable file structure

