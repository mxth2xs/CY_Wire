#!/bin/bash

# Script de filtrage pour le projet C-Wire
# Usage : ./c-wire.sh chemin_du_fichier.csv type_station type_consommateur


# Vérification des paramètres
if [ "$#" -lt 3 ]; then
  echo "Erreur : Nombre de paramètres incorrect."
  echo "Usage : ./c-wire.sh chemin_du_fichier.csv type_station type_consommateur"
  exit 1
fi

# Paramètres
fichier_csv="$1"
type_station="$2"
type_consommateur="$3"

# Vérifier si le fichier existe
if [ ! -f "$fichier_csv" ]; then
  echo "Erreur : Le fichier $fichier_csv n'existe pas."
  exit 1
fi

# Créer un dossier temporaire
mkdir -p tmp

# Filtrer les lignes selon les paramètres (exemple avec grep)
if [ "$type_station" == "hvb" ]; then
  grep "HV-B" "$fichier_csv" > tmp/filtre_hvb.csv
  echo "Lignes HV-B filtrées dans tmp/filtre_hvb.csv"
fi

# Terminer le script
echo "Traitement terminé."
