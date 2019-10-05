#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "main.h"
#include "constantes.h"
#include "jouer.h"

int main ( int argc, char** argv )
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Surface *ecran = NULL, *texte = NULL;
    SDL_Event event;
    SDL_Rect position;

    int continuer = 1, retour = 0;

    TTF_Font *police = NULL;
    SDL_Color couleurBlanche = {255, 255, 255}, couleurNoir = {0, 0, 0};//Variables

    position.x = 0;
    position.y = 0;

    lancement();

    if(TTF_Init() == -1)//Initialisation de la TTF
    {
        fprintf(stderr, "Erreur lors de l'initialisation de la TTF : %s\n", TTF_GetError);
        exit(EXIT_FAILURE);
    }
    police = TTF_OpenFont("data/Limousines.ttf", 50);

    ecran = SDL_SetVideoMode(LARGEUR_FENETRE, LONGUEUR_FENETRE, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    SDL_WM_SetCaption("Morpion", NULL);
    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));

    texte = TTF_RenderText_Shaded(police,"1. Mode 1 Joueur" , couleurNoir, couleurBlanche);//Phrase 1
    position.x = LARGEUR_FENETRE/2 - (texte->w / 2);
    position.y = LONGUEUR_FENETRE/2 - (texte->h);
    SDL_BlitSurface(texte, NULL, ecran, &position);

    texte = TTF_RenderText_Shaded(police,"2. Mode 2 Joueurs" , couleurNoir, couleurBlanche);//Phrase 2
    position.x = LARGEUR_FENETRE/2 - (texte->w / 2);
    position.y = LONGUEUR_FENETRE/2;
    SDL_BlitSurface(texte, NULL, ecran, &position);//Affichage du menu principal


    SDL_Flip(ecran);

    while(continuer)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
        case SDL_QUIT:
            exit(EXIT_SUCCESS);
            break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                continuer = 0;
                break;
            case SDLK_KP1://Mode 1 joueur
                niveauIA(ecran, police);
                break;
            case SDLK_KP2://Mode 2 joueurs
                retour = jouerDuel(ecran);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }


        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));

        texte = TTF_RenderText_Shaded(police,"1. Mode 1 Joueur" , couleurNoir, couleurBlanche);//Phrase 1
        position.x = LARGEUR_FENETRE/2 - (texte->w / 2);
        position.y = LONGUEUR_FENETRE/2 - (texte->h);
        SDL_BlitSurface(texte, NULL, ecran, &position);

        texte = TTF_RenderText_Shaded(police,"2. Mode 2 Joueurs" , couleurNoir, couleurBlanche);//Phrase 2
        position.x = LARGEUR_FENETRE/2 - (texte->w / 2);
        position.y = LONGUEUR_FENETRE/2;
        SDL_BlitSurface(texte, NULL, ecran, &position);//On actualise l'affichage lorsqu'on revient au menu principal


        SDL_Flip(ecran);
    }

    SDL_FreeSurface(ecran);
    SDL_FreeSurface(texte);

    TTF_CloseFont(police);
    TTF_Quit();
    return 0;
}

void lancement()//Fonction qui se lance au début du programme pour effectuer les réglages de bases
{
    srand(time(NULL));//Initialisation pour l'aléatoire
}
