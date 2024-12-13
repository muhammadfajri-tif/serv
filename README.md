# SERV

[![Docker Image CI](https://github.com/muhammadfajri-tif/serv/actions/workflows/docker-ci.yml/badge.svg)](https://github.com/muhammadfajri-tif/serv/actions/workflows/docker-ci.yml)

<!--toc:start-->

Table of contents:

- [Overview](#overview)
  - [Features](#features)
  - [Project Structures](#project-structures)
- [Quick Start](#quick-start)
- [Configuration](#configuration)
  - [Web Server](#web-server)
  - [Static Content](#static-content)
  - [Logging](#logging)

<!--toc:end-->

---

## Overview

Simple HTTP Server written in C. Utilize `fork()` and semaphore to handle multi-client
connection.

### Features

- Simple HTTP request including GET, POST, and HEAD.
- Routing management.
- Multi-client connection handling using `fork()` and semaphore for handle concurrency.
- Logging client request.
- Run on any x86-64 arch platform including MacOS, Windows, Linux/Unix, BSD, etc.
  using container like Docker.

### Project Structures

- `main.c`: Entry point for the server.
- `config.h`: Configuration file. See [Configuration](#configuration) section
  for more details.
- `net.h` and `net.c`: Initialize socket and TCP connections.
- `http.h` and `http.c`: Manages HTTP request and response for client.
- `utils.h` and `utils.c`: Utility functions for parsing and validating file/request.
- `router.h` and `router.c`: Define and manages available route and it's method.

---

## Quick Start

1. Clone [this repository](https://github.com/muhammadfajri-tif/serv)

   ```sh
   git clone https://github.com/muhammadfajri-tif/serv.git

   cd serv
   ```

2. (optional) [Configure Web Server](#configuration) in [`config.h`](./config.h)

3. Build/Compile the source code

   ```sh
   make build
   ```

4. Run Server

   ```sh
   chmod +x ./serv
   ./serv
   ```

   The server is using port `8080` by default or specify it using argument.

   ```sh
   # ./serv <your-port>
   ./serv 6969
   ```

5. Open [http://localhost:8080/](http://localhost:8080/) or [http://localhost:`<your-port>`/](http://localhost:6969).

### Run Serv in Docker

Serv available as image in [Docker Hub](https://hub.docker.com/r/muhammadfajritif/serv).

To run Serv, simply pull the image

```sh
docker pull muhammadfajritif/serv:latest
```

Then launch the container

```sh
docker run -p 8080:8080 muhammadfajritif/serv
```

Open [http://localhost:8080/](http://localhost:8080/)

---

## Configuration

Configuration for web server all available in [`config.h`](./config.h)
so you can edit it.

### Web Server

| Name             | Default value | Description                                                                                                                                                                 |
| ---------------- | ------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `PORT`           | `"8080"`      | Port number that used for web server.                                                                                                                                       |
| `BUFFER_SIZE`    | `4096`        | Maximum size of the buffer for store data/payload from each client.                                                                                                         |
| `BACKLOG`        | `10`          | Max length for queue of pending connection.                                                                                                                                 |
| `MAX_CONNECTION` | `1000`        | Max connection that web server can handle. Each connection create new child process using `fork()`. Set this configuration wisely according to the CPU specifications used. |

### Static Content

| Name             | Default value   | Description                                                                                                                |
| ---------------- | --------------- | -------------------------------------------------------------------------------------------------------------------------- |
| `ROOT_DIR`       | `"./www"`       | Root directory that web server read for serving static files/content.                                                      |
| `ROOT_FILE`      | `"/index.html"` | Default file that web server read for entry point.                                                                         |
| `NOT_FOUND_FILE` | `"/404.html"`   | Fallback file when web server uses when it cannot find file the requested file. Used when the response status code is 404. |

### Logging

| Name         | Default value | Description                                                |
| ------------ | ------------- | ---------------------------------------------------------- |
| `CHUNK_SIZE` | `1024`        | Maximum size of the buffer for store log for each request. |
| `LOG_FILE`   | `./serv.log`  | Default file used to store web server's logs.              |
