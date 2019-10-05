#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "jouer.h"
#include "constantes.h"

int niveauIA(SDL_Surface *ecran, TTF_Font *police)
{
    int continuer = 1, niveau = 0;
    SDL_Surface *texte = NULL;
    SDL_Color couleurBlanche = {255, 255, 255}, couleurNoir = {0, 0, 0};
    SDL_Event event;
    SDL_Rect position;

    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));

    texte = TTF_RenderText_Shaded(police,"1. Niveau normal" , couleurNoir, couleurBlanche);//Phrase 1
    position.x = LARGEUR_FENETRE/2 - (texte->w / 2);
    position.y = LONGUEUR_FENETRE/2 - (texte->h);
    SDL_BlitSurface(texte, NULL, ecran, &position);

    texte = TTF_RenderText_Shaded(police,"2. Aléatoire !" , couleurNoir, couleurBlanche);//Phrase 2
    position.x = LARGEUR_FENETRE/2 - (texte->w / 2);
    position.y = LONGUEUR_FENETRE/2;
    SDL_BlitSurface(texte, NULL, ecran, &position);//Affichage du menu en mode 1 joueur

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
            case SDLK_KP1:
                niveau = 1;
                jouerCIA(ecran, niveau);//Niveau normal
                SDL_FreeSurface(texte);
                return 1;
                break;
            case SDLK_KP2:
                niveau = 0;
                jouerCIA(ecran, niveau);//Niveau idiot(niveau = 0)
                SDL_FreeSurface(texte);
                return 1;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
    SDL_FreeSurface(texte);
    return 1;
}

