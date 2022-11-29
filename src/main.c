#include  "geracodigo.h"
#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
int main(int argc, char *argv[]){
    FILE *myfp;
    unsigned char codigo[100];
    funcp funcaoSB;
    int res;

    /* Abre o arquivo fonte */
    if ((myfp = fopen("teste2.txt", "r")) == NULL) {
      perror("Falha na abertura do arquivo fonte");
      exit(1);
    }
    /* compila a função SB */
    funcaoSB = geraCodigo(myfp, codigo);

    for(int i = 0; i <100; i++){
      printf("%02X ", codigo[i]);
    }
    fclose(myfp);

    /* chama a função */
    res = (*funcaoSB)(1, 3);  /* passando parâmetro apropriados */

    printf("%d", res);
    return 0;
    }