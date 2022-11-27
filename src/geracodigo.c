#include <stdio.h>
#include <stdlib.h>
#include "geracodigo.h"

funcp geracodigo(FILE *f, unsigned char *codigo);
static void error (const char *msg, int line);
int escreverVet(unsigned char *codigo, int posicaoVet, unsigned char novoCod[], int tamanhoAdicional);
int posicaoVariavel(unsigned char *codigo, int posicaoVet, int val );
int operacao(unsigned char *codigo, int posicaoVet, char var0, int idx0);

#define TAMANHO_DESVIO_CON 12
#define TAMANHO_INICIO 8
#define TAMANHO_DESVCOND 6
#define TAMANHO_DESVIO_INCON 5
#define TAMANHO_VAR_REG 4
#define TAMANHO_OPERACAO 2
#define TAMANHO_FINAL 2
#define TAMANHO_MULT_PAR 2

static unsigned char inicioCod[TAMANHO_INICIO] = {0x55, 0x48, 0x89, 0xE5, 0x48, 0x83, 0xEC, 0x20};    
static unsigned char desvioCond_P1[TAMANHO_DESVCOND] = {0x0F, 0x8C, 0x00, 0x00, 0x00, 0x00};
static unsigned char desvioCod_P2[TAMANHO_DESVCOND] = {0x0F, 0x84, 0x00, 0x00, 0x00, 0x00};
static unsigned char desvioIncon[TAMANHO_DESVIO_INCON] = {0XE9, 0x00, 0x00, 0x00, 0x00};
static unsigned char movParamReg[3] = {0x44, 0x8B, 0x55};
static unsigned char movRegiVar[3] = {0x44, 0x89, 0x55};
static unsigned char movSubVarVar[3] = {0x44, 0x29, 0x55};
static unsigned char somaVarVar[3] = {0x44, 0x01, 0x55};
static unsigned char multiVV_VP[3] = {0x44, 0x0F, 0xAF};
static unsigned char multiConst[3] = {0x45, 0x69, 0xD2};
static unsigned char operacao_P1[TAMANHO_OPERACAO] = {0xFF, 0x00};
static unsigned char operacao_P2[TAMANHO_OPERACAO] = {0xFE, 0x00};
static unsigned char multi_PAR[TAMANHO_MULT_PAR] = {0x0F, 0xAF};
static unsigned char finalCod[TAMANHO_FINAL] = {0xC9, 0xC3};

typedef int (*funcp) ();
struct If{																		
    int desvioIfJL[20];
    int destinoJL;
    int desvioIfJE[20];																	
    int destinoJE;
};
struct Go{
    int desvioGO[20];
    int destinoGO;
};

static void error (const char *msg, int line){
    fprintf(stderr, "erro %s na linha %d\n", msg, line);											
    exit(EXIT_FAILURE);
}

int posicaoVariavel(unsigned char *codigo, int posicaoVet, int val){
    int retorno = 0;
    if(val == 1){
        codigo[posicaoVet] = 0xF0;
        retorno = posicaoVet+1;
        return retorno;
    }else if(val == 2){
        codigo[posicaoVet] = 0xF4;
        retorno = posicaoVet+1;
        return retorno;
    }else if(val == 3){
        codigo[posicaoVet] = 0xF8;
        retorno = posicaoVet+1;
        return retorno;
    }else if(val == 4){
        codigo[posicaoVet] = 0xFC;
        retorno = posicaoVet+1;
        return retorno;
    }
    return 0;
}

int operacao(unsigned char *codigo, int posicaoVet, char var0, int idx0){
    if(var0=='p'){
        if(idx0 ==1){   
            posicaoVet = escreverVet(codigo, posicaoVet, operacao_P1, TAMANHO_OPERACAO);					
        }else if(idx0 ==2){
            posicaoVet = escreverVet(codigo, posicaoVet, operacao_P2, TAMANHO_OPERACAO);  
        }
        return 0;
    }else if(var0=='v'){
        codigo[posicaoVet] = 0x7D;
        posicaoVet++;
        posicaoVariavel(codigo, posicaoVet, idx0);
        codigo[posicaoVet] = 0x00;
        posicaoVet++;
        return posicaoVet;
    }
    return 0;
}

