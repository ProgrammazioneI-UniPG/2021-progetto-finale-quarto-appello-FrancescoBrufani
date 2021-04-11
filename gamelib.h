#ifndef gamelib_
#define gamelib_
#include <stdbool.h>
extern void imposta_gioco();
extern void gioca();
extern void termina_gioco();
extern void inputLimitato(const char *msg, int *input, int min, int max);
enum Stato_giocatore { astronauta, impostore, assassinato, defenestrato };
enum Nome_giocatore {
  blu,
  rosso,
  verde,
  giallo,
  marrone,
  arancione,
  ocra,
  nero,
  bianco,
  viola
};
enum Tipo_stanza { vuota, quest_semplice, quest_complicata, botola };
struct Stanza {
  struct Stanza *avanti;
  struct Stanza *destra;
  struct Stanza *sinistra;
  struct Stanza *Stanza_precedente;
  enum Tipo_stanza tipo;
  bool emergenza_chiamata;
  int id;
};
struct Giocatore {
  enum Nome_giocatore nome;
  enum Stato_giocatore tipo;
  struct Stanza *StanzaAttuale;
};
#endif
