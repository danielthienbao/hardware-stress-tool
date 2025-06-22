# Usage Guide

How to use the Hardware Stress Diagnostic Tool like a pro!

## Basic Usage

Run the program and you'll see a menu:
```
========================================
    Hardware Stress Diagnostic Tool    
========================================
Cross-platform hardware stress testing
and system monitoring utility

System Information:
==================
CPU Cores: 8
Total RAM: 8033 MB
...

Select test type:
1. CPU Stress Test
2. Memory Stress Test
3. Disk Stress Test
4. GPU Simulation Test
5. Combined Stress Test
6. System Monitor Demo
7. Fault Injection Demo
0. Exit
Choice: 
```

Just type a number and hit Enter!

## Test Types Explained

### 1. CPU Stress Test
**What it does:** Maxes out all your CPU cores with math calculations
**Duration:** 10 seconds by default
**Good for:** 
- Testing CPU stability
- Checking cooling performance
- Finding overclocking limits

**What you'll see:**
```
CPU Stress Test Starting...
Duration: 10 seconds
Progress: [====================]

Test Results:
=============
Duration: 10.1 seconds
Operations: 1,234,567
Errors: 0
Status: PASSED
```

### 2. Memory Stress Test
**What it does:** Allocates lots of RAM and fills it with random data
**Duration:** 10 seconds
**Good for:**
- Testing RAM stability
- Finding memory errors
- Checking available memory

### 3. Disk Stress Test
**What it does:** Creates temporary files and does lots of read/write operations
**Duration:** 10 seconds
**Good for:**
- Testing hard drive performance
- Finding disk errors
- Checking I/O speed

**Note:** Creates temporary files that are automatically cleaned up

### 4. GPU Simulation Test
**What it does:** Simulates GPU workload with matrix math (runs on CPU)
**Duration:** 10 seconds
**Good for:**
- Testing parallel processing
- Simulating graphics workload
- Learning about matrix operations

### 5. Combined Test
**What it does:** Runs CPU and Memory tests at the same time
**Good for:** Overall system stress testing

### 6. System Monitor Demo
**What it does:** Shows real-time system stats for 15 seconds
**What you'll see:**
```
System Monitor Demo
===================
Monitoring system metrics for 15 seconds...

CPU: 45.2%  Memory: 67.8%  Disk: 23.4%  RAM: 5432MB total
```

### 7. Fault Injection Demo
**What it does:** Simulates system failures and recovery
**Good for:** Testing how programs handle errors

## Configuration (Advanced)

You can customize settings by editing `config/default.json`:

```json
{
  "cpu_test": {
    "duration_seconds": 60,    // How long to run
    "thread_count": 0,         // 0 = auto-detect cores
    "target_load": 0.8         // 80% CPU load target
  },
  "memory_test": {
    "memory_size_mb": 100,     // How much RAM to use
    "duration_seconds": 60
  }
}
```

## Command Line Options (Coming Soon)

Future versions will support:
```bash
# Run specific test for custom duration
./stress_tool --cpu --duration=30

# Run in quiet mode
./stress_tool --quiet --memory

# Save results to file
./stress_tool --output=results.json
```

## Reading Log Files

The tool creates `stress_tool.log` with detailed information:
```
[2024-06-22 14:30:15.123] INFO: System monitoring started
[2024-06-22 14:30:16.456] INFO: CPU stress test started - 8 threads
[2024-06-22 14:30:26.789] INFO: CPU stress test completed
```

## Safety Tips

⚠️ **Important Safety Information**

### Temperature Monitoring
- Watch your CPU temperature during tests
- Normal: 30-60°C idle, 60-80°C under load
- Concerning: Above 85°C
- **Stop tests if temperature exceeds 90°C**

### System Performance
- Tests will slow down your computer temporarily
- Close other important programs before testing
- Don't run tests during important work

### Duration Recommendations
- **Short tests (10-30 seconds):** Safe for most systems
- **Medium tests (1-5 minutes):** Good for stability testing
- **Long tests (10+ minutes):** Only with good cooling

### When to Stop a Test
- Computer becomes unresponsive
- Unusual noises from fans or hard drive
- System temperature too high
- Blue screen or crash (rare but possible)

## Interpreting Results

### CPU Test Results
```
Operations: 1,234,567    // Higher = faster CPU
Errors: 0               // Should always be 0
Status: PASSED          // FAILED means system problem
```

### Memory Test Results
- **High operations count:** Fast RAM
- **Any errors:** Possible RAM problems
- **Test fails:** May indicate hardware issues

### What "Good" Results Look Like
- Zero errors in all tests
- Consistent performance across runs
- System remains responsive
- No crashes or freezes

## Troubleshooting

### Test Results Look Wrong
- Run the test multiple times
- Close other programs
- Check if system is overheating

### Program Crashes
- Check the log file: `stress_tool.log`
- Try shorter test durations
- Run Python demo to isolate issues

### Low Performance
- Normal if other programs are running
- Check if CPU is throttling due to heat
- Ensure laptop is plugged in (not on battery)

## Educational Value

This tool teaches you about:
- **System resources:** CPU, memory, disk I/O
- **Multithreading:** How programs use multiple CPU cores
- **System monitoring:** Reading performance metrics
- **Error handling:** How programs deal with failures
- **Cross-platform code:** Writing code that works everywhere

## Experiment Ideas

Try these modifications:
1. Change test durations in the config file
2. Run tests while monitoring with Task Manager
3. Compare results on different computers
4. Run tests before and after system updates
5. Monitor temperature during different test types

---

**Remember:** This is a learning tool. Experiment safely and have fun exploring how your computer works! 🚀 