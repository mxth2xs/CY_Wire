#!/bin/bash

# Script Shell pour le projet C-Wire 
# Auteur: Pfleger Paul, Da Costa Silva Mathias
# Usage : ./c-wire.sh chemin_du_fichier.csv type_station type_consommateur [identifiant_centrale]

# Validation des paramètres
if [ "$#" -lt 3 ]; then
    echo "Erreur : Nombre de paramètres insuffisant."
    echo "Usage : ./c-wire.sh chemin_du_fichier.csv type_station type_consommateur [identifiant_centrale]"
    exit 1
fi

# Récupération des paramètres
fichier_csv="$1"
type_station="$2"
type_consommateur="$3"
identifiant_centrale="${4:-}" # Optionnel

# Vérification du fichier d'entrée
if [ ! -f "$fichier_csv" ]; then
    echo "Erreur : Le fichier $fichier_csv n'existe pas."
    exit 1
fi

# Vérification des types de station
if [[ "$type_station" != "hvb" && "$type_station" != "hva" && "$type_station" != "lv" ]]; then
    echo "Erreur : Type de station invalide. Les options valides sont : hvb, hva, lv."
    exit 1
fi

# Vérification des types de consommateur
if [[ "$type_consommateur" != "comp" && "$type_consommateur" != "indiv" && "$type_consommateur" != "all" ]]; then
    echo "Erreur : Type de consommateur invalide. Les options valides sont : comp, indiv, all."
    exit 1
fi

# Vérification de l'identifiant de la centrale (si fourni)
if [ -n "$identifiant_centrale" ]; then
    echo "Vérification de l'ID de la centrale : $identifiant_centrale"
    id_valide=$(awk -F';' -v id="$identifiant_centrale" '$1 == id {print $1; exit}' "$fichier_csv")
    if [ -z "$id_valide" ]; then
        echo "Erreur : L'identifiant de la centrale $identifiant_centrale n'existe pas dans le fichier."
        exit 1
    fi
fi
    
# Filtrage des options interdites
if [[ "$type_station" == "hvb" && ( "$type_consommateur" == "all" || "$type_consommateur" == "indiv" ) ]]; then
    echo "Erreur : Les options hvb all ou hvb indiv sont interdites."
    exit 1
fi

if [[ "$type_station" == "hva" && ( "$type_consommateur" == "all" || "$type_consommateur" == "indiv" ) ]]; then
    echo "Erreur : Les options hva all ou hva indiv sont interdites."
    exit 1
fi

# Création des dossiers nécessaires
mkdir -p tmp tests

# Vidage du dossier tmp
rm -f tmp/* 

# Préparation du fichier temporaire
fichier_filtre="tmp/filtre_${type_station}_${type_consommateur}.csv"

# Filtrage des données dans le fichier CSV
awk -F';' -v station="$type_station" -v consommateur="$type_consommateur" -v centrale="$identifiant_centrale" '
BEGIN { OFS=";" }
{
    if (station == "hvb" && $2 != "-" && $6 == "-" && (consommateur == "comp" || consommateur == "all")) {
        if (centrale == "" || $1 == centrale) print $0;
    }
    else if (station == "hva" && $3 != "-" && $6 == "-" && (consommateur == "comp" || consommateur == "all")) {
        if (centrale == "" || $1 == centrale) print $0;
    }
    else if (station == "lv" && $4 != "-" && (consommateur == "comp" || consommateur == "indiv" || consommateur == "all")) {
        if (centrale == "" || $1 == centrale) print $0;
    }
}' "$fichier_csv" > "$fichier_filtre"

echo "Les données filtrées sont sauvegardées dans $fichier_filtre"

# Vérification et compilation du programme C
if [ ! -f "codeC/c-wire" ]; then
    echo "Compilation du programme C en cours..."
    make -C codeC
    if [ $? -ne 0 ]; then
        echo "Erreur : Échec de la compilation du programme C."
        exit 1
    fi
fi

# Exécution du programme C
echo "Exécution du programme C..."
./codeC/c-wire "$fichier_filtre" "tmp/resultat_${type_station}_${type_consommateur}.csv"
if [ $? -ne 0 ]; then
    echo "Erreur : Échec lors de l'exécution du programme C."
    exit 1
fi

echo "Résultat disponible dans tmp/resultat_${type_station}_${type_consommateur}.csv"

echo "Traitement terminé."
