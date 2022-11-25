#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "geracodigo.h"

#define TAMANHO_INICIO 11
#define TAMANHO_DESVIO_CON 12
#define TAMANHO_OPERACAO 2
#define TAMANHO_DESVIO_INCON 5
#define TAMANHO_FINAL 2

static unsigned char inicioCod[TAMANHO_INICIO] = {0x55, 0x48, 0x89, 0xE5, 0x48, 0x83, 0xEC, 0x10, 0x89, 0x7D, 0xE4};    
static unsigned char desvioCon[TAMANHO_DESVIO_CON] = {0x0F, 0x8C, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x84, 0x00, 0x00, 0x00, 0x00};
static unsigned char operacao_P1[TAMANHO_OPERACAO] = {0xFF, 0x00};
static unsigned char operacao_P2[TAMANHO_OPERACAO] = {0xFE, 0x00};
static unsigned char desvioIncon[TAMANHO_DESVIO_INCON] = {0XE9, 0x00, 0x00, 0x00, 0x00};
static unsigned char finalCod[TAMANHO_FINAL] = {0xC9, 0xC3};

typedef int (*funcp) (int x);

struct If{																		
    int desvioIfJl[20];
    int desvioIfje[20];																	/* estruturas auxiliares para preencher os espa�os dos jumps*/
    int linhaIfjl;/*contador*/  
    int linhaIfje;/*contador*/ 
};
struct Go{
    int desvioGo[20];
    int linhaGo;/*contador*/
};

static void error (const char *msg, int line){
    fprintf(stderr, "erro %s na linha %d\n", msg, line);											
    exit(EXIT_FAILURE);
}

int posicaoVariavel(unsigned char *codigo, int posicaoVet, int val){
    int retorno = 0;
    if(val == 1){
        codigo[posicaoVet]=0xF0;
            
            retorno = posicaoVet+1;

            return retorno;
    }else if(val == 2){
        codigo[posicaoVet]=0xF4;
            retorno = posicaoVet+1;

            return retorno;
    }else if(val == 3){
        codigo[posicaoVet]=0xF8;
            retorno = posicaoVet+1;

            return retorno;
    }else{
        codigo[posicaoVet]=0xFC;
            retorno = posicaoVet+1;

            return retorno;
    }
    return 0;
}
int operacao(unsigned char *codigo, int posicaoVet, char var0, int idx0){
    int retorno = 0;

    if(var0=='p'){
        if(idx0 ==1){   
            posicaoVet +=escreverVet(codigo, posicaoVet, operacao_P1, TAMANHO_OPERACAO);					
        }else if(idx0 ==2){
            posicaoVet +=escreverVet(codigo, posicaoVet, operacao_P2, TAMANHO_OPERACAO);  
        }
        return 0;
    }else if(var0=='v'){
        codigo[posicaoVet]=0x7D;
        posicaoVet++;
        posicaoVariavel(codigo, posicaoVet, idx0);
        codigo[posicaoVet]=0x00;
        posicaoVet++;
        retorno = posicaoVet +2;
        return retorno;

    }
    return 0;
}

int escreverVet(unsigned char *codigo, int posicaoVet, unsigned char novoCod[], int tamanhoAdicional){
    int retorno = 0;
    for(int i = 0; i < tamanhoAdicional; posicaoVet++){
        codigo[posicaoVet+i] = novoCod[i];
    }
    retorno = posicaoVet + tamanhoAdicional;
    return retorno;
}


