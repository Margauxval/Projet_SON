# Projet_SON

# Description du projet

Ce programme transforme une Teensy en un sampler simple et réactif capable d’enregistrer un son, de le stocker temporairement, puis de le rejouer sous forme de notes MIDI via un clavier virtuel comme VMPK. L’ensemble repose sur la Teensy Audio Library, qui fournit les outils nécessaires pour la capture, le traitement et la lecture du signal audio.
Le fonctionnement repose sur trois éléments principaux :
- La capture du son via un micro branché sur l’Audioshield.
- Le contrôle utilisateur assuré par un bouton (enregistrement).
- La génération MIDI permettant de jouer l’échantillon enregistré à différentes hauteurs.

---

# Notice d’utilisation

**1. Préparation**  
- Intsaller un bouton, un casque et un micro sur la Teensy.
- Brancher la Teensy à l’ordinateur.  
- Ouvrir VMPK pour recevoir les notes MIDI.  
- Brancher les écouteurs sur la sortie audio.

**2. Enregistrer un son**  
- Appuyer sur le bouton pour commencer l’enregistrement.  
- Parler ou produire un son près du micro.  
- Relâcher le bouton pour terminer l’enregistrement.

**3. Jouer le son**  
- Utiliser le clavier virtuel VMPK : chaque touche joue l’échantillon à une hauteur différente.  
- Écouter le résultat via les écouteurs.

**4. Réenregistrer**  
- Appuyer à nouveau sur le bouton pour capturer un nouveau son.

---

# Améliorations

- effets (echo, orgue, etc.)
- polyphonie
- possibilité d'enregistrer et d'utiliser plusieurs sons
- utilisation d'un clavier matériel 