void jouerCIA(SDL_Surface *ecran, int niveau)
{
    int terrain[3][3] = {0};
    int continuer = 1, premierAJouer = 0, tourOrdinateur = 0, numeroJoueur = 0, numeroOrdinateur = 0, retour = 0, tour = 1;

    SDL_Event event;
    SDL_Surface *terrainMorpion = NULL, *croix, *croix_win = NULL, *cercle_win = NULL, *imagieMicrosoftWin = NULL, *imageAppleWin = NULL, *cercle = NULL, *screamer = NULL, *win = NULL, *egalite = NULL, *android = NULL;
    SDL_Rect position, positionSouris;
    SDL_Color couleur = {0, 0, 0};

    TTF_Font *police = NULL, *policeEgalite = NULL;
    police = TTF_OpenFont("data/Limousines.ttf", 50);
    policeEgalite = TTF_OpenFont("data/Limousines.ttf", 40);//Police plus petite

    screamer = IMG_Load("data/screamer.png");
    win = TTF_RenderText_Blended(police, "Vous avez gagné !", couleur);
    egalite = TTF_RenderText_Blended(policeEgalite, "Android aurait gagné ...", couleur);

    position.x = 0;
    position.y = 0;

    terrainMorpion = IMG_Load("data/terrain.jpg");
    android = IMG_Load("data/android.png");
    imageAppleWin = IMG_Load("data/apple_win.jpg");
    imagieMicrosoftWin = IMG_Load("data/microsoft_win.jpg");

    premierAJouer = (rand() % (1 - 0 + 1)) + 0;//Lancé de pieces

    if(premierAJouer)//l'utilisateur commence
    {
        numeroJoueur = 1;
        numeroOrdinateur = 2;
        cercle = IMG_Load("data/microsoft.png");//L'utilisateur est microsoft tout le temps
        croix = IMG_Load("data/apple.jpg");//L'ordinateur est apple
        cercle_win = IMG_Load("data/microsoft_win.jpg");
        croix_win = IMG_Load("data/apple_win.jpg");
    }
    else//L'ordinateur commence
    {
        numeroOrdinateur = 1;
        numeroJoueur = 2;
        cercle = IMG_Load("data/apple.jpg");
        croix = IMG_Load("data/microsoft.png");
        croix_win = IMG_Load("data/microsoft_win.jpg");
        cercle_win = IMG_Load("data/apple_win.jpg");
    }

    if(numeroOrdinateur == 1)//Si l'ordinateur commence, il joue un premier coup
    {
        if(!niveau)//Si il est idiot, il joue aléatoirement
            jouerAleatoirement(terrain, numeroOrdinateur);
        else
            premierCoup(terrain, numeroOrdinateur);
        tour++;
    }

    dessinerTerrain(ecran, terrainMorpion, cercle, croix, cercle_win, croix_win, terrain);
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
            default:
                break;
            }
            break;
        case SDL_MOUSEBUTTONUP://Clique de souris
            if(event.button.button == SDL_BUTTON_LEFT)
            {
                positionSouris.x = event.button.x;
                positionSouris.y = event.button.y;//On enregistre les coordonnées
                retour = placerElement(&positionSouris, terrain, numeroJoueur);//On vérifie si la case est libre, est si elle l'est, alors on la remplie
                if(retour == 1)//Si retour vaut 1, alors l'utilisateur à bien joué sont tour donc c'est au tour de l'ordinateur
                {
                    tourOrdinateur = 1;
                    tour++;

                    retour = etatPartie(terrain, premierAJouer);//Est-ce terminé ?
                }
            }
            break;
        default:
            break;
        }
        dessinerTerrain(ecran, terrainMorpion, cercle, croix, cercle_win, croix_win, terrain);
        SDL_Flip(ecran);//On actualise le terrain

        if(tourOrdinateur == 1 && retour == 0)//Si c'est au tour de l'ordinateur et si c'est pas terminé
        {
            SDL_Delay(500);//Après que l'utilisateur ai joué, une petite pause est crée pour pas que se soit trop brut
            tourBot(niveau, terrain, premierAJouer, tour);

            tourOrdinateur = 0;//Tour fini
            tour++;

            retour = etatPartie(terrain, premierAJouer);
        }
        dessinerTerrain(ecran, terrainMorpion, cercle, croix, cercle_win, croix_win, terrain);
        SDL_Flip(ecran);//Actualisation de l'affichage

        if(retour == 1 || retour == 2 || retour == 3)//Si la partie est terminée, on quitte la boucle
            continuer = 0;
    }

    if(retour)//Si gagné ou égalité
        SDL_Delay(300);//Petite pause avant de retourner dans le menu principal

    if(retour == 2)//l'ordinateur a gagné
    {
        position.x = 0;
        position.y = 0;
        SDL_BlitSurface(screamer, NULL, ecran, &position);//On affiche un piti screamer

        SDL_Flip(ecran);
        SDL_Delay(500);
    }
    else if(retour == 1)//Si c'est le joueur qui a gagné
    {
        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));
        position.x = LARGEUR_FENETRE/2 - win->w/2;
        position.y = LONGUEUR_FENETRE/2 - win->h/2;//On règle la position pour la phrase
        SDL_BlitSurface(win, NULL, ecran, &position);//Phrase

        position.x = LARGEUR_FENETRE/2 - imagieMicrosoftWin->w/2;
        position.y = LONGUEUR_FENETRE/2 + win->h/2 + 10;
        SDL_BlitSurface(imagieMicrosoftWin, NULL, ecran, &position);

        SDL_Flip(ecran);
        SDL_Delay(1000);
    }
    else if(retour == 3)//Si il y a égalité
    {
        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));
        position.x = LARGEUR_FENETRE/2 - egalite->w/2;
        position.y = LONGUEUR_FENETRE/2 - egalite->h/2;
        SDL_BlitSurface(egalite, NULL, ecran, &position);//Phrase

        position.x = LARGEUR_FENETRE/2 - android->w/2;
        position.y = LONGUEUR_FENETRE/2 + egalite->h/2 + 10;
        SDL_BlitSurface(android, NULL, ecran, &position);//Icon androidcercle

        SDL_Flip(ecran);
        SDL_Delay(1000);
    }

    SDL_FreeSurface(terrainMorpion);
    SDL_FreeSurface(cercle);
    SDL_FreeSurface(croix);
    SDL_FreeSurface(croix_win);
    SDL_FreeSurface(cercle_win);
    SDL_FreeSurface(imagieMicrosoftWin);
    SDL_FreeSurface(imageAppleWin);
    SDL_FreeSurface(egalite);
    SDL_FreeSurface(android);
    SDL_FreeSurface(screamer);
    SDL_FreeSurface(win);
    TTF_CloseFont(police);
    TTF_CloseFont(policeEgalite);
}

