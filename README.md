# PolyRealms

FR — Français
--------------

PolyRealms est une ré‑implémentation pédagogique en C++ des mécaniques principales du jeu de cartes Hero Realms. Le projet a été développé dans un cadre universitaire pour pratiquer la conception objet, la gestion de paquets de cartes (deckbuilding) et la simulation de parties en console.

Important : ce projet est une reproduction éducative et n'est pas affilié aux détenteurs des droits d'Hero Realms.

Fonctionnalités principales
- Deckbuilding : pioche, défausse, mélange, achat de cartes.
- Gestion du joueur et de l'état de la partie : points de vie, or, dégats, main...
- Résolution d'actions : attaques, soins, effets simples de cartes.
- Architecture modulaire facilitant l'ajout de nouvelles cartes et classes.

Organisation du dépôt
- cards/src/ : code source C++
- cards/include/ : en‑têtes publics
- cards/Makefile : Makefile du projet

Prérequis
- Compilateur C++ moderne (C++17 ou supérieur recommandé)
- Outils de build : make

Compilation

```bash
git clone https://github.com/IssamChafiq/PolyRealms.git
cd PolyRealms/cards
make
```

Exécution

```bash
./cards
```

État et limitations connues
- Certaines cartes ou effets avancés du jeu original peuvent ne pas être implémentés.
- Absence d'interface graphique : exécution principalement en console.

Licence et mentions légales
- Ce projet est à vocation pédagogique et n'a pas d'intention commerciale. Respectez la propriété intellectuelle du jeu original lors du partage public.

Contact / crédits
- Voir l'historique des commits pour la liste complète des contributeurs.

EN — English
--------------

PolyRealms is an educational C++ reimplementation of the core mechanics of the card game Hero Realms. The repository was created as a student project to practice object‑oriented design, deck management (deckbuilding), and console‑based game simulations.

Note: this project is an educational reimplementation and is not affiliated with the rights holders of Hero Realms.

Key features
- Deckbuilding: draw, discard, shuffle, buy cards.
- Player and game state management: health, gold, damage, hand...
- Action resolution: attacks, healing, simple card effects.
- Modular architecture to ease adding new cards and classes.

Repository layout
- cards/src/ : C++ sources
- cards/include/: public headers
- cards/Makefile : project Makefile

Requirements
- Modern C++ compiler (C++17 or newer recommended)
- Build tools: make

Build

```bash
git clone https://github.com/IssamChafiq/PolyRealms.git
cd PolyRealms/cards
make
```

Run

```bash
./cards
```

Known limitations
- Some advanced cards or effects from the original game may be missing.
- No graphical interface — the project runs primarily in console mode.

License & legal
- This repository is an educational project and is not commercially affiliated with the owners of Hero Realms.

Contact / credits
- See the Git history for contributors.
