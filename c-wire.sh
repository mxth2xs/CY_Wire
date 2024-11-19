#!/bin/bash

echo "Bienvenue dans le script C-Wire"

if [ -z "$1" ]; then 
	echo "erreur: aucun fichier fourni"
	echo "utilisation: ./c-wire.sh chemin_du_fichier_fournie.csv"
	exit 1
fi

fichier_csv="$1"

echo "fichier a traiter : $fichier_csv"

mkdir -p tmp

echo "traitement terminé. Les résultat sont dans le dossier tmp"

chmod +x c-wire.sh

