//Leo Land Bairos Lomardo - 2020201
//Lucas Daibes Rachid de Lucena - 2010796

#include <stdio.h>
#include <stdlib.h>
#include "geracodigo.h"
/*-------------------------------------------------------------------------*/
#define MAX_LINHAS 20
#define SIZE_FUNCTION 11

#define SIZE_RET_CST 7
#define SIZE_RET_PAR 5
#define SIZE_RET_VAR 5
#define SIZE_MOV_CST_REG 6
#define SIZE_MOV_VAR_REG 4
#define SIZE_MOV_PAR_REG 4
#define SIZE_BASE_OPR_CST 7
#define SIZE_BASE_OPR_VAR 4
#define SIZE_BASE_OPR_PAR 4
#define SIZE_MULT_OPR_CST 7
#define SIZE_MULT_OPR_VAR 5
#define SIZE_MULT_OPR_PAR 5
#define SIZE_MOV_CST_PAR 5
#define SIZE_MOV_REG_VAR 4
#define SIZE_MOV_VAR_PAR 3
#define SIZE_MOV_PAR_PAR 3
#define SIZE_CALL 8
#define SIZE_CMPL 4
#define SIZE_JNE 2
/*-------------------------------------------------------------------------*/
static unsigned char code_func[SIZE_FUNCTION] = {0x55, 0x48, 0x89, 0xe5, 0x48, 0x83, 0xec, 0x20, 0x89, 0x7d, 0xe4};
static unsigned char code_ret_cst[SIZE_RET_CST] = {0xb8, 0x00, 0x00, 0x00, 0x00,0xc9, 0xc3};
static unsigned char code_ret_par[SIZE_RET_PAR] = {0x8b, 0x45, 0xe4,0xc9, 0xc3};
static unsigned char code_ret_var[SIZE_RET_VAR] = {0x8b, 0x45, 0x00,0xc9, 0xc3};
static unsigned char code_mov_cst_reg[SIZE_MOV_CST_REG] = {0x41, 0xba, 0x00, 0x00, 0x00, 0x00};
static unsigned char code_mov_var_reg[SIZE_MOV_VAR_REG] = {0x44, 0x8b, 0x55, 0x00};
static unsigned char code_mov_par_reg[SIZE_MOV_PAR_REG] = {0x44, 0x8b, 0x55, 0xe4};
static unsigned char code_opr_add_cst[SIZE_BASE_OPR_CST] = {0x41, 0x81, 0xc2, 0x00, 0x00, 0x00, 0x00};
static unsigned char code_opr_add_var[SIZE_BASE_OPR_VAR] = {0x44, 0x03, 0x55, 0x00};
static unsigned char code_opr_add_par[SIZE_BASE_OPR_PAR] = {0x44, 0x03, 0x55, 0xe4};
static unsigned char code_opr_sub_cst[SIZE_BASE_OPR_CST] = {0x41, 0x81, 0xea, 0x00, 0x00, 0x00, 0x00};
static unsigned char code_opr_sub_var[SIZE_BASE_OPR_VAR] = {0x44, 0x2b, 0x55, 0x00};
static unsigned char code_opr_sub_par[SIZE_BASE_OPR_PAR] = {0x44, 0x2b, 0x55, 0xe4};
static unsigned char code_opr_mult_cst[SIZE_MULT_OPR_CST] = {0x45, 0x69, 0xd2, 0x00, 0x00, 0x00, 0x00};
static unsigned char code_opr_mult_var[SIZE_MULT_OPR_VAR] = {0x44, 0x0f, 0xaf, 0x55, 0x00};
static unsigned char code_opr_mult_par[SIZE_MULT_OPR_PAR] = {0x44, 0x0f, 0xaf, 0x55, 0xe4};
static unsigned char code_mov_reg_var[SIZE_MOV_REG_VAR] = {0x44, 0x89, 0x55, 0x00};
static unsigned char code_mov_cst_par[SIZE_MOV_CST_PAR] = {0xbf, 0x00, 0x00, 0x00, 0x00};
static unsigned char code_mov_var_par[SIZE_MOV_VAR_PAR] = {0x8b, 0x7d, 0x00};
static unsigned char code_mov_par_par[SIZE_MOV_PAR_PAR] = {0x8b, 0x7d, 0xe4};
static unsigned char code_call[SIZE_CALL] = {0xe8,0x00,0x00,0x00,0x00,0x89,0x45,0x00};
static unsigned char code_zret_cmpl[SIZE_CMPL] = {0x41, 0x83, 0xfa, 0x00};
static unsigned char code_zret_jne[SIZE_JNE] = {0x75, 0x07};
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
typedef int (*funcp) ();


/*-------------------------------------------------------------------------*/
funcp geraCodigo (FILE *arquivo_entrada, unsigned char codigo[]);
static void error (const char *msg, int line);

/*-------------------------------------------------------------------------*/
funcp geraCodigo (FILE *arquivo_entrada, unsigned char codigo[]){
    
    unsigned int contaLinha = 1; //variavel que vai sendo incrementada a medida que as linhas vão sendo lidas, para, em caso de erro, retornar em qual linha ocorreu
    unsigned int contaFuncao = 0;
    int byte_aux = 0;   //percorre byte a byte
    int c; //caracter

      while ((c = fgetc(arquivo_entrada)) != EOF) {
        switch (c) {
            case 'r': {   /* caso retorno */
                char var0; 
                int idx0; 

                if (fscanf(arquivo_entrada, "et %c%d", &var0, &idx0) != 2)
                    error("comando invalido", contaLinha);
                printf("%d ret %c%d\n", contaLinha, var0, idx0);
                break;
            }

            case 'v':

            case 'p': {             /* atribuicao operacao aritimetica */
                char var0 = c, var1, op;
                int idx0, idx1;

                if (fscanf(arquivo_entrada, "%d %c= %c%d", &idx0, &op, &var1, &idx1) != 4)
                    error("comando invalido", contaLinha);

                    
                printf("%d %c%d %c= %c%d\n", contaLinha, var0, idx0, op, var1, idx1);
                break;
            }

            case 'i': {               /* desvio condicional */
                char var0;
                int idx0, n1, n2;

                if (fscanf(arquivo_entrada, "f %c%d %d %d", &var0, &idx0, &n1, &n2) != 4)
                    error("comando invalido", contaLinha);
                printf("%d if %c%d %d %d\n", contaLinha, var0, idx0, n1, n2);
                break;
            }

            case 'g': {                    /* desvio incondicional */
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

    return (funcp) codigo;
}

static void error (const char *msg, int line){
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(EXIT_FAILURE);
}
