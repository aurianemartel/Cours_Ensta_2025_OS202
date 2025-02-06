# Calcul de l'ensemble de Mandelbrot en python
import numpy as np
from dataclasses import dataclass
from PIL import Image
from math import log
from time import time
import matplotlib as plt
import matplotlib.cm
from mpi4py import MPI

def pprint(*args, **kwargs):
    print(f"[{rank:02d}]", *args, **kwargs)

@dataclass
class MandelbrotSet:
    max_iterations: int
    escape_radius:  float = 2.0

    def __contains__(self, c: complex) -> bool:
        return self.stability(c) == 1

    def convergence(self, c: complex, smooth=False, clamp=True) -> float:
        value = self.count_iterations(c, smooth)/self.max_iterations
        return max(0.0, min(value, 1.0)) if clamp else value

    def count_iterations(self, c: complex,  smooth=False) -> int | float:
        z:    complex
        iter: int

        # On vérifie dans un premier temps si le complexe
        # n'appartient pas à une zone de convergence connue :
        #   1. Appartenance aux disques  C0{(0,0),1/4} et C1{(-1,0),1/4}
        if c.real*c.real+c.imag*c.imag < 0.0625:
            return self.max_iterations
        if (c.real+1)*(c.real+1)+c.imag*c.imag < 0.0625:
            return self.max_iterations
        #  2.  Appartenance à la cardioïde {(1/4,0),1/2(1-cos(theta))}
        if (c.real > -0.75) and (c.real < 0.5):
            ct = c.real-0.25 + 1.j * c.imag
            ctnrm2 = abs(ct)
            if ctnrm2 < 0.5*(1-ct.real/max(ctnrm2, 1.E-14)):
                return self.max_iterations
        # Sinon on itère
        z = 0
        for iter in range(self.max_iterations):
            z = z*z + c
            if abs(z) > self.escape_radius:
                if smooth:
                    return iter + 1 - log(log(abs(z)))/log(2)
                return iter
        return self.max_iterations


# On peut changer les paramètres des deux prochaines lignes
mandelbrot_set = MandelbrotSet(max_iterations=50, escape_radius=10)
width, height = 1024, 1024

scaleX = 3./width
scaleY = 2.25/height

# Calcul de l'ensemble de mandelbrot : 
deb = time()

# Initialisation de l'environnement MPI
globCom = MPI.COMM_WORLD.Dup()
nbp     = globCom.size
rank    = globCom.rank
name    = MPI.Get_processor_name()

height_bloc = height//nbp #uniquement pour nbp pair
if height%nbp != 0:
    #plantage
    raise RuntimeError("Nombre de processeurs pairs uniquement")

loc_convergence = np.empty((width, height_bloc), dtype=np.double)
glob_convergence = None

if rank==0:
    #glob_array = np.empty(N,dtype=np.int64)
    glob_convergence = np.empty((width, height), dtype=np.double)
    pprint(f"Shape glob_convergence :{glob_convergence.shape}\n")

pprint(f"Lignes traités :{rank*height_bloc} - {(rank+1)*height_bloc}\n")

# Boulot du processeur
for y in range(0, height_bloc):
    for x in range(width):
        c = complex(-2. + scaleX*x, -1.125 + scaleY*(y+rank*height_bloc))
        loc_convergence[x, y] = mandelbrot_set.convergence(c, smooth=True)

pprint(f"Shape loc_convergence :{loc_convergence.shape}\n")

# Gather : 
#globCom.Gather(loc_convergence, glob_convergence)

glob_convergence = globCom.gather(loc_convergence, root=0)

if rank==0:
    fin = time()
    print(f"Temps du calcul de l'ensemble de Mandelbrot : {fin-deb}")

    glob_convergence = np.concatenate(glob_convergence, axis=1)

    # Constitution de l'image résultante :
    deb = time()
    image = Image.fromarray(np.uint8(matplotlib.cm.plasma(glob_convergence.T)*255))
    fin = time()
    print(f"Temps de constitution de l'image : {fin-deb}")
    image.show()