int jouerDuel(SDL_Surface *ecran)
{
    int terrain[3][3] = {0};
    int continuer = 1, joueur = 1, retour = 3;

    SDL_Event event;
    SDL_Surface *terrainMorpion = NULL, *croix = NULL, *cercle = NULL, *cercle_win = NULL, *croix_win = NULL, *joueur1 = NULL, *joueur2 = NULL, *joueur1_win = NULL, *joueur2_win = NULL;
    SDL_Surface *phraseEgalite = NULL, *imageEgalite = NULL;
    SDL_Rect position, positionSouris;
    SDL_Color couleur1 = {255, 0, 0}, couleur2 = {0, 255, 70}, noir = {0, 0, 0};


    TTF_Font *police = NULL, *police_win = NULL;
    police = TTF_OpenFont("data/Limousines.ttf", 20);
    police_win= TTF_OpenFont("data/Limousines.ttf", 40);//Police plus grande

    position.x = 0;
    position.y = 0;

    terrainMorpion = IMG_Load("data/terrain.jpg");
    croix = IMG_Load("data/croix.png");
    cercle = IMG_Load("data/cercle.png");
    croix_win = IMG_Load("data/croix_win.png");
    cercle_win = IMG_Load("data/cercle_win.png");
    imageEgalite = IMG_Load("data/forever_alone.jpg");

    joueur1 = TTF_RenderText_Blended(police,"Joueur 1" , noir);
    joueur2 = TTF_RenderText_Blended(police,"Joueur 2" , noir);
    joueur1_win = TTF_RenderText_Blended(police_win, "Joueur 1 gagne !", noir);
    joueur2_win = TTF_RenderText_Blended(police_win, "Joueur 2 gagne !", noir);
    phraseEgalite = TTF_RenderText_Blended(police_win, "Vous êtes mauvais", noir);

    position.x = LARGEUR_FENETRE - joueur1->w;
    position.y = 0;

    dessinerTerrain(ecran, terrainMorpion, cercle, croix, cercle_win, croix_win, terrain);
    SDL_BlitSurface(joueur1, NULL, ecran, &position);
    SDL_Flip(ecran);//Affichage du terrain

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
            default:
                break;
            }
            break;
            case SDL_MOUSEBUTTONUP:
                if(event.button.button == SDL_BUTTON_LEFT)
                {
                    positionSouris.x = event.button.x;
                    positionSouris.y = event.button.y;

                    retour = placerElement(&positionSouris, terrain, joueur%2);//Si le joueur a cliqué sur une case vide retour = 1 et la case est remplie
                    if(retour == 1)
                        joueur++;//tour au joueur suivant
                }
                break;
            default:
                break;
        }
        dessinerTerrain(ecran, terrainMorpion, cercle, croix, cercle_win, croix_win, terrain);
        if(joueur%2 == 1)//Affiche le numero du joueur qui doit jouer
            SDL_BlitSurface(joueur1, NULL, ecran, &position);
        else
            SDL_BlitSurface(joueur2, NULL, ecran, &position);
        SDL_Flip(ecran);//Actualise le terrain

        retour = etatPartie(terrain, 1);//gagné ou egalité ?
        if(retour)//Si la partie est terminée
        {
            continuer = 0;
            dessinerTerrain(ecran, terrainMorpion, cercle, croix, cercle_win, croix_win, terrain);
            SDL_Flip(ecran);//On actualise le terrain
            SDL_Delay(300);//Petite pause
        }


    }
    if(retour == 1)//Si le joueur 1 gagne
    {
        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));
        position.x = LARGEUR_FENETRE/2 - joueur1_win->w/2;
        position.y = LONGUEUR_FENETRE/2 - joueur1_win->h/2;
        SDL_BlitSurface(joueur1_win, NULL, ecran, &position);//Phrase joueur 1

        position.x = LARGEUR_FENETRE/2 - cercle_win->w/2 + 10;
        position.y = LONGUEUR_FENETRE/2 + joueur1_win->h/2 + 10;
        SDL_BlitSurface(cercle_win, NULL, ecran, &position);

        SDL_Flip(ecran);
        SDL_Delay(1000);
    }
    else if(retour == 2)//Si le joueur 2 gagne
    {
        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));
        position.x = LARGEUR_FENETRE/2 - joueur2_win->w/2;
        position.y = LONGUEUR_FENETRE/2 - joueur2_win->h/2;
        SDL_BlitSurface(joueur2_win, NULL, ecran, &position);//Phrase joueur 2

        position.x = LARGEUR_FENETRE/2 - croix_win->w/2 + 10;
        position.y = LONGUEUR_FENETRE/2 + joueur2_win->h/2 + 10;
        SDL_BlitSurface(croix_win, NULL, ecran, &position);

        SDL_Flip(ecran);
        SDL_Delay(1000);
    }
    else if(retour == 3)
    {
        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));
        position.x = LARGEUR_FENETRE/2 - phraseEgalite->w/2;
        position.y = LONGUEUR_FENETRE/2 - phraseEgalite->h/2 - 20;
        SDL_BlitSurface(phraseEgalite, NULL, ecran, &position);

        position.x = LARGEUR_FENETRE/2 - imageEgalite->w/2;
        position.y = LONGUEUR_FENETRE/2 + phraseEgalite->h/2 - 20;
        SDL_BlitSurface(imageEgalite, NULL, ecran, &position);

        SDL_Flip(ecran);
        SDL_Delay(1000);
    }

    SDL_FreeSurface(terrainMorpion);
    SDL_FreeSurface(croix);
    SDL_FreeSurface(croix_win);
    SDL_FreeSurface(joueur1);
    SDL_FreeSurface(joueur1_win);
    SDL_FreeSurface(joueur2);
    SDL_FreeSurface(joueur2_win);
    SDL_FreeSurface(cercle);
    SDL_FreeSurface(cercle_win);
    SDL_FreeSurface(phraseEgalite);
    SDL_FreeSurface(imageEgalite);
    TTF_CloseFont(police);
    TTF_CloseFont(police_win);

    return 1;
}

