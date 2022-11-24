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


struct If{																		
    int desvioIfJl[20];
    int desvioIfje[20];																	/* estruturas auxiliares para preencher os espaços dos jumps*/
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

int posicaoVariavel(unsigned char codigo[], int posicaoVet, int val ){
    int retorno = 0;
    if(val == 1){
        codigo[posicaoVet]=0xF0;
            posicaoVet+=1;
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
int operacao(unsigned char codigo[], int posicaoVet, char var0, int idx0){
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
        posicaoVet+=1;
        posicaoVariavel(codigo, posicaoVet, idx0);
        codigo[posicaoVet]=0x00;
        posicaoVet+=1;
        retorno = posicaoVet +2;
        return retorno;

    }
    return 0;
}

int escreverVet(unsigned char codigo[], int posicaoVet, unsigned char novoCod[], int tamanhoAdicional){
    int retorno = 0;
    for(int i = 0; i < tamanhoAdicional; posicaoVet+=1){
        codigo[posicaoVet+i] = novoCod[i];
    }
    retorno = posicaoVet + tamanhoAdicional;
    return retorno;
}


funcp geraCodigo(FILE *arquivo_entrada, unsigned char codigo[]){	
    funcp cod;
    int posicaoVet = 1;
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

        endereco[linha]=(long)(&codigo[posicaoVet]); /* salvando os endereços das linhas no vetor*/
        linha++;

        switch (c){
            case 'i': { 																								 /* desvio condicional */
                char var0;
                int idx0, n1, n2;
                
                if (fscanf(arquivo_entrada, "f %c%d %d %d", &var0, &idx0, &n1, &n2) != 4){
                    error("comando invalido", line);
                }
                printf("%d if %c%d %d %d\n", line, var0, idx0, n1, n2);
                codigo[posicaoVet++] = 0x83;
                posicaoVet+=1;
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
                    posicaoVet+=1;
                    if(idx0==1){

                        codigo[posicaoVet]=0xF8;
                        posicaoVet+=1;

                    }else if(idx0==2){

                        codigo[posicaoVet]=0xF0;
                        posicaoVet+=1;
                    }
                }
                if(var0=='v'){         
                    codigo[posicaoVet]=0x8B;
                    posicaoVet+=1;
                    
                    codigo[posicaoVet]=0x45;
                    posicaoVet+=1;
                    
                posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);        
                }
                if(var0=='$'){   
                    codigo[posicaoVet]=0xB8;
                    posicaoVet+=1;
                    
                    for(auxFor = 0; auxFor<4; auxFor++){
                        codigo[posicaoVet] = idx0>>(8*(4-auxFor)) & 0xFF;
                        posicaoVet+=1;
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
                        posicaoVet+=1;

                        if(idx0 == 1){
                            codigo[posicaoVet]=0xF7;
                            posicaoVet+=1;
                        }else if(idx0 == 2){
                            codigo[posicaoVet]=0xFE;
                            posicaoVet+=1;
                        }
                        
                    }//fim pp
                    if((var0=='p' && var1=='v')||(var0=='v' && var1=='p')){
                        if(var0=='v' && var1=='p'){   
                            codigo[posicaoVet]=0x89;
                            posicaoVet+=1;
                            if(idx1 == 1){
                                codigo[posicaoVet]=0x7D;
                                posicaoVet+=1;
                            }else if(idx1 == 2){
                                codigo[posicaoVet]=0x75;
                                posicaoVet+=1;
                            }
                            
                           posicaoVet += posicaoVariavel(codigo, posicaoVet, idx0);          
                        }//fim vp
                        if(var0=='p' && var1=='$'){
                            if(idx0 == 1){
                                codigo[posicaoVet]=0xBF;
                                posicaoVet+=1;
                            }else if(idx0 == 2){
                                codigo[posicaoVet]=0xBE;
                                posicaoVet+=1;
                            }
                            for(auxFor = 0; auxFor<4; auxFor++){
                                codigo[posicaoVet] = idx1>>(8*(4-auxFor)) & 0xFF;
                                posicaoVet+=1;
                            }
                        }
                        if(var0=='p' && var1=='v'){   
                            codigo[posicaoVet]=0x8B;
                            posicaoVet+=1;
                            if(idx0 == 1){
                                codigo[posicaoVet]=0x7D;
                                posicaoVet+=1;
                            }else if(idx0 == 2){
                                codigo[posicaoVet]=0x75;
                                posicaoVet+=1;
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

                        for(auxFor = 0; auxFor<4; auxFor++){
                            codigo[posicaoVet] = idx1>>(8*(4-auxFor)) & 0xFF;
                            posicaoVet+=1;
                        }
                    }//fim v$
                    if(var0=='v'&& var1=='v'){
                        codigo[posicaoVet]=0x44;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x8B;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x55;
                        posicaoVet+=1;/*mov var1 para um registrador*/
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx1);;
                        
                        codigo[posicaoVet]=0x44;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x89;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x55;
                        posicaoVet+=1;/*mov %ebx, (%rbp)*/
                        
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);		
                        
                    }//fim vv 
                }//fim do :   
                if(op=='+'){												/* soma */
                    if(var0=='p' && var1=='p'){
                        codigo[posicaoVet]=0x01;
                        posicaoVet+=1;
                        if(idx0==1){
                            codigo[posicaoVet]=0xF7;
                            posicaoVet+=1;
                        }else if(idx0==2){
                            codigo[posicaoVet]=0xFE; 
                            posicaoVet+=1;
                        }
                
                    }//fim pp
                    if((var0=='p' && var1=='v')||(var0=='v' && var1=='p')){
                        if(var0=='v' && var1=='p'){   
                            codigo[posicaoVet]=0x01;
                            posicaoVet+=1;
                            if(idx1==1){
                                codigo[posicaoVet]=0x7d;
                                posicaoVet+=1;
                            }else if(idx1==2){
                                codigo[posicaoVet]=0x75;
                                posicaoVet+=1;
                            }
                            
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);             
                        }//fim vp
                        if(var0=='p' && var1=='v'){   
                            codigo[posicaoVet]=0x03;
                            posicaoVet+=1;
                            if(idx0==1){
                                codigo[posicaoVet]=0x7D;
                                posicaoVet+=1;
                            }else if(idx0==2){
                                codigo[posicaoVet]=0x75; 
                                posicaoVet+=1;
                            }
                            posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx1);;
                        }//fim pv
                    }
                    if(var0=='p' && var1=='$'){
                        codigo[posicaoVet]=0x81;
                        posicaoVet+=1;  
                        if(idx0==1){
                            codigo[posicaoVet]=0xC7;
                            posicaoVet+=1;
                        }else if(idx0==2){
                            codigo[posicaoVet]=0xC6;
                            posicaoVet+=1;
                        }          	
                        for(auxFor = 0; auxFor<4; auxFor++){
                            codigo[posicaoVet] = idx1>>(8*(4-auxFor)) & 0xFF;
                            posicaoVet+=1;
                        }                          
                    }//fim p$
                    if(var0=='v' && var1=='$'){    
                        codigo[posicaoVet]=0x81;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x45;
                        posicaoVet+=1;
                        
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);
                        for(auxFor = 0; auxFor<4; auxFor++){
                            codigo[posicaoVet] = idx1>>(8*(4-auxFor)) & 0xFF;
                            posicaoVet+=1;
                        }
                    }//fim v$
                    if(var0=='v' && var1=='v'){    
                        codigo[posicaoVet]=0x44;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x8b;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x55;
                        posicaoVet+=1;	/*mov var1 para um registrador*/
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx1);;
    
