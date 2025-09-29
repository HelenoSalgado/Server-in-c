# Simple C Web Server

A lightweight, multi-threaded HTTP/1.1 server written in C for serving static files.

## Dependencies

- `gcc` (GNU Compiler Collection)
- `make`
- A POSIX-compliant system with `pthreads` support (e.g., Linux, macOS).

## How to Run

1.  **Compile the server:**
    ```bash
    make
    ```

2.  **Run the server:**
    ```bash
    ./bin/server [options]
    ```

### Command-line Options

| Option      | Description                                           | Default      |
|-------------|-------------------------------------------------------|--------------|
| `-p <port>` | Specify the port to listen on.                        | `3000`       |
| `-d <dir>`  | Specify the web root directory.                       | `./web`      |
| `-b`        | Run the server as a daemon (in the background).       | (foreground) |
| `-h`        | Display the help message.                             |              |

**Example:**
```bash
# Run in the background on port 8080, serving files from /var/www
./bin/server -b -p 8080 -d /var/www
```

## Features

- **Multi-threaded:** Handles multiple connections simultaneously.
- **Configurable:** Set port and root directory via command line.
- **Static File Serving:** Serves static files from the specified root directory.
- **HTTP/1.1 Compliance:**
    - Supports `GET` and `HEAD` methods.
    - Returns `501 Not Implemented` for unsupported methods.
- **Logging:** Logs requests to standard output in the format: `[timestamp] - [client_ip] - "[method] [path]" [status_code]`.
- **Daemonization:** Can run as a background process.

---
For more detailed documentation, see the `docs` directory.