int placerElement(SDL_Rect *position, int terrain[][3], int joueur)//Teste les coordonnées du clique et vérifie si elles sont dans une case vide
{
    int x = 0, y = 0;

    if(position->x < 136)//Une case à un coté de 136 pixels
        x = 0;//Colonne numéro 0
    else if(position->x < 272)
        x = 1;//Colonne numéro 1
    else
        x = 2;//Colonne numéro 2

    if(position->y < 136)
        y = 0;//Ligne 0
    else if(position->y < 272)
        y = 1;//Ligne 1
    else
        y = 2;//Ligne 2

    if(terrain[x][y] != VIDE)// Pas vide ?
        return 0;//On quitte
    if(joueur == 1)//joueur%2 si c'est une partie duo pour savoir si c'est pair ou impair (impair = joueur 1 etc)
        terrain[x][y] = CERCLE;//Si joueur 1, on place la croix sinon on place le cercle
    else
        terrain[x][y] = CROIX;

    return 1;
}

int etatPartie(int terrain[][3], int premierAJouer)
{
    int numeroJoueur = 0, numeroOrdinateur = 0, x = 0, y = 0, pasVide = 0;

    if(premierAJouer)//Determine qui est premier
    {
        numeroJoueur = 1;
        numeroOrdinateur = 2;
    }
    else
    {
        numeroJoueur = 2;
        numeroOrdinateur = 1;
    }

    x = 1;
    for(y = 0; y < 3; y++)//Scrutement horizontal
    {
        if((terrain[x - 1][y] == terrain[x][y]) && (terrain[x][y] == terrain[x + 1][y]) && terrain[x][y] != VIDE)//Si les trois cases sont les mêmes et si elles ne sont pas vide
            {
                if(terrain[x][y] == numeroJoueur)//Appartiennent-elles au joueur ?
                {
                    terrain[x - 1][y] = numeroJoueur + 2;//On rajoute 2 à la case pour que ce soit le symbole "win" qui s'affiche
                    terrain[x][y] = numeroJoueur + 2;
                    terrain[x + 1][y] = numeroJoueur + 2;
                    return 1;//return 1 = utilisateur / joueur 1 gagnant
                }
                else if(terrain[x][y] == numeroOrdinateur)
                {
                    terrain[x - 1][y] = numeroOrdinateur + 2;
                    terrain[x][y] = numeroOrdinateur + 2;
                    terrain[x + 1][y] = numeroOrdinateur + 2;
                    return 2;//return 2 = ordinateur / joueur 2 gagne
                }
            }
    }

    y = 1;
    for(x = 0; x < 3; x++)//Scrutement vertical
    {
        if((terrain[x][y - 1] == terrain[x][y]) && (terrain[x][y] == terrain[x][y + 1]) && terrain[x][y] != VIDE)
        {
            if(terrain[x][y] == numeroJoueur)
            {
                terrain[x][y - 1] = numeroJoueur + 2;
                terrain[x][y] = numeroJoueur + 2;
                terrain[x][y + 1] = numeroJoueur + 2;
                return 1;
            }
            else if(terrain[x][y] == numeroOrdinateur)
            {
                terrain[x][y - 1] = numeroOrdinateur + 2;
                terrain[x][y] = numeroOrdinateur + 2;
                terrain[x][y + 1] = numeroOrdinateur + 2;
                return 2;
            }
        }
    }

    x = 1;
    y = 1;
    if((terrain[x - 1][y - 1] == terrain[x][y]) && (terrain[x][y] == terrain[x + 1][y + 1]) && terrain[x][y] != VIDE)//Scrutement diagonal
    {
        if(terrain[x][y] == numeroJoueur)
        {
            terrain[x - 1][y - 1] = numeroJoueur + 2;
            terrain[x][y] = numeroJoueur + 2;
            terrain[x + 1][y + 1] = numeroJoueur + 2;
            return 1;
        }
        else if(terrain[x][y] == numeroOrdinateur)
        {
            terrain[x - 1][y - 1] = numeroOrdinateur + 2;
            terrain[x][y] = numeroOrdinateur + 2;
            terrain[x + 1][y + 1] = numeroOrdinateur + 2;
            return 2;
        }
    }
    if((terrain[x + 1][y - 1] == terrain[x][y]) && (terrain[x][y] == terrain[x - 1][y + 1]) && (terrain[x][y] != VIDE))
    {
        if(terrain[x][y] == numeroJoueur)
        {
            terrain[x + 1][y - 1] = numeroJoueur + 2;
            terrain[x][y] = numeroJoueur + 2;
            terrain[x - 1][y + 1] = numeroJoueur + 2;
            return 1;
        }
        else if(terrain[x][y] == numeroOrdinateur)
        {
            terrain[x + 1][y - 1] = numeroOrdinateur + 2;
            terrain[x][y] = numeroOrdinateur + 2;
            terrain[x - 1][y + 1] = numeroOrdinateur + 2;
            return 2;
        }
    }
    for(x = 0; x < 3; x++)//Parcours toutes les cases du tableau et vérifie si l'une d'entre elle est vide
    {
        for(y = 0; y < 3; y++)
        {
            if(terrain[x][y] == VIDE)
                pasVide = 1;//Si une ou plus sont vide, alors pasVide = 1
        }
    }
    if(pasVide != 1)//Si pasVide est pas = 1 on quitte et on dit que y a égalité (return = 3)
        return 3;
    else
        return 0;
}

