# 🚀 Quick Start Guide

Get Proton Web Server running in under 2 minutes!

## For Linux Users

```bash
# 1. Install build tools (if needed)
sudo apt-get install build-essential

# 2. Build the project
make

# 3. Run the server
./build/proton -c conf/proton.conf.example

# 4. Test it
curl http://localhost:8080/
```

**That's it!** Open http://localhost:8080/ in your browser to see the welcome page.

## For Windows Users (WSL2 Required)

```powershell
# 1. Open WSL terminal
wsl

# 2. Navigate to project
cd /mnt/c/path/to/My-web-server---Proton-web

# 3. Install build tools
sudo apt-get update && sudo apt-get install build-essential

# 4. Build
make

# 5. Run
./build/proton -c conf/proton.conf.example
```

## What Just Happened?

- ✅ Proton compiled from C source code
- ✅ Master process spawned worker processes
- ✅ Workers created epoll-based event loops
- ✅ HTTP server listening on port 8080
- ✅ Static file module loaded

## Test Commands

```bash
# Get the main page
curl http://localhost:8080/

# Check response headers
curl -I http://localhost:8080/

# Load test (requires apache2-utils)
ab -n 1000 -c 10 http://localhost:8080/
```

## Create Your Own Content

```bash
# Edit the welcome page
nano public/index.html

# Add more files
echo "Hello from Proton!" > public/test.txt

# Test your new file
curl http://localhost:8080/test.txt
```

## Stop the Server

Press `Ctrl+C` in the terminal where Proton is running.

## Next Steps

- 📖 Read [BUILD.md](BUILD.md) for detailed build instructions
- 📖 Read [README.md](README.md) for architecture documentation
- 🔧 Edit [conf/proton.conf.example](conf/proton.conf.example) to customize
- 💻 Explore the source code in [src/](src/)

## Troubleshooting

**Port 8080 already in use?**
```bash
# Edit conf/proton.conf.example and change the port
nano conf/proton.conf.example
# Change "listen 8080;" to "listen 8888;" or any free port
```

**Build errors?**
```bash
# Clean and rebuild
make clean
make debug
```

**Can't connect?**
```bash
# Check if server is running
ps aux | grep proton

# Check if port is listening
netstat -tuln | grep 8080
# or
ss -tuln | grep 8080
```

---

**Need help?** Check [BUILD.md](BUILD.md) for detailed instructions or open an issue on GitHub.
