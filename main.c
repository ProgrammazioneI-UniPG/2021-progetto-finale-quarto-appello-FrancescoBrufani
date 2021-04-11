#include "gamelib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main() {
  srand(time(NULL));
  int scelta;
  do {
    scelta = -1;
    inputLimitato(
        "Premere 1 per  impostare il gioco\nOppure 2 per giocare\n3 invece per "
        "terminare il gioco (dealloca strutture)\ne 4 per uscire",
        &scelta, 1, 4);
    switch (scelta) {
    case 1:
      imposta_gioco();
      break;
    case 2:
      gioca();
      break;
    case 3:
      termina_gioco();
    }
  } while (scelta != 4);
  return 0;
}