void dessinerTerrain(SDL_Surface *ecran, SDL_Surface *terrainMorpion, SDL_Surface *cercle, SDL_Surface *croix, SDL_Surface *cercle_win, SDL_Surface *croix_win, int terrain[][3])//Va afficher actualiser le terrain sur l'écran
{
    int x = 0, y = 0;

    SDL_Rect position;

    position.x = 0;
    position.y = 0;
    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));
    SDL_BlitSurface(terrainMorpion, NULL, ecran, &position);
    for(x = 0; x < 3; x++)
    {
        position.x = x * TAILLE_BLOC + 3;//On se décale d'un bloc + 10 pixels car les croix ne font pas 136 pixels
        for(y = 0; y < 3; y++)
        {
            position.y = y * TAILLE_BLOC + 3;

            switch(terrain[x][y])
            {
            case CERCLE://Place cercle
                SDL_BlitSurface(cercle, NULL, ecran, &position);
                break;
            case CROIX://Place croix
                SDL_BlitSurface(croix, NULL, ecran, &position);
                break;
            case VIDE:
                break;
            case CERCLE_WIN:
                SDL_BlitSurface(cercle_win, NULL, ecran, &position);
                break;
            case CROIX_WIN:
                SDL_BlitSurface(croix_win, NULL, ecran, &position);
                break;
            default:
                break;
            }
        }
    }
}

