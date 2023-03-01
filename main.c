//Tietorakenteet ja algoritmit harjoitustyo, C
//Iiso Kramsu

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

unsigned long int laskuri = 0;

typedef int (*vertailufunktio)(const void*, const void*);

// Struct-rakenteet

typedef struct lista {
    char *string;
    unsigned long int arvo;
    struct lista *seuraava;
} lista;

typedef struct hashtaulu {
    unsigned long int koko;
    lista **taulu;
} hashtaulu;

typedef struct yleisin {
    char string[100];
    unsigned long int arvo;
} yleisin;

// Funktiot

hashtaulu *luoHashtaulu(unsigned long int);                // Hashtaulun luonti
int hash(hashtaulu *hashtaulu, char *sana);                // Hashfunktio
lista *etsi_string(hashtaulu *hashtaulu, char *str);       // Etsii sanalle paikan, jos on jo varattu otetaan seuraava
int lisaa_string(hashtaulu *hashtaulu, char *str);         // Sanan tauluunlis�ysfunktio
int vertaa(yleisin *e1, yleisin *e2);                      // Qsort-funktiolle sanojen vertailufunktio
void tyhjennaTaulu(hashtaulu *hashtaulu);                  // Hash-taulun tyhjent�j�

// Main, miss� tiedoston kysely, avaus, funktioiden kutsut,
// sadan yleisimm�n sanan tulostus sek� tiedoston sulkeminen

int main(int argc, char** argv) {

    char tiedostonimi[100];
    char sana[100];
    char c;
    int sanat = 1;

    FILE *tiedosto;

// Kysyt��n tiedoston nime� k�ytt�j�lt�
// Lasketaan tiedostosta sanojen m��r�

    if (argc == 2) {
        tiedosto = fopen(argv[1], "r+");
    }
    else {
        do {
            printf("Syota tiedoston nimi ilman .txt-paatetta: \n");
            scanf("%s", tiedostonimi);
            strcat(tiedostonimi, ".txt");
            tiedosto = fopen(tiedostonimi, "r+");
        } while (tiedosto == 0);
    }
    while((c = fgetc(tiedosto)) != EOF) {
        if(c == ' ')
            sanat++;
    }

// Tulostetaan sanojen lukum��r�, t�t� m��r�� k�ytet��n my�s hash-arvojen luonnissa
// Luodaan samalla hashtaulu funktion kautta, kooksi tulee sanojen m��r�

    printf("Sanojen lukumaara = %d\n", sanat);
    rewind(tiedosto);

    laskuri = sanat;
    hashtaulu *sanataulu;
    sanataulu = luoHashtaulu(laskuri);

// K�yd��n l�pi sanat tiedostosta ja lis�t��n hash-tauluun
// Teht�v�n vaatimukset: luetaan vain aakkoset ja heittomerkki

    while (!feof(tiedosto)) {
        fscanf(tiedosto, "%s", sana);
        char tempSana[100];

        int i = 0;
        int c = 0;
        for (i = 0; i < strlen(sana); i++) {
          if ((isalpha(sana[i])) || (sana[i] == '\'' || sana[i] == 39)) {
             tempSana[c] = sana[i];
             c++;
        }
       }
        tempSana[c] = '\0';
        lisaa_string(sanataulu, tempSana);
    }

// Lasketaan taulusta sata yleisint� sanaa ja tulostetaan ne
// K�ytetty apuna qsort-funktiota

    yleisin yleisin[99];
    lista *lista;
    int loydetytSanat =0;

    for (int i = 0; i<laskuri; i++) {
        for (lista = sanataulu->taulu[i]; lista != NULL; lista = lista->seuraava) {

        if (loydetytSanat<100) {
          strcpy(yleisin[loydetytSanat].string, lista->string);
          yleisin[loydetytSanat].arvo= lista->arvo;
          loydetytSanat = loydetytSanat + 1;
          qsort((void *) &yleisin, 100, sizeof(struct yleisin), (vertailufunktio)vertaa);
        }

        else if (lista->arvo > yleisin[0].arvo) {
          strcpy(yleisin[0].string, lista->string);
          yleisin[0].arvo = lista->arvo;
          qsort((void *) &yleisin, 100, sizeof(struct yleisin), (vertailufunktio)vertaa);
        }
      }
    }

    for(int i =99; i>0; i--) {
        printf("%s %lu \n", yleisin[i].string, yleisin[i].arvo);
    }

// Tyhjennet��n hash-taulu ja suljetaan tiedosto

    tyhjennaTaulu(sanataulu);
    int fclose(FILE *tiedosto);

    return 0;
}

