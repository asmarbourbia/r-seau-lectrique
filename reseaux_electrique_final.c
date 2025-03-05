#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <gtk/gtk.h>
#include <cairo.h>

#define MAX_NODES 100
#define INF INT_MAX

typedef struct {
    int src, dest, weight;
} Edge;

typedef struct {
    int V, E;
    Edge edges[MAX_NODES * MAX_NODES];
    Edge originalEdges[MAX_NODES * MAX_NODES];
    int originalE;
} Graph;

Graph* globalGraph;
Edge removedEdgesStack[MAX_NODES * MAX_NODES];
int removedEdgesCount = 0;
GtkWidget *drawing_area;

int positions[MAX_NODES][2] = {
    {300, 300}, {500, 300}, {300, 500}, {500, 500}
};

Graph* createGraph(int V, int E) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->V = V;
    graph->E = E;
    graph->originalE = E;
    return graph;
}

void dijkstra(Graph* graph, int start) {
    int dist[MAX_NODES], visited[MAX_NODES];
    for (int i = 0; i < graph->V; i++) {
        dist[i] = INF;
        visited[i] = 0;
    }
    dist[start] = 0;

    for (int count = 0; count < graph->V - 1; count++) {
        int minDist = INF, minIndex = -1;
        for (int v = 0; v < graph->V; v++)
            if (!visited[v] && dist[v] <= minDist) minDist = dist[v], minIndex = v;

        if (minIndex == -1) break;
        visited[minIndex] = 1;

        for (int i = 0; i < graph->E; i++) {
            Edge e = graph->edges[i];
            int neighbor = (e.src == minIndex) ? e.dest : (e.dest == minIndex) ? e.src : -1;
            if (neighbor != -1 && !visited[neighbor] && dist[minIndex] != INF && dist[minIndex] + e.weight < dist[neighbor]) {
                dist[neighbor] = dist[minIndex] + e.weight;
            }
        }
    }

    printf("🔹 Chemins optimaux depuis la centrale :\n");
    for (int i = 0; i < graph->V; i++) {
        if (dist[i] == INF)
            printf("⚠ Maison %d : INJOIGNABLE\n", i);
        else
            printf("🏠 Maison %d : %d unités de perte\n", i, dist[i]);
    }
}

void simulateFailure(GtkWidget *widget, gpointer user_data) {
    Graph* graph = (Graph*)user_data;

    if (graph->E == 0) {
        printf("🚨 Plus de lignes à supprimer !\n");
        return;
    }

    GtkWidget *dialog = gtk_message_dialog_new(
        NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
        "Voulez-vous vraiment supprimer une ligne électrique ?"
    );

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES) {
        Edge removedEdge = graph->edges[0];
        removedEdgesStack[removedEdgesCount++] = removedEdge;

        for (int i = 0; i < graph->E - 1; i++)
            graph->edges[i] = graph->edges[i + 1];
        graph->E--;

        printf("❌ Ligne supprimée : %d-%d (%d)\n",
               removedEdge.src, removedEdge.dest, removedEdge.weight);
        dijkstra(graph, 0);
        gtk_widget_queue_draw(drawing_area);
    }
    gtk_widget_destroy(dialog);
}

void restoreEdge(GtkWidget *widget, gpointer user_data) {
    Graph* graph = (Graph*)user_data;

    if (removedEdgesCount == 0) {
        printf("🚨 Aucune ligne à restaurer !\n");
        return;
    }

    Edge restoredEdge = removedEdgesStack[--removedEdgesCount];
    if (graph->E < MAX_NODES * MAX_NODES) {
        graph->edges[graph->E++] = restoredEdge;
        printf("✅ Ligne restaurée : %d-%d (%d)\n",
               restoredEdge.src, restoredEdge.dest, restoredEdge.weight);
        dijkstra(graph, 0);
        gtk_widget_queue_draw(drawing_area);
    }
}

void resetNetwork(GtkWidget *widget, gpointer user_data) {
    Graph* graph = (Graph*)user_data;

    for (int i = 0; i < graph->originalE; i++)
        graph->edges[i] = graph->originalEdges[i];

    graph->E = graph->originalE;
    removedEdgesCount = 0;
    dijkstra(graph, 0);
    gtk_widget_queue_draw(drawing_area);
}

static gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
    cairo_paint(cr);

    // Dessiner les lignes supprimées
    for (int i = 0; i < removedEdgesCount; i++) {
        Edge e = removedEdgesStack[i];
        cairo_set_source_rgb(cr, 1, 0, 0);
        cairo_set_line_width(cr, 2);
        cairo_move_to(cr, positions[e.src][0], positions[e.src][1]);
        cairo_line_to(cr, positions[e.dest][0], positions[e.dest][1]);
        cairo_stroke(cr);
    }

    // Dessiner les lignes actives
    for (int i = 0; i < globalGraph->E; i++) {
        Edge e = globalGraph->edges[i];
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_set_line_width(cr, 2);
        cairo_move_to(cr, positions[e.src][0], positions[e.src][1]);
        cairo_line_to(cr, positions[e.dest][0], positions[e.dest][1]);
        cairo_stroke(cr);

        char text[32];
        snprintf(text, sizeof(text), "%d", e.weight);
        cairo_move_to(cr, (positions[e.src][0] + positions[e.dest][0])/2 + 5, 
                         (positions[e.src][1] + positions[e.dest][1])/2);
        cairo_show_text(cr, text);
    }

    // Dessiner les nœuds avec numéros
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 14);
    for (int i = 0; i < globalGraph->V; i++) {
        // Cercle
        cairo_set_source_rgb(cr, i == 0 ? 0 : 0, 0, i == 0 ? 1 : 0);
        cairo_arc(cr, positions[i][0], positions[i][1], 20, 0, 2 * M_PI);
        cairo_fill(cr);
        
        // Texte
        cairo_set_source_rgb(cr, 1, 1, 1);
        char label[3];
        snprintf(label, sizeof(label), "%d", i);
        cairo_text_extents_t extents;
        cairo_text_extents(cr, label, &extents);
        cairo_move_to(cr, 
            positions[i][0] - extents.width/2 - extents.x_bearing/2, 
            positions[i][1] + extents.height/2 - extents.y_bearing/2
        );
        cairo_show_text(cr, label);
    }

    // Légende
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_move_to(cr, 20, 30);
    cairo_show_text(cr, "Légende:");

    cairo_set_source_rgb(cr, 0, 0, 1);
    cairo_arc(cr, 20, 50, 8, 0, 2 * M_PI);
    cairo_fill(cr);
    cairo_move_to(cr, 35, 55);
    cairo_show_text(cr, "Centrale");

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_arc(cr, 20, 80, 8, 0, 2 * M_PI);
    cairo_fill(cr);
    cairo_move_to(cr, 35, 85);
    cairo_show_text(cr, "Maison");

    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_move_to(cr, 20, 110);
    cairo_show_text(cr, "Ligne supprimée");

    return FALSE;
}

void launch_gui(Graph* graph) {
    globalGraph = graph;
    GtkWidget *window, *box, *button_box;

    gtk_init(NULL, NULL);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Réseau Électrique Intelligent");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 800);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), box);

    drawing_area = gtk_drawing_area_new();
    gtk_box_pack_start(GTK_BOX(box), drawing_area, TRUE, TRUE, 0);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), NULL);

    button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_set_spacing(GTK_BOX(button_box), 5);
    gtk_box_pack_start(GTK_BOX(box), button_box, FALSE, FALSE, 5);

    GtkWidget *btn_simulate = gtk_button_new_with_label("💡 Simuler panne");
    GtkWidget *btn_restore = gtk_button_new_with_label("🔄 Restaurer");
    GtkWidget *btn_reset = gtk_button_new_with_label("♻ Réinitialiser");

    g_signal_connect(btn_simulate, "clicked", G_CALLBACK(simulateFailure), graph);
    g_signal_connect(btn_restore, "clicked", G_CALLBACK(restoreEdge), graph);
    g_signal_connect(btn_reset, "clicked", G_CALLBACK(resetNetwork), graph);

    gtk_container_add(GTK_CONTAINER(button_box), btn_simulate);
    gtk_container_add(GTK_CONTAINER(button_box), btn_restore);
    gtk_container_add(GTK_CONTAINER(button_box), btn_reset);

    gtk_widget_show_all(window);
    gtk_main();
}

int main() {
    setlocale(LC_ALL, "C.UTF-8");

    int V = 4, E = 5;
    Graph* graph = createGraph(V, E);

    Edge initialEdges[] = {
        {0, 1, 10}, {0, 2, 6}, {0, 3, 5},
        {1, 3, 15}, {2, 3, 4}
    };

    for (int i = 0; i < E; i++) {
        graph->edges[i] = initialEdges[i];
        graph->originalEdges[i] = initialEdges[i];
    }

    dijkstra(graph, 0);
    launch_gui(graph);

    free(graph);
    return 0;
}