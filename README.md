# Hardware Stress Tool (MVP)

A lightweight Python tool to stress-test your computer's CPU, memory, disk, network, and GPU, with real-time system monitoring.

## Features
- **CPU Stress:** Burn CPU cores with configurable worker count
- **Memory Stress:** Allocate and hold large blocks of RAM
- **Disk Stress:** Write large files to disk
- **Network Stress:** Download repeatedly from a specified URL
- **GPU Stress:** Run CUDA kernel (requires pycuda, NVIDIA GPU)
- **System Monitoring:** Live CPU, RAM, and disk usage stats
- **Duration Control:** Automatically stops after specified time

## Requirements
- Python 3.7+
- [psutil](https://pypi.org/project/psutil/)
- [requests](https://pypi.org/project/requests/) *(for network stress)*
- [pycuda](https://pypi.org/project/pycuda/) *(for GPU stress, optional)*
- [numpy](https://pypi.org/project/numpy/) *(for GPU stress, optional)*

Install dependencies:
```bash
pip install -r requirements.txt
```

## Usage
```bash
python stress_tool.py --cpu 4 --memory 2GB --disk 5GB --duration 60 --network-url https://example.com --gpu
```

**Arguments:**
- `--cpu N`         Number of CPU stress workers (processes)
- `--memory SIZE`   Memory to allocate (e.g., 2GB, 512MB)
- `--disk SIZE`     Disk to write (e.g., 5GB, 500MB)
- `--duration SEC`  Duration in seconds (default: 60)
- `--monitor-interval SEC`  Monitoring interval in seconds (default: 2)
- `--network-url URL`  URL to download repeatedly for network stress
- `--gpu`           Enable GPU stress (requires pycuda, NVIDIA GPU)

## Example
Stress 2 CPU cores, allocate 1GB RAM, write 2GB to disk, download from example.com, and stress GPU for 30 seconds:
```bash
python stress_tool.py --cpu 2 --memory 1GB --disk 2GB --duration 30 --network-url https://example.com --gpu
```

## Notes
- Disk stress writes a temporary file to your system temp directory and deletes it after the test.
- Network stress requires the `requests` library.
- GPU stress requires `pycuda` and `numpy`, and a compatible NVIDIA GPU.
- Use with caution! This tool will heavily load your system.
- Stop anytime with `Ctrl+C`.

## License
MIT 