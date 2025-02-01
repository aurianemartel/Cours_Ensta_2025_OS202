
# TD1

`pandoc -s --toc README.md --css=./github-pandoc.css -o README.html`

## lscpu

*lscpu donne des infos utiles sur le processeur : nb core, taille de cache :*

```
Processeur(s) :                         4
Thread(s) par cœur :                    2
Cœur(s) par socket :                    2
Socket:                                 1

Donc 2 coeurs réellement, qui en hyperthreading donnent 4 threads parallèles.

Caches (sum of all):                        
  L1d:                                      64 KiB (2 instances)
  L1i:                                      64 KiB (2 instances)
  L2:                                       512 KiB (2 instances)
  L3:                                       3 MiB (1 instance)
```


## Produit matrice-matrice

### Effet de la taille de la matrice

  n            | MFlops
---------------|--------
1024 (origine) |111
1023           |115
1025           |115

L'alignement de la taille de la matrice avec la longueur des blocs de cache peut engendrer des pertes d'efficacité de la gestion du cache. En effet, les lignes de caches sont organisées en sets, et le choix du set pour une case mémoire est déterminé par un modulo sur une puissance de deux. Si la matrice est d'une taille puissance de deux, quand j'itère sur une des dimensions d'une matrice, je n'ai accès qu'à certains de ces sets. C'est une limitation artificielle de la taille de mon cache, donc je dois accéder à ma mémoire RAM plus souvent.

Cela n'est pas flagrant sur mon ordinateur, peut-être que mon processeur intègre des mécanismes de gestion de cache "intelligent" qui réduisent ces pertes d'efficacité, du type prefetch intelligent.


### Permutation des boucles

*Expliquer comment est compilé le code (ligne de make ou de gcc) : on aura besoin de savoir l'optim, les paramètres, etc. Par exemple :*

`make TestProduct.exe && ./TestProduct.exe 1024`


  ordre           | time    | MFlops  | MFlops(n=2048)
------------------|---------|---------|----------------
i,k,j (origine)   | 19.6741 | 109.153 | 85.5885
i,j,k             | 8.57384 | 250.469 | 191.796
j,i,k             | 8.82752 | 243.272 | 190.058
j,k,i             | 0.896604| 2395.13 | 2419.98
k,i,j             | 18.9484 | 113.333 | 86.7403
k,j,i             | 1.05376 | 2037.92 | 2023.19


Les matrices sont rangées dans la mémoire par colonnes. C'est l'indice le plus à l'intérieur qui bouge le plus souvent. i concerne les colonnes de A et de C, le calcul est donc plus efficace quand i est au centre (j,k,i et k,j,i), puisqu'on maximise l'usage du cache pour les deux matrices à la fois. À l'inverse, le calcul est beaucoup moins efficace quand j est au centre, puisqu'en itérant sur les lignes on ne fait pas usage du cache et on accède à chaque fois à la mémoire RAM.

NB : La différence entre les tests en 1024 et 2048 peut être due à d'autres événements, comme un changement de fenêtre pour noter les réponses.

### OMP sur la meilleure boucle

`make TestProduct.exe && OMP_NUM_THREADS=8 ./TestProduct.exe 1024`

  OMP_NUM         | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
------------------|---------|----------------|----------------|---------------
1                 | 1680.99 | 1797.09        | 1858.21        | 1806.02
2                 | 1804.16 | 1796.25        | 1889.71        | 1801.93
3                 | 1780.12 | 1803.79        | 1908.36        | 1788.43
4                 | 1828.35 | 1801.88        | 1985.42        | 1794.83
5                 | 1785.39 | 1800.98        | 1951.01        | 1796.23
6                 | 1804.33 | 1792.45        | 1852           | 1792.34
7                 | 1776.96 | 1805.81        | 1881.95        | 1794.52
8                 | 1770.39 | 1745.89        | 1863.8         | 1791.85

Il n'y pas d'amélioration significative. Il y a une faible amélioration pour n=512 à 4 ou 5 threads, mais c'est minime. Le calcul lui-même n'est pas le facteur limitant: même avec la combinaison de boucle kji que j'ai utilisée, c'est l'accès mémoire qui diminue l'efficacité.

1.4 : L'accéleration due à la parallélisation est limitée par l'accès en mémoire, et cet accès en mémoire n'est pas encore complètement utilisé. Il faudrait organiser les calculs pour qu'ils utilisent au maximum le cache disponible avant de le modifier en accédant à la mémoire RAM.

### Produit par blocs

`make TestProduct.exe && ./TestProduct.exe 1024`

  szBlock         | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
------------------|---------|----------------|----------------|---------------
origine (=4096)   | 2322.19 | 2488.82        | 2765.75        | 2387.66
32                | 2465.22 | 2373.83        | 2391.21        | 2300.81
64                | 2257.89 | 2239.47        | 2193.91        | 2136.79
128               | 2534.99 | 2571.12        | 2469.65        | 2436.67
256               | 2592.07 | 2732.97        | 2630.09        | 2331.32
512               | 2365.07 | 2346.36        | 2767.9         | 2349
1024              | 2478.92 | 2301.5         | 2553.46        | 2336.54


On observe globalement peu d'amélioration en séquentiel par rapport au calcul sans blocks (origine). On peut supposer que le facteur limitant est ici le temps de calcul du processeur.


### Bloc + OMP


  szBlock      | OMP_NUM | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)|
---------------|---------|---------|----------------|----------------|---------------|
1024           |  1      |         |                |                |               |
1024           |  8      |         |                |                |               |
512            |  1      |         |                |                |               |
512            |  8      |         |                |                |               |

*Discuter les résultats.*


### Comparaison avec BLAS, Eigen et numpy

*Comparer les performances avec un calcul similaire utilisant les bibliothèques d'algèbre linéaire BLAS, Eigen et/ou numpy.*


# Tips

```
	env
	OMP_NUM_THREADS=4 ./produitMatriceMatrice.exe
```

```
    $ for i in $(seq 1 4); do elap=$(OMP_NUM_THREADS=$i ./TestProductOmp.exe|grep "Temps CPU"|cut -d " " -f 7); echo -e "$i\t$elap"; done > timers.out
```
