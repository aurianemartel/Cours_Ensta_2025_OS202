#!/bin/bash

echo "🔧 Setting up a stable benchmarking environment..."

# 1. Disable CPU Frequency Scaling (Set to Performance Mode)
echo "🔄 Disabling CPU frequency scaling..."
if command -v cpupower &> /dev/null; then
    sudo cpupower frequency-set -g performance
elif command -v cpufreq-set &> /dev/null; then
    for cpu in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor; do
        echo performance | sudo tee "$cpu" > /dev/null
    done
else
    echo "⚠️ cpupower or cpufreq-set not found. Please install them."
    exit 1
fi

# 2. Disable Turbo Boost (Prevents Thermal Throttling)
echo "🚫 Disabling Turbo Boost..."
if [ -f /sys/devices/system/cpu/intel_pstate/no_turbo ]; then
    echo 1 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo > /dev/null
elif [ -f /sys/devices/system/cpu/cpufreq/boost ]; then
    echo 0 | sudo tee /sys/devices/system/cpu/cpufreq/boost > /dev/null
else
    echo "⚠️ Turbo Boost control not found. Skipping..."
fi

# 3. Pin Threads to Physical Cores
echo "📌 Pinning OpenMP threads to physical cores..."
NUM_PHYSICAL_CORES=$(lscpu | awk '/^Cœur\(s\) par socket :/ {print $4}')
export OMP_NUM_THREADS=$NUM_PHYSICAL_CORES
export OMP_PROC_BIND=TRUE
export OMP_PLACES=cores

# 4. Disable Background Services (e.g., system updates)
echo "🔕 Stopping background services..."
sudo systemctl stop thermald 2>/dev/null
sudo systemctl stop ondemand 2>/dev/null
sudo systemctl stop snapd 2>/dev/null
sudo systemctl stop packagekit 2>/dev/null

# 5. Clear Cached Memory (Optional, for consistent memory benchmarks)
echo "🧹 Clearing cached memory..."
sync && echo 3 | sudo tee /proc/sys/vm/drop_caches > /dev/null

# 6. Display System Status
echo "📊 System settings after optimization:"
echo " - CPU governor: $(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor)"
echo " - Turbo Boost: $(cat /sys/devices/system/cpu/intel_pstate/no_turbo 2>/dev/null || cat /sys/devices/system/cpu/cpufreq/boost 2>/dev/null)"
echo " - OpenMP Threads: $OMP_NUM_THREADS"

echo "✅ Benchmark environment is now stable!"