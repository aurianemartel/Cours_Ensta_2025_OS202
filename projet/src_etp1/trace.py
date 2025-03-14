import matplotlib.pyplot as plt

def tracer_et_sauvegarder_courbes():
    # Données fournies
    src_etp0 = {'total': 13.7092, 'calcul': 4.75898, 'display': 7.9162}
    donnees = {
        1: {'total': 13.9446, 'calcul': 2.29557, 'display': 10.6573},
        2: {'total': 13.6174, 'calcul': 1.5594, 'display': 11.0523},
        4: {'total': 13.4666, 'calcul': 1.67008, 'display': 10.7809}
    }

    # Extraction des données pour les tracés
    nb_threads = list(donnees.keys())
    temps_total = [src_etp0['total']] + [donnees[nt]['total'] for nt in nb_threads]
    temps_calcul = [src_etp0['calcul']] + [donnees[nt]['calcul'] for nt in nb_threads]
    temps_display = [src_etp0['display']] + [donnees[nt]['display'] for nt in nb_threads]

    # Calcul du speed-up
    speed_up = [src_etp0['calcul'] / t for t in temps_calcul]

    # Tracé des courbes
    plt.figure(figsize=(12, 6))

    # Courbes des temps
    plt.subplot(1, 2, 1)
    plt.plot([0] + nb_threads, temps_total, 'o-', label='Temps total')
    plt.plot([0] + nb_threads, temps_calcul, 's-', label='Temps calcul')
    plt.plot([0] + nb_threads, temps_display, '^-', label='Temps display')
    plt.xlabel('Nombre de threads')
    plt.ylabel('Temps (secondes)')
    plt.title('Temps en fonction du nombre de threads')
    plt.legend()
    plt.grid(True)

    # Courbe du speed-up
    plt.subplot(1, 2, 2)
    plt.plot([0] + nb_threads, speed_up, 'd-', color='purple')
    plt.xlabel('Nombre de threads')
    plt.ylabel('Speed-up')
    plt.title('Speed-up en fonction du nombre de threads')
    plt.grid(True)

    # Ajustement de la mise en page
    plt.tight_layout()

    # Sauvegarde des graphiques dans un fichier PNG
    plt.savefig('result.png', dpi=300)

    # Fermeture de la figure pour libérer la mémoire
    plt.close()

# Appel de la fonction pour tracer et sauvegarder les courbes
tracer_et_sauvegarder_courbes()
