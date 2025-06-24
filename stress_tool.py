"""
Hardware Stress Tool (MVP)

Usage Example:
    python stress_tool.py --cpu 4 --memory 2GB --disk 5GB --duration 60

Features:
- CPU, Memory, Disk stress
- System monitoring (CPU, RAM, Disk)
- Duration control
"""
import argparse
import multiprocessing
import threading
import time
import os
import sys
import signal
from stressors import burn_cpu, burn_memory, burn_disk, burn_network, burn_gpu
from monitoring import Monitor
import tempfile

def parse_size(size_str):
    size_str = size_str.strip().upper()
    if size_str.endswith('GB'):
        return int(float(size_str[:-2]))
    elif size_str.endswith('G'):
        return int(float(size_str[:-1]))
    elif size_str.endswith('MB'):
        return int(float(size_str[:-2]) / 1024)
    elif size_str.endswith('M'):
        return int(float(size_str[:-1]) / 1024)
    else:
        return int(float(size_str))  # Assume GB

def main():
    parser = argparse.ArgumentParser(description="Hardware Stress Tool")
    parser.add_argument('--cpu', type=int, default=0, help='Number of CPU stress workers')
    parser.add_argument('--memory', type=str, default='0', help='Memory to allocate (e.g., 2GB)')
    parser.add_argument('--disk', type=str, default='0', help='Disk to write (e.g., 5GB)')
    parser.add_argument('--duration', type=int, default=60, help='Duration in seconds')
    parser.add_argument('--monitor-interval', type=int, default=2, help='Monitoring interval (sec)')
    parser.add_argument('--network-url', type=str, default=None, help='URL to download repeatedly for network stress')
    parser.add_argument('--gpu', action='store_true', help='Enable GPU stress (requires pycuda)')
    parser.add_argument('--export-csv', type=str, default=None, help='Export monitoring log to CSV file')
    parser.add_argument('--export-json', type=str, default=None, help='Export monitoring log to JSON file')
    parser.add_argument('--live-graph', action='store_true', help='Show live matplotlib graph of system usage')
    args = parser.parse_args()

    cpu_workers = args.cpu
    mem_gb = parse_size(args.memory)
    disk_gb = parse_size(args.disk)
    duration = args.duration
    monitor_interval = args.monitor_interval

    print(f"[INFO] Starting stress test: CPU={cpu_workers}, Memory={mem_gb}GB, Disk={disk_gb}GB, Duration={duration}s, Network={args.network_url is not None}, GPU={args.gpu}")

    stop_event = threading.Event()
    processes = []
    temp_disk_file = None

    monitor = Monitor(interval=monitor_interval)

    def cleanup():
        monitor.stop()
        stop_event.set()
        for p in processes:
            if p.is_alive():
                p.terminate()
        if temp_disk_file and os.path.exists(temp_disk_file):
            try:
                os.remove(temp_disk_file)
            except Exception:
                pass
        print("\n[INFO] Cleanup complete.")

    def signal_handler(sig, frame):
        print("\n[INFO] Caught signal, exiting...")
        cleanup()
        sys.exit(0)

    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)

    # Start CPU stress
    for _ in range(cpu_workers):
        p = multiprocessing.Process(target=burn_cpu)
        p.start()
        processes.append(p)

    # Start Memory stress
    if mem_gb > 0:
        p = multiprocessing.Process(target=burn_memory, args=(mem_gb,))
        p.start()
        processes.append(p)

    # Start Disk stress
    if disk_gb > 0:
        temp_disk_file = os.path.join(tempfile.gettempdir(), f"stress_disk_{os.getpid()}.bin")
        p = multiprocessing.Process(target=burn_disk, args=(temp_disk_file, disk_gb))
        p.start()
        processes.append(p)

    # Start Network stress
    if args.network_url:
        p = multiprocessing.Process(target=burn_network, args=(args.network_url, duration))
        p.start()
        processes.append(p)

    # Start GPU stress
    if args.gpu:
        p = multiprocessing.Process(target=burn_gpu, args=(duration,))
        p.start()
        processes.append(p)

    # Start monitoring
    monitor.start()

    # Show live graph if requested
    if args.live_graph:
        graph_thread = threading.Thread(target=monitor.live_graph, daemon=True)
        graph_thread.start()

    # Wait for duration
    try:
        time.sleep(duration)
    except KeyboardInterrupt:
        print("\n[INFO] Interrupted by user.")
    finally:
        cleanup()
        if args.export_csv:
            monitor.export_csv(args.export_csv)
            print(f"[INFO] Monitoring log exported to {args.export_csv}")
        if args.export_json:
            monitor.export_json(args.export_json)
            print(f"[INFO] Monitoring log exported to {args.export_json}")
        monitor.print_summary()

if __name__ == "__main__":
    main() 