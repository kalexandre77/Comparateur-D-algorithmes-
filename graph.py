import matplotlib.pyplot as plt
import sys

if len(sys.argv) < 3:
    print("Il manque un fichier en arguments")
    sys.exit(1)

nom_fichier = sys.argv[1]
option = sys.argv[2]


fichier = open(nom_fichier,'r')

data_temps = {
        "algo1": {"x": [], "y": []},
        "algo2": {"x" : [], "y" : []},
        "algo3" : {"x": [], "y" : []}
}


data_alloc = {
        "algo1": {"x": [], "y": []},
        "algo2": {"x" : [], "y" : []},
        "algo3": {"x": [], "y" : []}
}



with fichier as f:
    for l in f:
        lect = l.split()
        algo = lect[0].split('=')[1]
        nb_mots = int(lect[2].split('=')[1])
        temps = float(lect[3].split('=')[1])
        cummul_alloc = int(lect[4].split('=')[1])
        
        data_temps[algo]["y"].append(temps)
        data_temps[algo]["x"].append(nb_mots)

        data_alloc[algo]["y"].append(cummul_alloc)
        data_alloc[algo]["x"].append(nb_mots)



plt.figure(figsize=(10, 6))
if option == "t":
    for cle, valeurs in data_temps.items():
        plt.plot(valeurs["x"], valeurs["y"], marker='x', label=cle)
        plt.title("Comparaison des performances des algorithmes")
        plt.xlabel("Nombre de mots")
        plt.ylabel("Temps d'exécution (secondes)")
        plt.legend()
        
elif option == "a":
    for cle,valeurs in data_alloc.items():
        plt.plot(valeurs["x"], valeurs["y"], marker='x', label=cle)
        plt.title("Comparaison des performances des algorithmes")
        plt.xlabel("Nombre de mots")
        plt.ylabel("Allocation maximale")
        plt.legend()

plt.grid(True)
plt.savefig("courbes_performance.png")
plt.show()