int escreverVet(unsigned char *codigo, int posicaoVet, unsigned char novoCod[], int tamanhoAdicional){
    int retorno = 0;
    for(int i = 0; i <= tamanhoAdicional; i++){
        codigo[posicaoVet + i] = novoCod[i];
    }
    retorno = posicaoVet + tamanhoAdicional;
    return retorno;
}

funcp geracodigo(FILE *arquivo_entrada, unsigned char codigo[]){	
    struct Go desvio;
    struct If desvio2;
    int posicaoVet = 0;
    int c = 0;
    int line = 1;
    desvio.destinoGO=0;
    desvio2.destinoJL=0;
    desvio2.destinoJE=0;
    
    posicaoVet = escreverVet(codigo, posicaoVet, inicioCod,TAMANHO_INICIO);

    while ((c = fgetc(arquivo_entrada)) != EOF){
        switch (c){
            case 'r':{																								
                char var0;
                int idx0;
                if (fscanf(arquivo_entrada, "et %c%d", &var0, &idx0) != 2)
                    error("comando invalido", line);

                switch(var0){
                    case 'p':
                        codigo[posicaoVet] = 0x89;
                        posicaoVet++;

                        if(idx0==1){
                            codigo[posicaoVet] = 0xF8;
                            posicaoVet++;

                        }else if(idx0==2){
                            codigo[posicaoVet] = 0xF0;
                            posicaoVet++;
                        }
                        break;
                    case 'v':
                        codigo[posicaoVet] = 0x8B;
                        posicaoVet++;
                        
                        codigo[posicaoVet] = 0x45;
                        posicaoVet++;
                        
                        posicaoVet = posicaoVariavel(codigo, posicaoVet, idx0);  
                        break;
                    case '$':
                        codigo[posicaoVet] = 0xB8;
                        posicaoVet++;
                        
                        *((int *)&codigo[posicaoVet])=idx0;
                        posicaoVet+=4;
                        break;
                    }
                posicaoVet = escreverVet(codigo, posicaoVet, finalCod,TAMANHO_FINAL);
                printf("%d ret %c%d\n", line, var0, idx0);
                break;
            }
            case 'v':
            case 'p':{ 																			
                char var0 = c, var1, op;
                int idx0, idx1;
                
                if (fscanf(arquivo_entrada, "%d %c= %c%d", &idx0, &op, &var1, &idx1) != 4){
                    error("comando invalido", line);
                }
                printf("%d %c%d %c= %c%d\n", line, var0, idx0, op, var1, idx1);

                if(op==':'){
                    if(var0=='p' && var1=='p'){
                        codigo[posicaoVet] = 0x89;
                        posicaoVet++;

                        if(idx0 == 1){
                            codigo[posicaoVet] = 0xF7;
                            posicaoVet++;
                        }else if(idx0 == 2){
                            codigo[posicaoVet] = 0xFE;
                            posicaoVet++;
                        }
                    }
                    if(var0=='v' && var1=='p'){   
                        codigo[posicaoVet] = 0x89;
                        posicaoVet++;

                        if(idx1 == 1){
                            codigo[posicaoVet] = 0x7D;
                            posicaoVet++;
                        }else if(idx1 == 2){
                            codigo[posicaoVet] = 0x75;
                            posicaoVet++;
                        }
                        posicaoVet = posicaoVariavel(codigo, posicaoVet, idx0);          
                    }
                    if(var0=='p' && var1=='$'){
                        if(idx0 == 1){
                            codigo[posicaoVet] = 0xBF;
                            posicaoVet++;
                        }else if(idx0 == 2){
                            codigo[posicaoVet] = 0xBE;
                            posicaoVet++;
                        }
                        *((int*)&codigo[posicaoVet]) = idx1;		
                        posicaoVet+=4;
                    }
                    if(var0=='p' && var1=='v'){   
                        codigo[posicaoVet] = 0x8B;
                        posicaoVet++;

                        if(idx0 == 1){
                            codigo[posicaoVet] = 0x7D;
                            posicaoVet++;
                        }else if(idx0 == 2){
                            codigo[posicaoVet] = 0x75;
                            posicaoVet++;
                        }
                        posicaoVet = posicaoVariavel(codigo, posicaoVet, idx1);
                    }
                    if(var0=='v' && var1=='$'){   
                        codigo[posicaoVet] = 0xC7;
                        posicaoVet++;
                        codigo[posicaoVet] = 0x45;
                        posicaoVet++;
                        
                        posicaoVet = posicaoVariavel(codigo, posicaoVet, idx0);
                        *((int *)&codigo[posicaoVet])=idx1;
                        posicaoVet+=4;
                    }
                    if(var0=='v'&& var1=='v'){
                        posicaoVet = escreverVet(codigo, posicaoVet, movParamReg,3);
                        posicaoVet = posicaoVariavel(codigo, posicaoVet, idx1);
                        
                        posicaoVet = escreverVet(codigo, posicaoVet, movRegiVar,3);
                        posicaoVet = posicaoVariavel(codigo, posicaoVet, idx0);		
                    }
                }
                if(op=='+'){												
                    if(var0=='p' && var1=='p'){
                        codigo[posicaoVet] = 0x01;
                        posicaoVet++;

                        if(idx0==1){
                            codigo[posicaoVet] = 0xF7;
                            posicaoVet++;
                        }else if(idx0==2){
                            codigo[posicaoVet] = 0xFE; 
                            posicaoVet++;
                        }
                    }
                    if(var0=='v' && var1=='p'){   
                        codigo[posicaoVet] = 0x01;
                        posicaoVet++;

                        if(idx1==1){
                            codigo[posicaoVet] = 0x7D;
                            posicaoVet++;
                        }else if(idx1==2){
                            codigo[posicaoVet] = 0x75;
                            posicaoVet++;
                        }
                        posicaoVet = posicaoVariavel(codigo, posicaoVet, idx0);             
                    }
                    if(var0=='p' && var1=='v'){   
                        codigo[posicaoVet] = 0x03;
                        posicaoVet++;

                        if(idx0==1){
                            codigo[posicaoVet] = 0x7D;
                            posicaoVet++;
                        }else if(idx0==2){
                            codigo[posicaoVet] = 0x75; 
                            posicaoVet++;
                        }
                        posicaoVet = posicaoVariavel(codigo, posicaoVet, idx1);
                    }
                    if(var0=='p' && var1=='$'){
                        codigo[posicaoVet] = 0x81;
                        posicaoVet++; 

                        if(idx0==1){
                            codigo[posicaoVet] = 0xC7;
                            posicaoVet++;
                        }else if(idx0==2){
                            codigo[posicaoVet] = 0xC6;
                            posicaoVet++;
                        }          	
                        *((int *)&codigo[posicaoVet])=idx1;
                        posicaoVet+=4;                     
                    }
                    if(var0=='v' && var1=='$'){    
                        codigo[posicaoVet] = 0x81;
                        posicaoVet++;
                        codigo[posicaoVet] = 0x45;
                        posicaoVet++;
                        
                        posicaoVet = posicaoVariavel(codigo, posicaoVet, idx0);
                        *((int *)&codigo[posicaoVet])=idx1;
                        posicaoVet+=4;
                    }
                    if(var0=='v' && var1=='v'){    
                        posicaoVet = escreverVet(codigo, posicaoVet, movParamReg,3);	
                        posicaoVet = posicaoVariavel(codigo, posicaoVet, idx1);
    
                        posicaoVet = escreverVet(codigo, posicaoVet, somaVarVar,3);	
                        posicaoVet = posicaoVariavel(codigo, posicaoVet, idx0);
                    } 
                }
                if(op=='-'){														
                    if(var0=='p' && var1=='p'){
                        codigo[posicaoVet] = 0x29;
                        posicaoVet++;

                        if(idx0==1){
                            codigo[posicaoVet] = 0xF7;
                            posicaoVet++;
                        }else if(idx0==2){
                            codigo[posicaoVet] = 0xFE;
                            posicaoVet++;
                        }	
                    }
                    if(var0=='v' && var1=='p'){   
                        codigo[posicaoVet] = 0x29;
                        posicaoVet++;

                        if(idx1==1){
                            codigo[posicaoVet] = 0x7D;
                            posicaoVet++;
                        }else if(idx1==2){
                            codigo[posicaoVet] = 0x75;
                            posicaoVet++;
                        }
                        posicaoVet = posicaoVariavel(codigo, posicaoVet, idx0);               
                    }
                    if(var0=='p' && var1=='v'){   
                        codigo[posicaoVet] = 0x2B;
                        posicaoVet++;

                        if(idx0==1){
                            codigo[posicaoVet] = 0x7D;
                            posicaoVet++;
                        }else if(idx0==2){
                            codigo[posicaoVet] = 0x75;
                            posicaoVet++;
                        }
                        posicaoVet = posicaoVariavel(codigo, posicaoVet, idx1);
                    }
                    if(var0=='p' && var1=='$'){
                        codigo[posicaoVet] = 0x81;
                        posicaoVet++;

                        if(idx0==1){
                            codigo[posicaoVet] = 0xEF;
                            posicaoVet++;
                        }else if(idx0==2){
                            codigo[posicaoVet] = 0xEE;
                            posicaoVet++;
                        }
                        *((int *)&codigo[posicaoVet])=idx1;
                        posicaoVet+=4;       
                    }
                    if(var0=='v' && var1=='$'){    
                        codigo[posicaoVet] = 0x81;
                        posicaoVet++;
                        codigo[posicaoVet] = 0x6D;
                        posicaoVet++;
                        
                        posicaoVet =posicaoVariavel(codigo, posicaoVet, idx0);
                        *((int *)&codigo[posicaoVet])=idx1;
                        posicaoVet+=4;
                    }
                    if(var0=='v' && var1=='v'){    
                        posicaoVet = escreverVet(codigo, posicaoVet, movParamReg,3);
                        posicaoVet = posicaoVariavel(codigo, posicaoVet, idx1);
                        posicaoVet = escreverVet(codigo, posicaoVet, movSubVarVar,3);
                        posicaoVet = posicaoVariavel(codigo, posicaoVet, idx0);
                    }    	
                }
                if(op=='*'){
                    if(var0=='p' && var1=='p'){
                        posicaoVet = escreverVet(codigo, posicaoVet, multi_PAR,TAMANHO_MULT_PAR);
                        if(idx0==1){
                            codigo[posicaoVet] = 0xFE;
                            posicaoVet++;
                        }else if(idx0==2){
                            codigo[posicaoVet] = 0xF7;
                            posicaoVet++;
                        }
                    }
                    if(var0=='v' && var1=='p'){   
                        posicaoVet = escreverVet(codigo, posicaoVet, movParamReg,3);
                        posicaoVet = posicaoVariavel(codigo, posicaoVet, idx0);
                        posicaoVet = escreverVet(codigo, posicaoVet, multiVV_VP,3);

                        if(idx1==1){
                            codigo[posicaoVet] = 0xD7;
                            posicaoVet++;
                        }else if(idx1==2){
                            codigo[posicaoVet] = 0xD6;
                            posicaoVet++;
                        }                                 
                        
                        posicaoVet = escreverVet(codigo, posicaoVet, movRegiVar,3);
                        posicaoVet = posicaoVariavel(codigo, posicaoVet, idx0);                              
                    }
                    if(var0=='p' && var1=='v'){   
                        posicaoVet = escreverVet(codigo, posicaoVet, multi_PAR,TAMANHO_MULT_PAR);
                        if(idx0==1){
                            codigo[posicaoVet] = 0x7D;
                            posicaoVet++;
                        }else if(idx0==2){
                            codigo[posicaoVet] = 0x75;
                            posicaoVet++;
                        }
                        posicaoVet =posicaoVariavel(codigo, posicaoVet, idx1);
                    }
                    if(var0=='p' && var1=='$'){
                        codigo[posicaoVet] = 0x69;
                        posicaoVet++;

                        if(idx0==1){
                            codigo[posicaoVet] = 0xFF;
                            posicaoVet++;
                        }else if(idx0==2){
                            codigo[posicaoVet] = 0xF6;
                            posicaoVet++;
                        }
                        *((int *)&codigo[posicaoVet])=idx1;
                        posicaoVet+=4;              
                    }   
                    if(var0=='v' && var1=='$'){    
                        posicaoVet = escreverVet(codigo, posicaoVet, movParamReg,3);
                        posicaoVet = posicaoVariavel(codigo, posicaoVet, idx0);
                        posicaoVet = escreverVet(codigo, posicaoVet, multiConst,3);

                        *((int *)&codigo[posicaoVet])=idx1;
                        posicaoVet+=4;       
                        posicaoVet = escreverVet(codigo, posicaoVet, movRegiVar,3);
                        posicaoVet =posicaoVariavel(codigo, posicaoVet, idx0); 
                    }
                    if(var0=='v'&& var1=='v'){

                        posicaoVet = escreverVet(codigo, posicaoVet, movParamReg,3);
                        posicaoVet =posicaoVariavel(codigo, posicaoVet, idx0);
                        posicaoVet = escreverVet(codigo, posicaoVet, multiVV_VP,3);

                        codigo[posicaoVet] = 0x55;
                        posicaoVet++;

                        posicaoVet =posicaoVariavel(codigo, posicaoVet, idx1);
                        posicaoVet = escreverVet(codigo, posicaoVet, movRegiVar,3);
                        posicaoVet =posicaoVariavel(codigo, posicaoVet, idx0);
                    } 
                }       
                break;                
            }
            case 'i': { 																								 
                char var0;
                int idx0, n1, n2;
                
                if (fscanf(arquivo_entrada, "f %c%d %d %d", &var0, &idx0, &n1, &n2) != 4){
                    error("comando invalido", line);
                }
                
                operacao(codigo, posicaoVet, var0,idx0);
                posicaoVet = escreverVet(codigo, posicaoVet, desvioCond_P1,TAMANHO_DESVCOND);
                desvio2.desvioIfJL[desvio2.destinoJL] = n1;
                desvio2.destinoJL++;

                posicaoVet = escreverVet(codigo, posicaoVet, desvioCod_P2,TAMANHO_DESVCOND);
                desvio2.desvioIfJE[desvio2.destinoJE] = n2;
                desvio2.destinoJE++;

                printf("%d if %c%d %d %d\n", line, var0, idx0, n1, n2);
                break;
            }
            case 'g': {																								
                int n1; 
                if (fscanf(arquivo_entrada, "o %d", &n1) != 1){
                    error("Comando invalido", line);
                }
                posicaoVet = escreverVet(codigo, posicaoVet, desvioIncon,TAMANHO_DESVIO_INCON);
                desvio.desvioGO[desvio.destinoGO] = n1; 
                desvio.destinoGO++;
                printf("%d go %d\n", line, n1);
                break;
            }
            default: error("comando desconhecido", line);
        }
        line ++;
        fscanf(arquivo_entrada, " ");
    }
    return (funcp)codigo;
}