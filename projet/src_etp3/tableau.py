import re

def extraire_donnees(fichier):
    """Extrait les données du fichier et les organise dans un dictionnaire."""
    donnees = {
        'avant_optim': {'calcul': None, 'total': None, 'display': None},
        'apres_optim': {}
    }
    with open(fichier, 'r') as f:
        contenu = f.read()
        # Extraction des données avant optimisation
        match_avant = re.search(r'# avant optim asynchrone\s*'
                                r'Temps calcul : (\d+\.\d+) secondes\s*'
                                r'Temps total : (\d+\.\d+) secondes\s*'
                                r'Temps display : (\d+\.\d+) secondes', contenu)
        if match_avant:
            donnees['avant_optim']['calcul'] = float(match_avant.group(1))
            donnees['avant_optim']['total'] = float(match_avant.group(2))
            donnees['avant_optim']['display'] = float(match_avant.group(3))
        # Extraction des données après optimisation
        matches_apres = re.findall(r'Nb threads (\d+)\s*'
                                   r'Temps calcul : (\d+\.\d+) secondes\s*'
                                   r'Temps total : (\d+\.\d+) secondes\s*'
                                   r'Temps display : (\d+\.\d+) secondes', contenu)
        for match in matches_apres:
            threads = int(match[0])
            donnees['apres_optim'][threads] = {
                'calcul': float(match[1]),
                'total': float(match[2]),
                'display': float(match[3])
            }
    return donnees

def generer_tableau_markdown(donnees):
    """Génère un tableau en Markdown à partir des données fournies."""
    tableau = []
    # En-tête du tableau
    en_tete = ["Temps", "Avant optimisation"]
    for threads in range(1, 5):
        en_tete.append(f"Après optimisation ({threads} threads)")
    tableau.append("| " + " | ".join(en_tete) + " |")
    tableau.append("|---" + "|---" * (len(en_tete) - 1) + "|")
    # Données pour chaque type de temps
    for temps_type in ['calcul', 'total', 'display']:
        ligne = [temps_type.capitalize()]
        ligne.append(donnees['avant_optim'][temps_type])
        for threads in range(1, 5):
            if threads in donnees['apres_optim']:
                ligne.append(donnees['apres_optim'][threads][temps_type])
            else:
                ligne.append(None)
        tableau.append("| " + " | ".join([str(valeur) if valeur is not None else "" for valeur in ligne]) + " |")
    return "\n".join(tableau)

def main():
    fichier = 'data.txt'  # Nom du fichier contenant les données
    donnees = extraire_donnees(fichier)
    tableau_markdown = generer_tableau_markdown(donnees)
    print(tableau_markdown)

if __name__ == '__main__':
    main()
