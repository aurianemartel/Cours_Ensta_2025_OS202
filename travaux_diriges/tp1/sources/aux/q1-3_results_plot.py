import matplotlib.pyplot as plt

# Data from the table
omp_nums = [1, 2, 3, 4, 5, 6, 7, 8]
mflops_n1024 = [1789.59, 2809.43, 2716.26, 3364.56, 3084.71, 3199.67, 3370.68, 3528.89]
mflops_n2048 = [1866.56, 2735.33, 3035.49, 3283.31, 3124.86, 3313.94, 3338.62, 3391.87]
mflops_n512 = [1868.86, 1939.28, 3033.78, 3003.24, 3262.53, 3311.39, 3437.97, 3221.35]
mflops_n4096 = [1745.08, 2792.64, 2976.46, 3185.23, 3226.81, 3258.23, 3307.09, 3347.77]

# Plotting
plt.figure(figsize=(10, 6))
plt.plot(omp_nums, mflops_n1024, marker='o', linestyle='-', label='n=1024')
plt.plot(omp_nums, mflops_n2048, marker='s', linestyle='--', label='n=2048')
plt.plot(omp_nums, mflops_n512, marker='^', linestyle='-.', label='n=512')
plt.plot(omp_nums, mflops_n4096, marker='x', linestyle=':', label='n=4096')

plt.xlabel('OMP_NUM')
plt.ylabel('MFlops')
plt.title('MFlops vs. OMP_NUM for different matrix sizes (n)')
plt.xticks(omp_nums)
plt.legend()
plt.grid(True)
plt.savefig("q1-3_plots.jpg")
plt.show()