                        codigo[posicaoVet]=0x44;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x01;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x55;
                        posicaoVet+=1;	/*mov o valor do resultado de volta para a posicao de memoria*/
                        
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);
                    }//fim vv   
                            
                }//fim do +
                if(op=='-'){														/* subtracao */
                    if(var0=='p' && var1=='p'){
                        codigo[posicaoVet]=0x29;
                        posicaoVet+=1;
                        if(idx0==1){
                            codigo[posicaoVet]=0xF7;
                            posicaoVet+=1;
                        }else if(idx0==2){
                            codigo[posicaoVet]=0xFE;
                            posicaoVet+=1;
                        }	
                    }//fim pp
                    if((var0=='p' && var1=='v')||(var0=='v' && var1=='p')){
                        if(var0=='v' && var1=='p'){   
                            codigo[posicaoVet]=0x29;
                            posicaoVet+=1;
                            if(idx1==1){
                                codigo[posicaoVet]=0x7D;
                                posicaoVet+=1;
                            }else if(idx1==2){
                                codigo[posicaoVet]=0x75;
                                posicaoVet+=1;
                            }
        
                            
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);               
                        }//fim vp
                        if(var0=='p' && var1=='v'){   
                            codigo[posicaoVet]=0x2b;
                            posicaoVet+=1;
                            if(idx0==1){
                                codigo[posicaoVet]=0x7D;
                                posicaoVet+=1;
                            }else if(idx0==2){
                                codigo[posicaoVet]=0x75;
                                posicaoVet+=1;
                            }
                            
                            posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx1);;
                        }//fim pv
                    }        
                    if(var0=='p' && var1=='$'){
                        codigo[posicaoVet]=0x81;
                        posicaoVet+=1;
                        if(idx0==1){
                                codigo[posicaoVet]=0xEF;
                                posicaoVet+=1;
                            }else if(idx0==2){
                                codigo[posicaoVet]=0xEE;
                                posicaoVet+=1;
                            }
                        
                        for(auxFor = 0; auxFor<4; auxFor++){
                            codigo[posicaoVet] = idx1>>(8*(4-auxFor)) & 0xFF;
                            posicaoVet+=1;
                        }              
                    }//fim p$
                    if(var0=='v' && var1=='$'){    
                        codigo[posicaoVet]=0x81;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x6d;
                        posicaoVet+=1;
                        
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);
                        for(auxFor = 0; auxFor<4; auxFor++){
                            codigo[posicaoVet] = idx1>>(8*(4-auxFor)) & 0xFF;
                            posicaoVet+=1;
                        }
                    }//fim v$
                    if(var0=='v' && var1=='v'){    
                        codigo[posicaoVet]=0x44;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x8b;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x55;
                        posicaoVet+=1;/*mov var1 para um registrador*/
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx1);;
            
                        codigo[posicaoVet]=0x44;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x29;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x55;
                        posicaoVet+=1;/*mov o valor do resultado de volta para a posicao de memoria*/
                        
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);
                    }//fim vv        	
                }//fim do -
                if(op=='*'){
                    if(var0=='p' && var1=='p'){
                    codigo[posicaoVet]=0x0F;
                    posicaoVet+=1;
                    codigo[posicaoVet]=0xAF;
                    posicaoVet+=1;
                    if(idx0==1){
                            codigo[posicaoVet]=0xFE;
                            posicaoVet+=1;
                        }else if(idx0==2){
                            codigo[posicaoVet]=0xF7;
                            posicaoVet+=1;
                        }
                			
                    }
                    if((var0=='p' && var1=='v')||(var0=='v' && var1=='p')){
                        if(var0=='v' && var1=='p'){   
                            codigo[posicaoVet]=0x44;
                            posicaoVet+=1;
                            codigo[posicaoVet]=0x8b;
                            posicaoVet+=1;
                            codigo[posicaoVet]=0x55;
                            posicaoVet+=1; /*mov variavel para registrador*/
                            
                            posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);
                                
                            codigo[posicaoVet]=0x44;
                            posicaoVet+=1;
                            codigo[posicaoVet]=0x0f;
                            posicaoVet+=1;
                            codigo[posicaoVet]=0xaf;
                            posicaoVet+=1;   

                            if(idx1==1){
                                codigo[posicaoVet]=0xD7;
                                posicaoVet+=1;
                            }else if(idx0==2){
                                codigo[posicaoVet]=0xD6;
                                posicaoVet+=1;
                            }                                 
                            
                            codigo[posicaoVet]=0x44;
                            posicaoVet+=1;
                            codigo[posicaoVet]=0x89;
                            posicaoVet+=1;
                            codigo[posicaoVet]=0x55;
                            posicaoVet+=1; /*mov o valor do resultado de volta para a posicao de memoria*/
                            
                            posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);                              
                        }//fim vp
                        if(var0=='p' && var1=='v'){   
                            codigo[posicaoVet]=0x0f;posicaoVet+=1;
                            codigo[posicaoVet]=0xaf;posicaoVet+=1;
                            if(idx0==1){
                                codigo[posicaoVet]=0x7D;
                                posicaoVet+=1;
                            }else if(idx0==2){
                                codigo[posicaoVet]=0x75;
                                posicaoVet+=1;
                            }
                            posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx1);;
                        }
                    }//fim pv        
                    if(var0=='p' && var1=='$'){
                        codigo[posicaoVet]=0x69;
                        posicaoVet+=1;
                        if(idx0==1){
                            codigo[posicaoVet]=0xFF;
                            posicaoVet+=1;
                        }else if(idx0==2){
                            codigo[posicaoVet]=0xF6;
                            posicaoVet+=1;
                        }
                        for(auxFor = 0; auxFor<4; auxFor++){
                            codigo[posicaoVet] = idx1>>(8*(4-auxFor)) & 0xFF;
                            posicaoVet+=1;
                        }                
                    }   //fim p$
                    if(var0=='v' && var1=='$'){    
                        codigo[posicaoVet]=0x44;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x8b;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x55;
                        posicaoVet+=1;
                        
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);
                        
                        codigo[posicaoVet]=0x45;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x69;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0xd2;
                        posicaoVet+=1;
                        for(auxFor = 0; auxFor<4; auxFor++){
                            codigo[posicaoVet] = idx1>>(8*(4-auxFor)) & 0xFF;
                            posicaoVet+=1;
                        }
                                        
                        codigo[posicaoVet]=0x44;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x89;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x55;
                        posicaoVet+=1;/*mov o valor do resultado de volta para a posicao de memoria*/
                        
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0); 
                    }
                    if(var0=='v'&& var1=='v'){
                        codigo[posicaoVet]=0x44;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x8b;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x55;
                        posicaoVet+=1;/*mov var1 para um registrador*/
                        
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);

                        codigo[posicaoVet]=0x44;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x0f;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0xaf;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x55;
                        posicaoVet+=1;
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx1);; 

                        codigo[posicaoVet]=0x44;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x89;
                        posicaoVet+=1;
                        codigo[posicaoVet]=0x55;
                        posicaoVet+=1;/*mov o valor do resultado de volta para a posicao de memoria*/
                        
                        posicaoVet +=posicaoVariavel(codigo, posicaoVet, idx0);
                    } 
                }//fim da *             
                break;                
            }
            }
        line ++;
        fscanf(arquivo_entrada, " ");
    }
   
   /*preenchendo os espaços (00) */
  
        auxDesv = 0;    
        desvio.linhaGo = 0;
        while(codigo[posicaoVet]!=0xc9)/*percorre o vetor ate o fim*/
        {
            if(codigo[posicaoVet]==0xe9){/*ate achar a instrucao do go*/
                auxDesv+=5;/* anda 5 p pegar o endereço da proxima instrucao */
                num = endereco[desvio.desvioGo[desvio.linhaGo]-1]-(long)&codigo[posicaoVet];/* calculando o deslocamento */
                auxDesv-=4; 
                desvio.linhaGo++;              
                while(a<4){
                    codigo[posicaoVet]=pnum[a];
                    posicaoVet+=1; a++;
                }
                a=0;
            }
            posicaoVet+=1;
        }   
        auxDesv = 0;
        desvio2.linhaIfje=0; desvio2.linhaIfjl=0;
        while(codigo[posicaoVet]!=0xc9){
            if(codigo[posicaoVet]==0x8c){/* ou ate achar a instrucao do if*/
                auxDesv+=5;/* anda 5 p pegar o endereço da proxima instrucao */
                num2=endereco[desvio2.desvioIfJl[desvio2.linhaIfjl]-1]-(long)&codigo[posicaoVet];/* calculando o deslocamento */
                auxDesv-=4;/* volta 4 para preencher */

               while(a<4){
                    codigo[posicaoVet]=pnum2[a];
                    posicaoVet+=1; a++;
                }
                a=0;
                desvio2.linhaIfjl++;
            }
            if(codigo[posicaoVet]==0x84){
                auxDesv+=5;/* anda 5 p pegar o endereço da proxima instrucao */
                num2=endereco[desvio2.desvioIfje[desvio2.linhaIfje]-1]-(long)&codigo[posicaoVet];/* calculando o deslocamento */
                auxDesv-=4;/* volta 4 para preencher */
               while(a<4){/* preenche o vetor */
                    codigo[posicaoVet]=pnum2[a];
                    posicaoVet+=1; a++;
                }
                a=0;
                desvio2.linhaIfje++;
            } 
            posicaoVet+=1;
        }   
  cod=(funcp)codigo;
  return cod;

}