// Hash-taulun luomiseen k�ytett�v� funktio
// K�ytet��n apuna malloc-funktiota joka luo muistipaikan hash-taululle

hashtaulu *luoHashtaulu(unsigned long int koko) {

    hashtaulu *uusitaulu;
    int i;
    if (koko < 1) return NULL;
    if ((uusitaulu = malloc(sizeof(hashtaulu))) == NULL) {
    return NULL;
    }

    if ((uusitaulu->taulu = malloc(sizeof(lista*) * koko)) == NULL) {
    return NULL;
    }

    for(i=0; i < koko; i++) uusitaulu->taulu[i] = NULL;
    uusitaulu->koko = koko;
    return uusitaulu;
}

// Hash-funktio, jonka tarkoituksena luoda sanalle oma hash arvo.
// Korotetaan seuraavaa kirjaimen arvoa aina yht� suuremmalla potenssilla.
// V�ltet��n t�rm�ykset
// Muutetaan jokainen sana pieniksi kirjaimiksi.

int hash(hashtaulu *hashtaulu, char *sana) {

    int i;
    int j;
    int summa = 0;

    for (i = 0; i < strlen(sana); i++) {
      sana[i] = tolower(sana[i]);

      int temp = 1;

      for (j = 0; j < i; j++) {
        temp = temp * 64;
    }

    summa = summa + sana[i] * temp;
    }

    int tulos = 0;
    tulos = summa;
    summa = 0;

    return tulos % hashtaulu->koko;
}

// Paikan haku hash taulukosta sanalle

lista *etsi_string(hashtaulu *hashtaulu, char *str) {

    lista *lista;
    int hasharvo = hash(hashtaulu, str);

    for (lista = hashtaulu->taulu[hasharvo]; lista != NULL; lista = lista->seuraava) {
        if (strcmp(str, lista->string) == 0) return lista;
    }

    return NULL;
}

// Sanan lis�ys hash taulukkoon

int lisaa_string(hashtaulu *hashtaulu, char *str) {
    lista *uusilista;
    lista *nykyinenlista;
    int hasharvo = hash(hashtaulu, str);

    if ((uusilista = malloc(sizeof(lista))) == NULL) return 1;

    nykyinenlista = etsi_string(hashtaulu, str);

    if (nykyinenlista != NULL) {
        nykyinenlista->arvo = nykyinenlista->arvo+1;
        return 0;
    };

    uusilista->string = strdup(str);
    uusilista->arvo = 1;
    uusilista->seuraava = hashtaulu->taulu[hasharvo];
    hashtaulu->taulu[hasharvo] = uusilista;

    return 0;
}

// Vertailufunktio qsort-funktiota varten

int vertaa(yleisin *e1, yleisin *e2) {
    if (e1->arvo < e2->arvo)
        return -1;
    else if (e1->arvo > e2->arvo)
        return 1;
    else
        return 0;
}

//tyhjennysfunktio hash taulun tyhjent�miseen

void tyhjennaTaulu(hashtaulu *hashtaulu) {
    int i;
    lista *lista, *temp;
        if (hashtaulu==NULL) return;
        for (i=0; i < hashtaulu->koko; i++) {
            lista = hashtaulu->taulu[i];

            while (lista != NULL) {
             temp = lista;
             lista = lista->seuraava;
             free(temp->string);
             free(temp);
            }

        }
        free(hashtaulu->taulu);
        free(hashtaulu);
}