funcp geraCodigo(FILE *arquivo_entrada, unsigned char codigo[]){	
    funcp cod;
    int posicaoVet = 0;
    int line = 1;
    int auxFor = 0;
    int c;
    unsigned int auxDesv = 0;
    int a=0;

    int num,num2,linha;/*contador endereco*/
    unsigned char *pnum=(unsigned char*)&num;
    unsigned char *pnum2=(unsigned char*)&num2;
    long int endereco[20];

    struct Go desvio;
    struct If desvio2;

    desvio.linhaGo=0;
    desvio2.linhaIfjl=0;
    desvio2.linhaIfje=0;
    linha=0;

    //inicio pilha
  
    posicaoVet +=escreverVet(codigo, posicaoVet, inicioCod,TAMANHO_INICIO);


    while ((c = fgetc(arquivo_entrada)) != EOF){                                        

        endereco[line]=(long)(&codigo[posicaoVet]); /* salvando os endere�os das linhas no vetor*/
        line++;

        switch (c){
            case 'i': { 																								 /* desvio condicional */
                char var0;
                int idx0, n1, n2;
                
                if (fscanf(arquivo_entrada, "f %c%d %d %d", &var0, &idx0, &n1, &n2) != 4){
                    error("comando invalido", line);
                }
                printf("%d if %c%d %d %d\n", line, var0, idx0, n1, n2);
                codigo[posicaoVet++] = 0x83;
                posicaoVet++;
                posicaoVet += operacao(codigo, posicaoVet, var0,idx0);
                
            
                desvio2.desvioIfJl[desvio2.linhaIfjl]=n1;
                desvio2.linhaIfjl++;
                
                posicaoVet +=escreverVet(codigo, posicaoVet, desvioCon,TAMANHO_DESVIO_CON);

                desvio2.desvioIfje[desvio2.linhaIfje]=n2;
                desvio2.linhaIfje++;

                printf("%d if %c%d %d %d\n", line, var0, idx0, n1, n2);
                break;
            }
            case 'g': {																								/* desvio incondicional */
                int n = 0; 
                if (fscanf(arquivo_entrada, "o %d", &n) != 1){
                    error("Comando invalido", line);
                }
                posicaoVet +=escreverVet(codigo, posicaoVet, desvioIncon,TAMANHO_DESVIO_INCON);
                
                desvio.desvioGo[desvio.linhaGo]=n; 
                desvio.linhaGo++;
                printf("%d go %d\n", line, n);
                break;
            }
            case 'r':{																								/* retorno */
                char var0;
                int idx0;
                
                if (fscanf(arquivo_entrada, "et %c%d", &var0, &idx0) != 2)
                    error("comando invalido", line);
                if(var0=='p'){
                    codigo[posicaoVet]=0x89;
                    posicaoVet++;
                    if(idx0==1){

                        codigo[posicaoVet]=0xF8;
                        posicaoVet++;

                    }else if(idx0==2){

                        codigo[posicaoVet]=0xF0;
                        posicaoVet++;
                    }
                }
                if(var0=='v'){         
                    codigo[posicaoVet]=0x8B;
                    posicaoVet++;
                    
                    codigo[posicaoVet]=0x45;
                    posicaoVet++;
                    
                posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);        
                }
                if(var0=='$'){   
                    codigo[posicaoVet]=0xB8;
                    posicaoVet++;
                    
                    for(auxFor = 0; auxFor<4; auxFor++){
                        codigo[posicaoVet] = idx0>>(8*(4-auxFor)) & 0xFF;
                        posicaoVet++;
                    }
                }
                    posicaoVet +=escreverVet(codigo, posicaoVet, finalCod,TAMANHO_FINAL);
                printf("%d ret %c%d\n", line, var0, idx0);
                break;
            }//fim do case r
            
            case 'v':
            case 'p':{ 																			/* atribuicao */
                char var0 = c, var1, op;
                int idx0, idx1;
                
                if (fscanf(arquivo_entrada, "%d %c= %c%d", &idx0, &op, &var1, &idx1) != 4){
                    error("comando invalido", line);
                }
                printf("%d %c%d %c= %c%d\n", line, var0, idx0, op, var1, idx1);

                if(op==':'){
                    if(var0=='p' && var1=='p'){
                        codigo[posicaoVet]=0x89;
                        posicaoVet++;

                        if(idx0 == 1){
                            codigo[posicaoVet]=0xF7;
                            posicaoVet++;
                        }else if(idx0 == 2){
                            codigo[posicaoVet]=0xFE;
                            posicaoVet++;
                        }
                        
                    }//fim pp
                    if((var0=='p' && var1=='v')||(var0=='v' && var1=='p')){
                        if(var0=='v' && var1=='p'){   
                            codigo[posicaoVet]=0x89;
                            posicaoVet++;
                            if(idx1 == 1){
                                codigo[posicaoVet]=0x7D;
                                posicaoVet++;
                            }else if(idx1 == 2){
                                codigo[posicaoVet]=0x75;
                                posicaoVet++;
                            }
                            
                           posicaoVet += posicaoVariavel(codigo, posicaoVet, idx0);          
                        }//fim vp
                        if(var0=='p' && var1=='$'){
                            if(idx0 == 1){
                                codigo[posicaoVet]=0xBF;
                                posicaoVet++;
                            }else if(idx0 == 2){
                                codigo[posicaoVet]=0xBE;
                                posicaoVet++;
                            }
                            for(auxFor = 0; auxFor<4; auxFor++){
                                codigo[posicaoVet] = idx1>>(8*(4-auxFor)) & 0xFF;
                                posicaoVet++;
                            }
                        }
                        if(var0=='p' && var1=='v'){   
                            codigo[posicaoVet]=0x8B;
                            posicaoVet++;
                            if(idx0 == 1){
                                codigo[posicaoVet]=0x7D;
                                posicaoVet++;
                            }else if(idx0 == 2){
                                codigo[posicaoVet]=0x75;
                                posicaoVet++;
                            }
                            

                            posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx1);;
                        }
                    }//fim pv        
                    //fim p$
                    if(var0=='v' && var1=='$'){   

                        codigo[posicaoVet]=0xC7;
                        posicaoVet++;
                        codigo[posicaoVet]=0x45;
                        posicaoVet++;
                        
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);

                        *((int *)&codigo[posicaoVet])=idx0;/*essa linha coloca o numero em hexa, da forma little endian, no vetor*/
            posicaoVet+=4;/*soma 4 pois um int tem esse tamanho*/
                    }//fim v$
                    if(var0=='v'&& var1=='v'){
                        codigo[posicaoVet]=0x44;
                        posicaoVet++;
                        codigo[posicaoVet]=0x8B;
                        posicaoVet++;
                        codigo[posicaoVet]=0x55;
                        posicaoVet++;/*mov var1 para um registrador*/
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx1);;
                        
                        codigo[posicaoVet]=0x44;
                        posicaoVet++;
                        codigo[posicaoVet]=0x89;
                        posicaoVet++;
                        codigo[posicaoVet]=0x55;
                        posicaoVet++;/*mov %ebx, (%rbp)*/
                        
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);		
                        
                    }//fim vv 
                }//fim do :   
                if(op=='+'){												/* soma */
                    if(var0=='p' && var1=='p'){
                        codigo[posicaoVet]=0x01;
                        posicaoVet++;
                        if(idx0==1){
                            codigo[posicaoVet]=0xF7;
                            posicaoVet++;
                        }else if(idx0==2){
                            codigo[posicaoVet]=0xFE; 
                            posicaoVet++;
                        }
                
                    }//fim pp
                    if((var0=='p' && var1=='v')||(var0=='v' && var1=='p')){
                        if(var0=='v' && var1=='p'){   
                            codigo[posicaoVet]=0x01;
                            posicaoVet++;
                            if(idx1==1){
                                codigo[posicaoVet]=0x7d;
                                posicaoVet++;
                            }else if(idx1==2){
                                codigo[posicaoVet]=0x75;
                                posicaoVet++;
                            }
                            
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);             
                        }//fim vp
                        if(var0=='p' && var1=='v'){   
                            codigo[posicaoVet]=0x03;
                            posicaoVet++;
                            if(idx0==1){
                                codigo[posicaoVet]=0x7D;
                                posicaoVet++;
                            }else if(idx0==2){
                                codigo[posicaoVet]=0x75; 
                                posicaoVet++;
                            }
                            posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx1);;
                        }//fim pv
                    }
                    if(var0=='p' && var1=='$'){
                        codigo[posicaoVet]=0x81;
                        posicaoVet++;  
                        if(idx0==1){
                            codigo[posicaoVet]=0xC7;
                            posicaoVet++;
                        }else if(idx0==2){
                            codigo[posicaoVet]=0xC6;
                            posicaoVet++;
                        }          	
                        *((int *)&codigo[posicaoVet])=idx0;/*essa linha coloca o numero em hexa, da forma little endian, no vetor*/
            posicaoVet+=4;/*soma 4 pois um int tem esse tamanho*/                          
                    }//fim p$
                    if(var0=='v' && var1=='$'){    
                        codigo[posicaoVet]=0x81;
                        posicaoVet++;
                        codigo[posicaoVet]=0x45;
                        posicaoVet++;
                        
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);
                        *((int *)&codigo[posicaoVet])=idx0;/*essa linha coloca o numero em hexa, da forma little endian, no vetor*/
            posicaoVet+=4;/*soma 4 pois um int tem esse tamanho*/
                    }//fim v$
                    if(var0=='v' && var1=='v'){    
                        codigo[posicaoVet]=0x44;
                        posicaoVet++;
                        codigo[posicaoVet]=0x8b;
                        posicaoVet++;
                        codigo[posicaoVet]=0x55;
                        posicaoVet++;	/*mov var1 para um registrador*/
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx1);;
    
                        codigo[posicaoVet]=0x44;
                        posicaoVet++;
                        codigo[posicaoVet]=0x01;
                        posicaoVet++;
                        codigo[posicaoVet]=0x55;
                        posicaoVet++;	/*mov o valor do resultado de volta para a posicao de memoria*/
                        
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);
                    }//fim vv   
                            
                }//fim do +
                if(op=='-'){														/* subtracao */
                    if(var0=='p' && var1=='p'){
                        codigo[posicaoVet]=0x29;
                        posicaoVet++;
                        if(idx0==1){
                            codigo[posicaoVet]=0xF7;
                            posicaoVet++;
                        }else if(idx0==2){
                            codigo[posicaoVet]=0xFE;
                            posicaoVet++;
                        }	
                    }//fim pp
                    if((var0=='p' && var1=='v')||(var0=='v' && var1=='p')){
                        if(var0=='v' && var1=='p'){   
                            codigo[posicaoVet]=0x29;
                            posicaoVet++;
                            if(idx1==1){
                                codigo[posicaoVet]=0x7D;
                                posicaoVet++;
                            }else if(idx1==2){
                                codigo[posicaoVet]=0x75;
                                posicaoVet++;
                            }
        
                            
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);               
                        }//fim vp
                        if(var0=='p' && var1=='v'){   
                            codigo[posicaoVet]=0x2b;
                            posicaoVet++;
                            if(idx0==1){
                                codigo[posicaoVet]=0x7D;
                                posicaoVet++;
                            }else if(idx0==2){
                                codigo[posicaoVet]=0x75;
                                posicaoVet++;
                            }
                            
                            posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx1);;
                        }//fim pv
                    }        
                    if(var0=='p' && var1=='$'){
                        codigo[posicaoVet]=0x81;
                        posicaoVet++;
                        if(idx0==1){
                                codigo[posicaoVet]=0xEF;
                                posicaoVet++;
                            }else if(idx0==2){
                                codigo[posicaoVet]=0xEE;
                                posicaoVet++;
                            }
                        
                        *((int *)&codigo[posicaoVet])=idx0;/*essa linha coloca o numero em hexa, da forma little endian, no vetor*/
                        posicaoVet+=4;/*soma 4 pois um int tem esse tamanho*/              
                    }//fim p$
                    if(var0=='v' && var1=='$'){    
                        codigo[posicaoVet]=0x81;
                        posicaoVet++;
                        codigo[posicaoVet]=0x6d;
                        posicaoVet++;
                        
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);
                        *((int *)&codigo[posicaoVet])=idx0;/*essa linha coloca o numero em hexa, da forma little endian, no vetor*/
                        posicaoVet+=4;/*soma 4 pois um int tem esse tamanho*/
                    }//fim v$
                    if(var0=='v' && var1=='v'){    
                        codigo[posicaoVet]=0x44;
                        posicaoVet++;
                        codigo[posicaoVet]=0x8B;
                        posicaoVet++;
                        codigo[posicaoVet]=0x55;
                        posicaoVet++;/*mov var1 para um registrador*/
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx1);;
            
                        codigo[posicaoVet]=0x44;
                        posicaoVet++;
                        codigo[posicaoVet]=0x29;
                        posicaoVet++;
                        codigo[posicaoVet]=0x55;
                        posicaoVet++;/*mov o valor do resultado de volta para a posicao de memoria*/
                        
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);
                    }//fim vv        	
                }//fim do -
                if(op=='*'){
                    if(var0=='p' && var1=='p'){
                    codigo[posicaoVet]=0x0F;
                    posicaoVet++;
                    codigo[posicaoVet]=0xAF;
                    posicaoVet++;
                    if(idx0==1){
                            codigo[posicaoVet]=0xFE;
                            posicaoVet++;
                        }else if(idx0==2){
                            codigo[posicaoVet]=0xF7;
                            posicaoVet++;
                        }
                			
                    }
                    if((var0=='p' && var1=='v')||(var0=='v' && var1=='p')){
                        if(var0=='v' && var1=='p'){   
                            codigo[posicaoVet]=0x44;
                            posicaoVet++;
                            codigo[posicaoVet]=0x8b;
                            posicaoVet++;
                            codigo[posicaoVet]=0x55;
                            posicaoVet++; /*mov variavel para registrador*/
                            
                            posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);
                                
                            codigo[posicaoVet]=0x44;
                            posicaoVet++;
                            codigo[posicaoVet]=0x0f;
                            posicaoVet++;
                            codigo[posicaoVet]=0xaf;
                            posicaoVet++;   

                            if(idx1==1){
                                codigo[posicaoVet]=0xD7;
                                posicaoVet++;
                            }else if(idx0==2){
                                codigo[posicaoVet]=0xD6;
                                posicaoVet++;
                            }                                 
                            
                            codigo[posicaoVet]=0x44;
                            posicaoVet++;
                            codigo[posicaoVet]=0x89;
                            posicaoVet++;
                            codigo[posicaoVet]=0x55;
                            posicaoVet++; /*mov o valor do resultado de volta para a posicao de memoria*/
                            
                            posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);                              
                        }//fim vp
                        if(var0=='p' && var1=='v'){   
                            codigo[posicaoVet]=0x0f;posicaoVet++;
                            codigo[posicaoVet]=0xaf;posicaoVet++;
                            if(idx0==1){
                                codigo[posicaoVet]=0x7D;
                                posicaoVet++;
                            }else if(idx0==2){
                                codigo[posicaoVet]=0x75;
                                posicaoVet++;
                            }
                            posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx1);;
                        }
                    }//fim pv        
                    if(var0=='p' && var1=='$'){
                        codigo[posicaoVet]=0x69;
                        posicaoVet++;
                        if(idx0==1){
                            codigo[posicaoVet]=0xFF;
                            posicaoVet++;
                        }else if(idx0==2){
                            codigo[posicaoVet]=0xF6;
                            posicaoVet++;
                        }
                        *((int *)&codigo[posicaoVet])=idx0;/*essa linha coloca o numero em hexa, da forma little endian, no vetor*/
            posicaoVet+=4;/*soma 4 pois um int tem esse tamanho*/                
                    }   //fim p$
                    if(var0=='v' && var1=='$'){    
                        codigo[posicaoVet]=0x44;
                        posicaoVet++;
                        codigo[posicaoVet]=0x8b;
                        posicaoVet++;
                        codigo[posicaoVet]=0x55;
                        posicaoVet++;
                        
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);
                        
                        codigo[posicaoVet]=0x45;
                        posicaoVet++;
                        codigo[posicaoVet]=0x69;
                        posicaoVet++;
                        codigo[posicaoVet]=0xd2;
                        posicaoVet++;
                        *((int *)&codigo[posicaoVet])=idx0;/*essa linha coloca o numero em hexa, da forma little endian, no vetor*/
            posicaoVet+=4;/*soma 4 pois um int tem esse tamanho*/
                                        
                        codigo[posicaoVet]=0x44;
                        posicaoVet++;
                        codigo[posicaoVet]=0x89;
                        posicaoVet++;
                        codigo[posicaoVet]=0x55;
                        posicaoVet++;/*mov o valor do resultado de volta para a posicao de memoria*/
                        
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0); 
                    }
                    if(var0=='v'&& var1=='v'){
                        codigo[posicaoVet]=0x44;
                        posicaoVet++;
                        codigo[posicaoVet]=0x8b;
                        posicaoVet++;
                        codigo[posicaoVet]=0x55;
                        posicaoVet++;/*mov var1 para um registrador*/
                        
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);

                        codigo[posicaoVet]=0x44;
                        posicaoVet++;
                        codigo[posicaoVet]=0x0f;
                        posicaoVet++;
                        codigo[posicaoVet]=0xaf;
                        posicaoVet++;
                        codigo[posicaoVet]=0x55;
                        posicaoVet++;
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx1);; 

                        codigo[posicaoVet]=0x44;
                        posicaoVet++;
                        codigo[posicaoVet]=0x89;
                        posicaoVet++;
                        codigo[posicaoVet]=0x55;
                        posicaoVet++;/*mov o valor do resultado de volta para a posicao de memoria*/
                        
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);
                    } 
                }//fim da *             
                break;                
            }
            }
        line ++;
        fscanf(arquivo_entrada, " ");
    }
   
   /*preenchendo os espa�os (00) */
  
        auxDesv = 0;    
        desvio.linhaGo = 0;
        while(codigo[posicaoVet]!=0xc9)/*percorre o vetor ate o fim*/
        {
            if(codigo[posicaoVet]==0xe9){/*ate achar a instrucao do go*/
                auxDesv+=5;/* anda 5 p pegar o endere�o da proxima instrucao */
                num = endereco[desvio.desvioGo[desvio.linhaGo]-1]-(long)&codigo[posicaoVet];/* calculando o deslocamento */
                auxDesv-=4; 
                desvio.linhaGo++;              
                while(a<4){
                    codigo[posicaoVet]=pnum[a];
                    posicaoVet++; a++;
                }
                a=0;
            }
            posicaoVet++;
        }   
        auxDesv = 0;
        desvio2.linhaIfje=0; desvio2.linhaIfjl=0;
        while(codigo[posicaoVet]!=0xc9){
            if(codigo[posicaoVet]==0x8c){/* ou ate achar a instrucao do if*/
                auxDesv+=5;/* anda 5 p pegar o endere�o da proxima instrucao */
                num2=endereco[desvio2.desvioIfJl[desvio2.linhaIfjl]-1]-(long)&codigo[posicaoVet];/* calculando o deslocamento */
                auxDesv-=4;/* volta 4 para preencher */

               while(a<4){
                    codigo[posicaoVet]=pnum2[a];
                    posicaoVet++; a++;
                }
                a=0;
                desvio2.linhaIfjl++;
            }
            if(codigo[posicaoVet]==0x84){
                auxDesv+=5;/* anda 5 p pegar o endere�o da proxima instrucao */
                num2=endereco[desvio2.desvioIfje[desvio2.linhaIfje]-1]-(long)&codigo[posicaoVet];/* calculando o deslocamento */
                auxDesv-=4;/* volta 4 para preencher */
               while(a<4){/* preenche o vetor */
                    codigo[posicaoVet]=pnum2[a];
                    posicaoVet++; a++;
                }
                a=0;
                desvio2.linhaIfje++;
            } 
            posicaoVet++;
        }   
  cod=(funcp)codigo;
  return cod;

}

