#include <stdio.h>
#include <stdlib.h>
/*-------------------------------------------------------------------------*/
#define MAX_LINHAS 20



/*-------------------------------------------------------------------------*/
typedef int (*funcp) ();


/*-------------------------------------------------------------------------*/
funcp geraCodigo (FILE *arquivo_entrada, unsigned char codigo[]);
static void error (const char *msg, int line);

/*-------------------------------------------------------------------------*/
funcp geraCodigo (FILE *arquivo_entrada, unsigned char codigo[]){
    int contaLinha = 1; //variavel que vai sendo incrementada a medida q as linhas vao sendo lidas, p em caso de erro, retornar em ql linha foi
    int c;

      while ((c = fgetc(arquivo_entrada)) != EOF) {
        switch (c) {
            case 'r': { /* retorno */
                char var0;
                int idx0;
                if (fscanf(arquivo_entrada, "et %c%d", &var0, &idx0) != 2)
                    error("comando invalido", contaLinha);
                printf("%d ret %c%d\n", contaLinha, var0, idx0);
                break;
            }
            case 'v':
            case 'p': { /*atribuicao operacao aritimetica*/
                char var0 = c, var1, op;
                int idx0, idx1;

                if (fscanf(arquivo_entrada, "%d %c= %c%d", &idx0, &op, &var1, &idx1) != 4)
                    error("comando invalido", contaLinha);

                    
                printf("%d %c%d %c= %c%d\n", contaLinha, var0, idx0, op, var1, idx1);
                break;
            }
            case 'i': { /* desvio condicional */
                char var0;
                int idx0, n1, n2;
                if (fscanf(arquivo_entrada, "f %c%d %d %d", &var0, &idx0, &n1, &n2) != 4)
                    error("comando invalido", contaLinha);
                printf("%d if %c%d %d %d\n", contaLinha, var0, idx0, n1, n2);
                break;
            }
            case 'g': { /* desvio incondicional */
                int n1;
                if (fscanf(arquivo_entrada, "o %d", &n1) != 1)
                    error("comando invalido", contaLinha);
                printf("%d go %d\n", contaLinha, n1);
                break;
            }
            default: error("comando desconhecido", contaLinha);
        }
        contaLinha ++;
        fscanf(arquivo_entrada, " ");
    }

    return (funcp) codigo;x 
}

static void error (const char *msg, int line){
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(EXIT_FAILURE);
}