# Réseau Électrique Intelligent

## Description
Ce projet simule un réseau électrique utilisant un graphe pour modéliser les connexions entre une centrale et plusieurs maisons. Il utilise l'algorithme de Dijkstra pour évaluer les chemins optimaux et permet la simulation de pannes et la restauration des lignes électriques via une interface graphique GTK.

## Fonctionnalités
- Affichage du réseau électrique avec les connexions et les poids des lignes.
- Simulation de pannes en supprimant des lignes électriques.
- Restauration des lignes supprimées.
- Réinitialisation complète du réseau.
- Calcul en temps réel des chemins les plus courts depuis la centrale.

## Technologies utilisées
- **Langage** : C
- **Bibliothèques** : GTK, Cairo

## Installation
1. Cloner le dépôt :
   ```sh
   git clone <URL_DU_REPO>
   cd <NOM_DU_REPO>
   ```
2. Installer les dépendances GTK :
   ```sh
   sudo apt-get install libgtk-3-dev
   ```
3. Compiler le projet :
   ```sh
   gcc -o reseau reseau.c `pkg-config --cflags --libs gtk+-3.0`
   ```
4. Exécuter le programme :
   ```sh
   ./reseau
   ```

## Utilisation
- Cliquez sur **"💡 Simuler panne"** pour supprimer une ligne aléatoire.
- Cliquez sur **"🔄 Restaurer"** pour remettre une ligne supprimée.
- Cliquez sur **"♻ Réinitialiser"** pour restaurer tout le réseau.
- Le réseau se met à jour dynamiquement après chaque action.

## Structure du code
- `createGraph(int V, int E)`: Crée un graphe avec `V` sommets et `E` arêtes.
- `dijkstra(Graph* graph, int start)`: Calcule les chemins les plus courts depuis `start`.
- `simulateFailure(GtkWidget *widget, gpointer user_data)`: Supprime une ligne du réseau.
- `restoreEdge(GtkWidget *widget, gpointer user_data)`: Restaure la dernière ligne supprimée.
- `resetNetwork(GtkWidget *widget, gpointer user_data)`: Réinitialise le réseau.
- `on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data)`: Dessine le réseau électrique.
- `launch_gui(Graph* graph)`: Lance l'interface graphique GTK.

## Auteur
- **Asmae Bourbia**



