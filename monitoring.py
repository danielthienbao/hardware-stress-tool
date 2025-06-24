import psutil
import time
import threading
import csv
import json
from collections import defaultdict
from rich.console import Console
from rich.table import Table
import matplotlib.pyplot as plt
import matplotlib.animation as animation

class Monitor:
    def __init__(self, interval=2):
        self.interval = interval
        self.stats = []
        self._stop_event = threading.Event()
        self.thread = threading.Thread(target=self._monitor_loop, daemon=True)
        self.console = Console()

    def _monitor_loop(self):
        while not self._stop_event.is_set():
            stat = {
                'time': time.strftime('%H:%M:%S'),
                'cpu': psutil.cpu_percent(interval=None),
                'ram': psutil.virtual_memory().percent,
                'disk': psutil.disk_usage('/').percent,
                'net_sent': psutil.net_io_counters().bytes_sent,
                'net_recv': psutil.net_io_counters().bytes_recv,
            }
            self.stats.append(stat)
            time.sleep(self.interval)

    def start(self):
        self.thread.start()

    def stop(self):
        self._stop_event.set()
        self.thread.join()

    def export_csv(self, filename):
        if not self.stats:
            return
        with open(filename, 'w', newline='') as f:
            writer = csv.DictWriter(f, fieldnames=self.stats[0].keys())
            writer.writeheader()
            writer.writerows(self.stats)

    def export_json(self, filename):
        with open(filename, 'w') as f:
            json.dump(self.stats, f, indent=2)

    def print_summary(self):
        if not self.stats:
            print('No stats collected.')
            return
        cpu_vals = [s['cpu'] for s in self.stats]
        ram_vals = [s['ram'] for s in self.stats]
        disk_vals = [s['disk'] for s in self.stats]
        net_sent = [s['net_sent'] for s in self.stats]
        net_recv = [s['net_recv'] for s in self.stats]
        print('\n--- Summary Report ---')
        print(f"CPU: avg={sum(cpu_vals)/len(cpu_vals):.1f}%, max={max(cpu_vals):.1f}%")
        print(f"RAM: avg={sum(ram_vals)/len(ram_vals):.1f}%, max={max(ram_vals):.1f}%")
        print(f"Disk: avg={sum(disk_vals)/len(disk_vals):.1f}%, max={max(disk_vals):.1f}%")
        print(f"Network sent: {net_sent[-1] - net_sent[0]} bytes, recv: {net_recv[-1] - net_recv[0]} bytes")

    def live_graph(self):
        plt.ion()
        fig, ax = plt.subplots()
        cpu_vals, ram_vals, disk_vals, times = [], [], [], []
        line_cpu, = ax.plot([], [], label='CPU %')
        line_ram, = ax.plot([], [], label='RAM %')
        line_disk, = ax.plot([], [], label='Disk %')
        ax.set_ylim(0, 100)
        ax.set_xlabel('Time (samples)')
        ax.set_ylabel('Usage (%)')
        ax.legend()
        fig.suptitle('Live System Usage')

        def update(frame):
            if not self.stats:
                return line_cpu, line_ram, line_disk
            cpu_vals.append(self.stats[-1]['cpu'])
            ram_vals.append(self.stats[-1]['ram'])
            disk_vals.append(self.stats[-1]['disk'])
            times.append(len(times))
            line_cpu.set_data(times, cpu_vals)
            line_ram.set_data(times, ram_vals)
            line_disk.set_data(times, disk_vals)
            ax.set_xlim(0, max(10, len(times)))
            return line_cpu, line_ram, line_disk

        ani = animation.FuncAnimation(fig, update, interval=self.interval*1000, blit=False)
        plt.show(block=True) 