int tourBot(int niveau, int terrain[][3], int premierAJouer, int tour)
{
    int retour = 0, numeroJoueur = 0, numeroOrdinateur = 0;

    if(premierAJouer == 1)//determine les numéros des joueurs
    {
        numeroJoueur = 1;
        numeroOrdinateur = 2;
    }
    else
    {
        numeroJoueur = 2;
        numeroOrdinateur = 1;
    }

    if(!niveau)//Si l'ordinateur est idiot il jouera tout le temps au hasard
        jouerAleatoirement(terrain, numeroOrdinateur);
    else
    {
        retour = canWin(terrain, premierAJouer);//Il vérifie en premier si il peut gagner
        if(retour)//As-t'il joué ?
            return 1;
        retour = canLoose(terrain, premierAJouer);//Verifie ensuite si il peut défendre
        if(retour)//As-t'il joué ?
            return 1;
        if(tour <= 4)//Si aucun de ces cas n'es présent, il va faire des coups pré-programmer si on est au tour 4 ou inférieur
            jouerOrdinateur(terrain, numeroOrdinateur);
        else//Sinon il joue aléatoirement
            jouerAleatoirement(terrain, numeroOrdinateur);
        return 1;
    }

    return 0;
}

void jouerAleatoirement(int terrain[][3], int joueur)
{
    const int MIN = 0, MAX = 2;
    int x = 0, y = 0;

    do
    {
        x = (rand() % (MAX - MIN + 1)) + MIN;//Selectionne un x au hasard
        y = (rand() % (MAX - MIN + 1)) + MIN;//Selectionne un y au hasard
    }
    while(terrain[x][y] != VIDE);//Tant que la case n'est pas vide

    if(joueur == 1)
        terrain[x][y] = JOUEUR_1;//si l'ordinateur est 1er à jouer, on place la croix (CROIX = JOUEUR_1)
    else
        terrain[x][y] = JOUEUR_2;//si ordi == joueur 2, place un cercle (== JOUEUR_2)
}

void premierCoup(int terrain[][3], int numeroOrdinateur)
{
    const int MIN = 1, MAX = 4;
    int resultat = 0, continuer = 1;

    do
    {
    resultat = (rand() % (MAX - MIN + 1)) + MIN;
    switch(resultat)//Selectionne un angle au hasardn et le rempli si il est vide
    {
    case 1:
        if(terrain[0][0] == VIDE)
        {
            terrain[0][0] = numeroOrdinateur;
            continuer = 0;
        }
        break;
    case 2:
        if(terrain[2][0] == VIDE)
        {
            terrain[2][0] = numeroOrdinateur;
            continuer = 0;
        }
        break;
    case 3:
        if(terrain[0][2] == VIDE)
        {
            terrain[0][2] = numeroOrdinateur;
            continuer = 0;
        }

        break;
    case 4:
        if(terrain[2][2] == VIDE)
        {
            terrain[2][2] = numeroOrdinateur;
            continuer = 0;
        }
        break;
    default:
        break;
    }
    }while(continuer);
}

