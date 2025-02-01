import matplotlib.pyplot as plt

# Corrected data from the table
sz_blocks = [1024, 512, 256, 128]
omp_nums = [1, 2, 4, 8]

mflops_n1024_sz1024 = [2009.26, 2989.19, 3805.23, 3702.36] # MFlops column
mflops_n1024_sz512 =  [1809.59, 3292.16, 3687.35, 3742.68]
mflops_n1024_sz256 =  [2105.94, 3971.44, 3303.29, 3890.75]
mflops_n1024_sz128 =  [2025.56, 3836.3,  3189.98, 3837.83]

mflops_n2048_sz1024 = [1868.78, 3368.09, 3679.67, 3646.61] # MFlops(n=2048) column
mflops_n2048_sz512 =  [1816.55, 3442.19, 3706.96, 3748.59]
mflops_n2048_sz256 =  [2080.55, 3610,    3976.73, 3863.7]
mflops_n2048_sz128 =  [2046.62, 3786.41, 3867.11, 3707.77]

mflops_n512_sz1024 =  [2018.31, 3366.47, 3866.95, 3774.61] # MFlops(n=512) column
mflops_n512_sz512 =   [2048.59, 3801.31, 3867.5,  3866.78]
mflops_n512_sz256 =   [2053.34, 3723.38, 3775.78, 3711.48]
mflops_n512_sz128 =   [2030.66, 3713.75, 3548.92, 3615.42]

mflops_n4096_sz1024 = [1882.1,  3438.61, 3684.3,  3666.21] # MFlops(n=4096) column
mflops_n4096_sz512 =  [1802.6,  3396.5,  3729.47, 3732.73]
mflops_n4096_sz256 =  [1908.38, 3720.51, 3870.94, 3672.22]
mflops_n4096_sz128 = [1966.6,  3639.35, 3774.33, 3657.45]


n_values = [1024, 2048, 512, 4096]
mflops_data = {
    1024: {
        1024: mflops_n1024_sz1024,
        512:  mflops_n1024_sz512,
        256:  mflops_n1024_sz256,
        128:  mflops_n1024_sz128,
    },
    2048: {
        1024: mflops_n2048_sz1024,
        512:  mflops_n2048_sz512,
        256:  mflops_n2048_sz256,
        128:  mflops_n2048_sz128,
    },
    512: {
        1024: mflops_n512_sz1024,
        512:  mflops_n512_sz512,
        256:  mflops_n512_sz256,
        128:  mflops_n512_sz128,
    },
    4096: {
        1024: mflops_n4096_sz1024,
        512:  mflops_n4096_sz512,
        256:  mflops_n4096_sz256,
        128:  mflops_n4096_sz128,
    }
}

sz_block_labels = {
    1024: 'szBlock=1024',
    512:  'szBlock=512',
    256:  'szBlock=256',
    128:  'szBlock=128',
}
markers = ['o', 's', '^', 'x']
linestyles = ['-', '--', '-.', ':']

fig, axes = plt.subplots(2, 2, figsize=(12, 8), sharey=True) # Create a 2x2 grid of subplots
axes = axes.flatten() # Flatten the 2D array of axes into a 1D array for easier iteration

for i, n in enumerate(n_values):
    ax = axes[i]
    for j, sz_block in enumerate(sz_blocks):
        ax.plot(omp_nums, mflops_data[n][sz_block], marker=markers[j], linestyle=linestyles[j], label=sz_block_labels[sz_block])

    ax.set_xlabel('OMP_NUM')
    ax.set_ylabel('MFlops')
    ax.set_title(f'MFlops vs. OMP_NUM for n={n}')
    ax.set_xticks(omp_nums)
    ax.grid(True)
    ax.legend()

fig.suptitle('MFlops Performance for Different Matrix Sizes (n) and szBlock Sizes', fontsize=14) # Overall title for the figure
fig.tight_layout(rect=[0, 0, 1, 0.95]) # Adjust layout to prevent title overlap, leave space for suptitle

plt.savefig("q1-7_plots_multi.jpg")
plt.show()