# Hardware Stress Tool

A simple Python tool to stress-test your CPU, memory, disk, network, and GPU, with live monitoring and logging.

## Quick Install
```
pip install -r requirements.txt
```

## Usage Example
```
python stress_tool.py --cpu 2 --memory 1GB --disk 1GB --duration 30 --network-url https://example.com --gpu --live-graph
```

- Use `--cpu`, `--memory`, `--disk`, `--network-url`, `--gpu` to stress different components.
- Add `--live-graph` to see a real-time usage graph.
- Use `--export-csv` or `--export-json` to save monitoring logs.

**Requires Python 3.7+** 