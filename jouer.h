#ifndef JOUER_H_INCLUDED
#define JOUER_H_INCLUDED

int niveauIA(SDL_Surface *ecran, TTF_Font *police);
void jouerCIA(SDL_Surface *ecran, int niveau);
int jouerDuel(SDL_Surface *ecran);
int placerElement(SDL_Rect *position, int terrain[][3], int joueur);
int etatPartie(int terrain[][3], int joueur1);
void dessinerTerrain(SDL_Surface *ecran, SDL_Surface *terrainMorpion, SDL_Surface *cercle, SDL_Surface *croix, SDL_Surface *cercle_win, SDL_Surface *croix_win, int terrain[][3]);
void jouerAleatoirement(int terrain[][3], int joueur);
void premierCoup(int terrain[][3], int numeroOrdinateur);
void jouerOrdinateur(int terrain[][3], int numeroOrdinateur);
int canWin(int terrain[][3], int premierAJouer);
int canLoose(int terrain[][3], int premierAJouer);
int tourBot(int niveau, int terrain[][3], int premierAJouer, int tour);

#endif // JOUER_H_INCLUDED
