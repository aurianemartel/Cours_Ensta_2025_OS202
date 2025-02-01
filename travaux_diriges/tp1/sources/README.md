
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
1                 |
2                 |
3                 |
4                 |
5                 |
6                 |
7                 |
8                 |

*Tracer les courbes de speedup (pour chaque valeur de n), discuter les résultats.*



### Produit par blocs

`make TestProduct.exe && ./TestProduct.exe 1024`

  szBlock         | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
------------------|---------|----------------|----------------|---------------
origine (=max)    |
32                |
64                |
128               |
256               |
512               |
1024              |

*Discuter les résultats.*



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