void jouerOrdinateur(int terrain[][3], int numeroOrdinateur)//Coups pré-programmer
{
    if(terrain[1][1] == VIDE)//Check en premier si il peut joué au milieu
        terrain[1][1] = numeroOrdinateur;
    else//Sinon il joue dans un angle
        premierCoup(terrain, numeroOrdinateur);
}

int canWin(int terrain[][3], int premierAJouer)
{
    int x = 0, y = 0, numeroOrdinateur = 0, compteurOrdinateur = 0, compteurVide = 0;

    if(premierAJouer == 1)
        numeroOrdinateur = 2;
    else
        numeroOrdinateur = 1;

    for(y = 0; y < 3; y++)//Scrutement horizontal
    {
        compteurOrdinateur = 0;
        compteurVide = 0;
        for(x = 0; x < 3; x++)//Parcours les cases horizontalement
        {
            switch(terrain[x][y])
            {
            case JOUEUR_1://Si c'est une case du joueur 1 et que l'ordinateur est le joueur 1
                if(numeroOrdinateur == 1)
                    compteurOrdinateur++;//compteurOrdinateur augmente de 1
                break;
            case JOUEUR_2:
                if(numeroOrdinateur == 2)
                    compteurOrdinateur++;
                break;
            case VIDE://Case vide, compteurVide augmente de 1
                compteurVide++;
                break;
            }
        }
        if(compteurOrdinateur == 2 && compteurVide == 1)//Pour qu'il puisse gagné il faut qu'il ai croisé 2 fois une case à lui et un case vide
        {
            for(x = 0; x < 3; x++)
                terrain[x][y] = numeroOrdinateur;//Il remplie ensuite toutes ces cases par son numéro(une seule change par rapport à avant)
            return 1;//l'ordinateur à jouer
        }
    }

    for(x = 0; x < 3; x++)//Scrutement vertical
    {
        compteurOrdinateur = 0;
        compteurVide = 0;
        for(y = 0; y < 3; y++)
        {
            switch(terrain[x][y])
            {
            case JOUEUR_1:
                if(numeroOrdinateur == 1)
                    compteurOrdinateur++;
                break;
            case JOUEUR_2:
                if(numeroOrdinateur == 2)
                    compteurOrdinateur++;
                break;
            case VIDE:
                compteurVide++;
                break;
            }
        }
        if(compteurOrdinateur == 2 && compteurVide == 1)
        {
            for(y = 0; y < 3; y++)
                terrain[x][y] = numeroOrdinateur;
            return 1;
        }
    }

    compteurOrdinateur = 0;
    compteurVide = 0;
    y = 0;
    for(x = 0; x < 3; x++)//Scrutement diagonal
    {
        switch(terrain[x][y])
        {
        case JOUEUR_1:
            if(numeroOrdinateur == 1)
                compteurOrdinateur++;
            break;
        case JOUEUR_2:
            if(numeroOrdinateur == 2)
                compteurOrdinateur++;
            break;
        case VIDE:
            compteurVide++;
            break;
        }
        y++;
    }
    if(compteurOrdinateur == 2 && compteurVide == 1)
    {
        y = 0;
        for(x = 0; x < 3; x++)
        {
            terrain[x][y] = numeroOrdinateur;
            y++;
        }

        return 1;
    }

    compteurOrdinateur = 0;
    compteurVide = 0;
    y = 2;

    for(x = 0; x < 3; x++)
    {
        switch(terrain[x][y])
        {
        case JOUEUR_1:
            if(numeroOrdinateur == 1)
                compteurOrdinateur++;
            break;
        case JOUEUR_2:
            if(numeroOrdinateur == 2)
                compteurOrdinateur++;
            break;
        case VIDE:
            compteurVide++;
            break;
        }
        y--;
    }
    if(compteurOrdinateur == 2 && compteurVide == 1)
    {
        y = 2;
        for(x = 0; x < 3; x++)
        {
            terrain[x][y] = numeroOrdinateur;
            y--;
        }
        return 1;
    }

    return 0;//return 0 si l'ordinateur n'a pas joué
}

