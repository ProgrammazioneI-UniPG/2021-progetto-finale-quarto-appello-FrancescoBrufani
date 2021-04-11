#include "gamelib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
static struct Giocatore *giocatori[10];
static unsigned short quest_da_finire;
static struct Stanza *stanza_inizio;
static struct Stanza **lista_stanze;
static int limStanze;
static int turno;
static int identificatore = 1;
static void stampa_giocatori();
static void inizia_gioco() {}
static int numeroGiocatori;
static bool giocoTerminato = false;
static void avanza(struct Giocatore *g);
static bool esegui_quest(struct Stanza *StanzaAttuale);
static void chiamata_emergenza(struct Stanza *StanzaAttuale);
static bool uccidi_astronauta(struct Stanza *StanzaAttuale,
                              struct Giocatore *killer);
static void usa_botola(struct Stanza *nodoA, struct Giocatore *nome);
static void sabotaggio(struct Stanza *StanzaImpostore);
static const char nomi[10][10] = {"blu",     "rosso",     "verde", "giallo",
                                  "marrone", "arancione", "ocra",  "nero",
                                  "bianco",  "viola"};
static const char stato[4][13] = {"astronauta", "impostore", "assassinato",
                                  "defenestrato"};
static const char stringaStanza[4][17] = {"niente", "quest_semplice",
                                          "quest_complicata", "botola"};
