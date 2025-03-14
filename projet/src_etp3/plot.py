import matplotlib.pyplot as plt

# Données extraites de data.txt
# Temps avant optimisation
temps_avant = {
    'calcul': 2.13842,
    'total': 11.4663,
    'display': 10.7736
}

# Temps après optimisation pour différents nombres de threads
temps_apres = {
    1: {'calcul': 2.50098, 'total': 2.80132, 'display': 1.56285},
    2: {'calcul': 1.65843, 'total': 1.94176, 'display': 0.979191},
    3: {'calcul': 1.89308, 'total': 2.17403, 'display': 1.16284},
    4: {'calcul': 7.65524, 'total': 7.98798, 'display': 3.54902}
}

# Préparation des données pour le graphique
n_threads = list(temps_apres.keys())
temps_calcul_avant = [temps_avant['calcul']] * len(n_threads)
temps_total_avant = [temps_avant['total']] * len(n_threads)
temps_display_avant = [temps_avant['display']] * len(n_threads)

temps_calcul_apres = [temps_apres[threads]['calcul'] for threads in n_threads]
temps_total_apres = [temps_apres[threads]['total'] for threads in n_threads]
temps_display_apres = [temps_apres[threads]['display'] for threads in n_threads]

# Création du graphique
plt.figure(figsize=(10, 6))

# Temps de calcul
plt.plot(n_threads, temps_calcul_apres, marker='o', linestyle='-', color='b', label='Temps calcul après optimisation')
plt.plot(n_threads, temps_calcul_avant, marker='o', linestyle='--', color='b', label='Temps calcul avant optimisation')

# Temps total
plt.plot(n_threads, temps_total_apres, marker='s', linestyle='-', color='g', label='Temps total après optimisation')
plt.plot(n_threads, temps_total_avant, marker='s', linestyle='--', color='g', label='Temps total avant optimisation')

# Temps d'affichage
plt.plot(n_threads, temps_display_apres, marker='^', linestyle='-', color='r', label='Temps display après optimisation')
plt.plot(n_threads, temps_display_avant, marker='^', linestyle='--', color='r', label='Temps display avant optimisation')

# Personnalisation du graphique
plt.xlabel('Nombre de threads')
plt.ylabel('Temps (secondes)')
plt.title('Comparaison des temps d\'exécution avant et après optimisation asynchrone')
plt.legend()
plt.grid(True)
plt.tight_layout()

# Sauvegarde du graphique
plt.savefig('result.png')

# Affichage du graphique
plt.show()