int canLoose(int terrain[][3], int premierAJouer)//Même façon de procéder sauf que cette fois ci on cherche les cases du joueur
{
    int x = 0, y = 0, numeroJoueur = 0, numeroOrdinateur = 0, compteurJoueur = 0, compteurVide = 0;

    if(premierAJouer == 1)
        {
            numeroJoueur = 1;
            numeroOrdinateur = 2;
        }
    else
    {
        numeroJoueur = 2;
        numeroOrdinateur = 1;
    }
    for(y = 0; y < 3; y++)//Scrutement horizontal
    {
        compteurJoueur = 0;
        compteurVide = 0;
        for(x = 0; x < 3; x++)
        {
            switch(terrain[x][y])
            {
            case JOUEUR_1:
                if(numeroJoueur == 1)
                    compteurJoueur++;
                break;
            case JOUEUR_2:
                if(numeroJoueur == 2)
                    compteurJoueur++;
                break;
            case VIDE:
                compteurVide++;
                break;
            }
        }
        if(compteurJoueur == 2 && compteurVide == 1)
        {
            for(x = 0; x < 3; x++)
                {
                    if(terrain[x][y] == VIDE)//rempli pas toutes les cases mais uniquement la vide
                        terrain[x][y] = numeroOrdinateur;
                }
            return 1;
        }
    }

    for(x = 0; x < 3; x++)//Scrutement vertical
    {
        compteurJoueur = 0;
        compteurVide = 0;
        for(y = 0; y < 3; y++)
        {
            switch(terrain[x][y])
            {
            case JOUEUR_1:
                if(numeroJoueur == 1)
                    compteurJoueur++;
                break;
            case JOUEUR_2:
                if(numeroJoueur == 2)
                    compteurJoueur++;
                break;
            case VIDE:
                compteurVide++;
                break;
            }
        }
        if(compteurJoueur == 2 && compteurVide == 1)
        {
            for(y = 0; y < 3; y++)
                {
                    if(terrain[x][y] == VIDE)
                        terrain[x][y] = numeroOrdinateur;
                }
            return 1;
        }
    }

    compteurJoueur = 0;
    compteurVide = 0;
    y = 0;
    for(x = 0; x < 3; x++)//Scrutement diagonal
    {
        switch(terrain[x][y])
        {
        case JOUEUR_1:
            if(numeroJoueur == 1)
                compteurJoueur++;
            break;
        case JOUEUR_2:
            if(numeroJoueur == 2)
                compteurJoueur++;
            break;
        case VIDE:
            compteurVide++;
            break;
        }
        y++;
    }
    if(compteurJoueur == 2 && compteurVide == 1)
    {
        y = 0;
        for(x = 0; x < 3; x++)
        {
            if(terrain[x][y] == VIDE)
                terrain[x][y] = numeroOrdinateur;
            y++;
        }

        return 1;
    }

    compteurJoueur = 0;
    compteurVide = 0;
    y = 2;

    for(x = 0; x < 3; x++)
    {
        switch(terrain[x][y])
        {
        case JOUEUR_1:
            if(numeroJoueur == 1)
                compteurJoueur++;
            break;
        case JOUEUR_2:
            if(numeroJoueur == 2)
                compteurJoueur++;
            break;
        case VIDE:
            compteurVide++;
            break;
        }
        y--;
    }
    if(compteurJoueur == 2 && compteurVide == 1)
    {
        y = 2;
        for(x = 0; x < 3; x++)
        {
            if(terrain[x][y] == VIDE)
                terrain[x][y] = numeroOrdinateur;
            y--;
        }
        return 1;
    }

    return 0;
}