static void aggiungiLista(struct Stanza *nuova, struct Stanza ***lista,
                          int *limite, int i) {
  if (i == *limite) {
    *limite *= 2;
    struct Stanza **tmp = realloc(*lista, *limite * sizeof(struct Stanza *));
    if (tmp == NULL) {
      exit(2);
    }
    *lista = tmp;
  }
  (*lista)[i] = nuova;
}
static struct Stanza *creaStanza() {
  struct Stanza *miaStanza;
  miaStanza = malloc(sizeof(struct Stanza));
  if (miaStanza == NULL) {
    exit(2);
  }
  miaStanza->id = identificatore;
  int random = rand() % 100;
  miaStanza->avanti = NULL;
  miaStanza->destra = NULL;
  miaStanza->sinistra = NULL;
  miaStanza->Stanza_precedente = NULL;
  if (random < 25) {
    miaStanza->tipo = botola;
  } else if (random < 40) {
    miaStanza->tipo = quest_complicata;
  } else if (random < 70) {
    miaStanza->tipo = quest_semplice;
  } else {
    miaStanza->tipo = vuota;
  }
  aggiungiLista(miaStanza, &lista_stanze, &limStanze, identificatore);
  identificatore++;
  return miaStanza;
}
extern void imposta_gioco() {
  if (*giocatori == NULL) {
    inputLimitato("Inserire il numero di giocatori (tra 4 e 10)",
                  &numeroGiocatori, 4, 10);
    int percentuale;
    inputLimitato(
        "Inserisci la probabilita'di essere impostore :", &percentuale, 0, 100);
    int incremento;
    inputLimitato(
        "Inserisci l'incremento di probabilita'all'aumentare dei giocatori :",
        &incremento, 0, 100);
    *giocatori = malloc(numeroGiocatori * sizeof(struct Giocatore));
    if (*giocatori == NULL) {
      exit(2);
    }
    for (int i = 1; i < numeroGiocatori; i++) {
      giocatori[i] = *giocatori + i;
    }
    int quests;
    inputLimitato(
        "Inserisci il numero di quest necessarie per vincere la partita:",
        &quests, 2, 100);
    quest_da_finire = quests;
    lista_stanze = malloc(sizeof(struct Stanza *));
    if (lista_stanze == NULL) {
      exit(2);
    }
    limStanze = 1;
    stanza_inizio = creaStanza();
    int impostori = 3;
    int random;
    for (int i = 0; i < numeroGiocatori; i++) {
      giocatori[i]->StanzaAttuale = stanza_inizio;
      random = rand() % 100;
      if (random <= percentuale) {
        giocatori[i]->tipo = impostore;
        impostori--;
      } else {
        giocatori[i]->tipo = astronauta;
      }
      if (impostori < 0) {
        giocatori[i]->tipo = astronauta;
      }
      percentuale += incremento;
    }
    if (impostori == 3) {
      random = rand() % numeroGiocatori;
      giocatori[random]->tipo = impostore;
    }
    int trasponi = 0;
    if (numeroGiocatori < 10) {
      trasponi = rand() % (10 - numeroGiocatori);
    }
    for (int i = 0; i < numeroGiocatori; i++) {
      giocatori[i]->nome = (enum Nome_giocatore)(i + trasponi);
    }
    enum Nome_giocatore t;
    for (int i = 0; i < numeroGiocatori; i++) {
      random = rand() % numeroGiocatori;
      t = giocatori[random]->nome;
      giocatori[random]->nome = giocatori[i]->nome;
      giocatori[i]->nome = t;
    }
    int scelta;
    do {
      scelta = -1;
      inputLimitato("Premere 1 per stampare i giocatori, Premere 2 per tornare "
                    "al menu'principale",
                    &scelta, 1, 2);
      switch (scelta) {
      case 1:
        stampa_giocatori();
        break;
      case 2:
        inizia_gioco();
      }
    } while (scelta != 2);
  } else if (giocoTerminato == false) {
    puts("Il gioco e'gia'inizializzato");
  } else {
    puts("Devi prima chiamare termina_gioco premendo 3!!");
  }
}
static void stampa_giocatori() {
  for (int i = 0; i < numeroGiocatori; i++) {
    printf("Numero giocatore:%d\n nome:%s\n lo stato del giocatore e':%s\n",
           i + 1, nomi[giocatori[i]->nome], stato[giocatori[i]->tipo]);
  }
}
extern void gioca() {
  if (*giocatori == NULL) {
    puts("Non hai inizializzato la partita!\n");
    return;
  }
  if (giocoTerminato == true) {
    puts("Non hai chiamato termina_gioco premendo 3...");
    return;
  }
  int scelta = 0;
  turno = 0;
  struct Giocatore *attuale;
  while (true) {
    attuale = giocatori[turno];
    if (attuale->tipo == astronauta || attuale->tipo == impostore) {
      printf("giocatore: %s\n lo stato del giocatore e': %s\n",
             nomi[attuale->nome], stato[attuale->tipo]);
      printf("Si trova nella stanza: %d \n Che ha : %s\n",
             attuale->StanzaAttuale->id,
             stringaStanza[attuale->StanzaAttuale->tipo]);
      if (attuale->tipo == astronauta) {
        inputLimitato("Premere 1 per avanzare\n Premere 2 per eseguire la "
                      "quest \nPremere 3 per la chiamata di emergenza:",
                      &scelta, 1, 3);
        switch (scelta) {
        case 1:
          avanza(attuale);
          break;
        case 2:
          if (esegui_quest(attuale->StanzaAttuale)) {
            return;
          }
          break;
        case 3:
          chiamata_emergenza(attuale->StanzaAttuale);
        }
      } else {
        inputLimitato(
            "\n Premere 1 per avanzare \n Premere 2 la chiamata di emergenza "
            "\n Premere 3 per uccidere un astronauta \n Premere 4 per usare "
            "la botola \n Premere 5 per un sabotaggio",
            &scelta, 1, 5);
        switch (scelta) {
        case 1:
          avanza(attuale);
          break;
        case 2:
          chiamata_emergenza(attuale->StanzaAttuale);
          break;
        case 3:
          if (uccidi_astronauta(attuale->StanzaAttuale, attuale)) {
            return;
          }
          break;
        case 4:
          usa_botola(attuale->StanzaAttuale, attuale);
          break;
        case 5:
          sabotaggio(attuale->StanzaAttuale);
        }
      }
    } else if (attuale->tipo == defenestrato) {
      printf("Il giocatore %s sta facendo un viaggio nello spazio infinito\n",
             nomi[attuale->nome]);
    } else {
      printf("Il giocatore %s e'stato brutalmente assassinato\n",
             nomi[attuale->nome]);
    }
    turno = (turno + 1) % numeroGiocatori;
  }
}
static void avanza(struct Giocatore *g) {
  puts("In quale direzione vuoi andare?");
  int scelta = 0;
  struct Stanza *precedente;
  struct Stanza *corrente;
  struct Stanza **stanze[3] = {&(g->StanzaAttuale->destra),
                               &(g->StanzaAttuale->sinistra),
                               &(g->StanzaAttuale->avanti)};
  precedente = g->StanzaAttuale;
  inputLimitato("premere 1 per andare a destra,2 per andare a sinistra e 3 "
                "per andare avanti:",
                &scelta, 1, 3);
  scelta--;
  corrente = *stanze[scelta];
  if (corrente == NULL) {
    corrente = creaStanza();
    corrente->Stanza_precedente = precedente;
    *stanze[scelta] = corrente;
    puts("Sei in una nuova stanza!\n ");
  } else {
    puts("Sei in una vecchia stanza! \n");
  }
  g->StanzaAttuale = corrente;
  printf("Ti sei spostato nella stanza: %d Che ha: %s\n", corrente->id,
         stringaStanza[corrente->tipo]);
}
static bool esegui_quest(struct Stanza *StanzaAttuale) {
  if (StanzaAttuale->tipo == vuota || StanzaAttuale->tipo == botola) {
    puts("Non ci sono quest da eseguire!");
    turno = (numeroGiocatori + turno - 1) % numeroGiocatori;
  } else if (StanzaAttuale->tipo == quest_semplice) {
    quest_da_finire--;
    StanzaAttuale->tipo = vuota;
    printf("Hai eseguito una quest semplice, ne rimangono %hu \n",
           quest_da_finire);
  } else {
    quest_da_finire -= 2;
    StanzaAttuale->tipo = vuota;
    if (quest_da_finire == 65535) {
      quest_da_finire = 0;
    }
    printf("Hai eseguito una quest complicata,ne rimangono %hu \n",
           quest_da_finire);
  }
  if (quest_da_finire == 0) {
    puts("Hanno vinto gli astronauti!! \n");
    giocoTerminato = true;
    return true;
  }
  return false;
}
static void defenestra(struct Stanza *StanzaAttuale) {
  int presenti = 0;
  struct Giocatore *array[10];
  for (int i = 0; i < numeroGiocatori; i++) {
    if (giocatori[i]->StanzaAttuale == StanzaAttuale &&
        (giocatori[i]->tipo == astronauta || giocatori[i]->tipo == impostore)) {
      array[presenti] = giocatori[i];
      presenti++;
    }
  }
  for (int i = 0; i < presenti; i++) {
    int random = rand() % 100;
    int prob = 30;
    if (array[i]->tipo == impostore) {
      for (int j = 0; j < presenti; j++) {
        if (i != j) {
          if (array[j]->tipo == impostore) {
            prob -= 30;
          } else if (array[j]->tipo == astronauta) {
            prob += 20;
          }
        }
      }
    } else if (array[i]->tipo == astronauta) {
      for (int j = 0; j < presenti; j++) {
        if (i != j) {
          if (array[j]->tipo == impostore) {
            prob += 20;
          } else if (array[j]->tipo == astronauta) {
            prob -= 30;
          }
        }
      }
    }
    if (random < prob) {
      array[i]->tipo = defenestrato;
      array[i]->StanzaAttuale = NULL;
      printf("E'stato defenestrato %s\n", nomi[array[i]->nome]);
      break;
    }
  }
}
static void chiamata_emergenza(struct Stanza *StanzaAttuale) {
  if (StanzaAttuale->emergenza_chiamata == false) {
    struct Giocatore *morti[10];
    int k = 0;
    for (int i = 0; i < numeroGiocatori; i++) {
      if (giocatori[i]->tipo == assassinato &&
          giocatori[i]->StanzaAttuale == StanzaAttuale) {
        morti[k] = giocatori[i];
        k++;
      }
    }
    if (k > 0) {
      defenestra(StanzaAttuale);
      for (int i = 0; i < k; i++) {
        morti[i]->StanzaAttuale = NULL;
      }
    } else {
      puts("Non c'e'alcun giocatore morto nella stanza!\n");
    }
    StanzaAttuale->emergenza_chiamata = true;
  } else {
    puts("E'gia'stata eseguita una chiamata d'emergenza !\n");
    turno = (numeroGiocatori + turno - 1) % numeroGiocatori;
  }
}
static void defenestraKiller(struct Giocatore *killer,
                             struct Giocatore **presentiP, int numeroP) {
  int presenti = 0;
  struct Giocatore *array[10];
  for (int i = 0; i < numeroGiocatori; i++) {
    if (giocatori[i]->StanzaAttuale ==
        killer->StanzaAttuale->Stanza_precedente) {
      array[presenti] = giocatori[i];
      presenti++;
    }
  }
  int prob = 0;
  int random = rand() % 100;
  for (int i = 0; i < numeroP; i++) {
    if (presentiP[i]->tipo == astronauta) {
      prob += 50;
    }
  }
  for (int i = 0; i < presenti; i++) {
    if (array[i]->tipo == astronauta) {
      prob += 20;
    }
  }
  if (random < prob) {
    killer->tipo = defenestrato;
    killer->StanzaAttuale = NULL;
    printf("Il giocatore: %s e'stato defenestrato!\n", nomi[killer->nome]);
  }
}
static bool uccidi_astronauta(struct Stanza *corrente,
                              struct Giocatore *killer) {
  int presenti = 0;
  struct Giocatore *array[10];
  for (int i = 0; i < numeroGiocatori; i++) {
    if (giocatori[i]->StanzaAttuale == corrente &&
        giocatori[i]->tipo == astronauta) {
      array[presenti] = giocatori[i];
      presenti++;
    }
  }
  if (presenti > 0) {
    int random = rand() % presenti;
    array[random]->tipo = assassinato;
    printf("E'stato assassinato %s\n", nomi[array[random]->nome]);
    presenti = 0;
    for (int i = 0; i < numeroGiocatori; i++) {
      if (giocatori[i]->StanzaAttuale == corrente) {
        array[presenti] = giocatori[i];
        presenti++;
      }
    }
    defenestraKiller(killer, array, presenti);
    int check = 0;
    for (int i = 0; i < numeroGiocatori; i++) {
      if (giocatori[i]->tipo == astronauta) {
        check++;
        break;
      }
    }
    if (check == 0) {
      puts("Hanno vinto gli impostori!");
      giocoTerminato = true;
      return true;
    }
  } else {
    puts("Non c'e'alcun astronauta nella stanza!\n");
    turno = (numeroGiocatori + turno - 1) % numeroGiocatori;
  }
  return false;
}
static void sabotaggio(struct Stanza *StanzaImpostore) {
  if (StanzaImpostore->tipo == quest_semplice ||
      StanzaImpostore->tipo == quest_complicata) {
    StanzaImpostore->tipo = vuota;
    puts("La quest della stanza e'stata sabotata \n");
  } else {
    puts("Non c'e'niente da sabotare ! \n");
    turno = (numeroGiocatori + turno - 1) % numeroGiocatori;
  }
}
static struct Stanza *ricercaBotola(struct Stanza *corrente) {
  struct Stanza *ris = NULL;
  struct Stanza **botole = malloc(sizeof(struct Stanza *));
  int trovate = 0;
  int dim = 1;
  for (int i = 0; i < identificatore; i++) {
    if (lista_stanze[i] != corrente) {
      if (lista_stanze[i]->tipo == botola) {
        aggiungiLista(lista_stanze[i], &botole, &dim, trovate);
        trovate++;
      }
    }
  }
  if (trovate > 0) {
    int random = rand() % trovate;
    ris = botole[random];
  }
  free(botole);
  return ris;
}
static struct Stanza *stanzaRandomica(struct Stanza *stanzaR) {
  struct Stanza *ris = NULL;
  if (identificatore > 1) {
    do {
      int random = rand() % identificatore;
      ris = lista_stanze[random];
    } while (ris != stanzaR);
  } else {
    ris = stanzaR;
  }
  return ris;
}
static void usa_botola(struct Stanza *nodoA, struct Giocatore *nome) {
  if (nodoA->tipo == botola) {
    struct Stanza *dest = ricercaBotola(nodoA);
    if (dest == NULL) {
      dest = stanzaRandomica(nodoA);
    }
    nome->StanzaAttuale = dest;
    printf("Hai usato una botola e sei andato in %hu \n", dest->id);
  } else {
    puts("Non c'e'una botola in questa stanza! \n");
    turno = (numeroGiocatori + turno - 1) % numeroGiocatori;
  }
}
extern void termina_gioco() {
  if (giocoTerminato == true) {
    giocoTerminato = false;
    for (int i = 0; i < identificatore; i++) {
      free(lista_stanze[i]);
    }
    stanza_inizio = NULL;
    free(lista_stanze);
    lista_stanze = NULL;
    free(*giocatori);
    for (int i = 0; i < numeroGiocatori; i++) {
      giocatori[i] = NULL;
    }
    numeroGiocatori = 0;
    quest_da_finire = 0;
    limStanze = 0;
    turno = 0;
    identificatore = 1;
  } else if (*giocatori == NULL) {
    puts("Il gioco non e'ancora stato inizializzato!");
  } else {
    puts("Il gioco non e'ancora stato giocato!");
  }
}

extern void inputLimitato(const char *msg, int *input, int min, int max) {
  static int tentativi = 0;
  int num;
  do {
    puts(msg);
    while (scanf("%d", input) != 1) {
      puts("\nIn questo programma sono consentiti SOLO input formati da un "
           "singolo int...");
      int b;
      do {
        b = getc(stdin);
        if (b == EOF) {
          puts(
              "\nRaggiunto un EOF nello stdin, quindi termino il programma...");
          exit(1);
        }
      } while (b != '\n');
      tentativi++;
      if (tentativi > 10) {
        puts("\nHai fatto piu'di 10 input errati, termino il programma...");
        exit(3);
      }
    }
    tentativi = 0;
    num = *input;
    if (num < min || num > max) {
      printf("Non sono ammessi int piu'grandi di %d o minori di %d\n", max,
             min);
    }
  } while (num < min || num > max);
}
