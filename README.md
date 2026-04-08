 *This project has been created as part of the 42 curriculum by dahmane, shtounek, mdodevsk*

# Webserv

## Description

Webserv is an HTTP/1.1 web server written in C++98, inspired by NGINX.
It is configured through a configuration file. It supports multiple servers and clients simultaneously, while beeing fully non-blocking by using `epoll`.

**Features**

 - Non-blocking I/O via single `epoll` instance
 - Multiple servers on different ports from a single config file
 - HTTP methods : `GET`, `POST`, `DELETE`
 - Static website serving
 - File upload support
 - Custom error pages
 - CGI execution (PHP, Python)
 - Cookies

## Instructions

> This program must run on Linux OS.

**Step 1 - Compilation**
```bash
make
```

**Step 2 - Run**
```bash
./webserv /tests/server.conf     # example
./webserv [configuration_file]   # custom config
```

**Step 3 - Clean**
```bash
make clean  # removes object files
make fclean # removes object files and binary
make re     # full recompilation
```

## Configuration

The configuration file is inspired by NGINX `server` block syntax.

```nginx
server
{
    listen 8080;
    server_name localhost;
    root ./www;
    index index.html;
    client_max_body_size 1048576;

    location /
    {
        root ./www;
        allow_methods GET POST DELETE;
        index index.html;
    }

    location /cgi-bin
    {
        root ./www;
        allow_methods GET POST;
        cgi .php /usr/bin/php-cgi;
        cgi .py /usr/bin/python3;
        cgi .sh /usr/bin/bash;
    }

    location /uploads
    {
        root ./www;
        allow_methods GET POST DELETE;
        autoindex on;
        upload_path ./www/uploads;
    }
}
```

## Feedback & Notes

### Score

<img width="198" height="168" alt="Screenshot 2026-04-08 at 09 13 00" src="https://github.com/user-attachments/assets/5f8b4f96-4e09-4b50-be6a-19b7996adac5" />

### Reviewer comments

> *"Un serveur super bien pensé et bien réalisé. Le groupe a une dynamique formidable et une admirable attache aux détails. J'ai été particulièrement étonné de par la propreté du code, des explications et des connaissances de chacun sur leur code mais aussi sur le code des autres membres du groupe. Tous les tests ont été réalisés et ont été réussis avec brio. La relecture du code était simple de part une présence mesurée de commentaires. La présence des commandes de tests dans le fichier txt est un détail marquant une réel intérêt pour une correction fluide. En somme, votre travail a tout les trois est incroyable et vous pouvez être fier du résultat fourni ! Il est sublime !"*

> *"Une équipe super complémentaire avec une totale maîtrise du sujet, bravo pour la réalisation du projet et l'énergie émise !"*

> *"Webserv bien fait et bien compris, merci pour toutes les explications ! Vous avez bien géré."*


# Resources
For this project, the following documentation has been used:
- for all the epoll functions: https://man7.org/linux/man-pages/man7
- for the standard functions of C++98: https://www.geeksforgeeks.org
- for the standard functions of PHP: https://www.php.net/manual, https://www.w3schools.com/php
- for CGI: https://www.php.net/manual
- for guide HTTP: https://developer.mozilla.org/fr/docs/Web/HTTP/Guide
- for NGINX: https://nginx.org/en/docs/http/ngx_http_core_module.html
- for status code: https://www.iana.org/assignments/http-status-codes/http-status-codes.xhtml

AI has also been used in this project, in order to get specific help on:
- Understanding the internal behavior of `epoll_create`, `epoll_ctl` and `epoll_wait`
- Clarifying the HTTP request/response cycle (headers, status codes, chunked encoding)
- Reviewing the CGI environment variable setup and fork/execve flow
- Debugging edge cases in the configuration file parser

All AI-generated content was reviewed, tested and validated by the team before integration.

