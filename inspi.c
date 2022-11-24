/* JESSICA PEREIRA CESARIO 1711179 3WA */
/* NOTA: 9,5 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "geracodigo.h"

static unsigned char *codigo;
static int i;


void liberacod(void *pf){
	free(pf);																														/*-- FUNCAO LIBERA --*/
	return;																							/* libera a memoria alocada para a criacao do vetor codigo */
}


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

  fprintf(stderr, "erro %s na linha %d\n", msg, line);											/*-- FUNCAO ERRO --*/
  exit(EXIT_FAILURE);
}
void posVariavel(int val){
	switch(val){
                case 1:{
                            codigo[i]=0xf0;i++;
                            break;}
                case 2:{																								         /*-- FUNÇAO posVariavel --*/
                            codigo[i]=0xf4;i++;
                            break;}													/* recebe o numero que corresponde a uma variavel e coloca na posicao */
                case 3:{																		              /* o byte correspondente no vetor  */
                            codigo[i]=0xf8;i++;
                            break;}
                case 4:{
                            codigo[i]=0xfc;i++;
                            break;}           
            } 
	return;
}
void operacao(char var0, int idx0){
    if(var0=='p'){
            switch(idx0){   
                    case 1:{																										/*-- FUNÇAO operacao --*/
                                codigo[i]=0xff;i++;
                                codigo[i]=0x00;i++;					/*recebe a variavel e o numero corespondente a mesma, identifica e coloca */
                                break;}																  /* o byte correspondente no vetor na posicao */
                    case 2:{
                                codigo[i]=0xfe;i++;
                                codigo[i]=0x00;i++;
                                break;}               
                }
            } 
        if(var0=='v'){
                    codigo[i]=0x7d;i++;
                    posVariavel(idx0);
                    codigo[i]=0x00;i++;
                }
}


funcp geracod (FILE *f){		

  funcp cod;
  int a=0, line=1;
  char c;
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
  
  codigo=(unsigned char*)malloc(110*sizeof(unsigned char));
  codigo[0]=0x55;codigo[1]=0x48;codigo[2]=0x89;codigo[3]=0xe5;					/* criacao da  pilha */
  codigo[4]=0x48;codigo[5]=0x83;codigo[6]=0xec;codigo[7]=0x10;
  i=8;
  
  while ((c = fgetc(f)) != EOF){
  
    endereco[linha]=(long)(&codigo[i]); /* salvando os endereços das linhas no vetor*/
    linha++;
    
    switch (c){
      case 'i': { 																								 /* desvio condicional */
        char var0;
        int idx0, n1, n2;
        
        if (fscanf(f, "f %c%d %d %d", &var0, &idx0, &n1, &n2) != 4)
            error("comando invalido", line);
        codigo[i]=0x83;i++;
        operacao(var0,idx0);
        
        codigo[i]=0x0f; i++; codigo[i]=0x8c; i++; /*jl lable*/
        codigo[i]=0x00; i++; codigo[i]=0x00; i++;
        codigo[i]=0x00; i++; codigo[i]=0x00; i++;
        desvio2.desvioIfJl[desvio2.linhaIfjl]=n1;
        desvio2.linhaIfjl++;
        
				codigo[i]=0x0f; i++; codigo[i]=0x84; i++; /*je lable*/
        codigo[i]=0x00; i++; codigo[i]=0x00; i++;
        codigo[i]=0x00; i++; codigo[i]=0x00; i++;
        desvio2.desvioIfje[desvio2.linhaIfje]=n2;
        desvio2.linhaIfje++;
        printf("%d if %c%d %d %d\n", line, var0, idx0, n1, n2);
        break;
      }
      case 'g': {																								/* desvio incondicional */
        int n; 
        if (fscanf(f, "o %d", &n) != 1)
          error("Comando invalido", line);
		codigo[i]=0xe9; i++; codigo[i]=0x00; i++;
		codigo[i]=0x00; i++; codigo[i]=0x00; i++;
		codigo[i]=0x00; i++;
		desvio.desvioGo[desvio.linhaGo]=n; desvio.linhaGo++;
		printf("%d go %d\n", line, n);
        break;
      }
      case 'r':{																								/* retorno */
        char var0;int idx0;
        
        if (fscanf(f, "et %c%d", &var0, &idx0) != 2)
            error("comando invalido", line);
        if(var0=='p'){
            codigo[i]=0x89;i++;
            switch(idx0){   
                case 1:{
                    codigo[i]=0xf8;i++;
                    break;}
                case 2:{
                    codigo[i]=0xf0;i++;
                    break;}               
            }
        }
        if(var0=='v'){         
            codigo[i]=0x8b; i++;
		    		codigo[i]=0x45; i++;
            posVariavel(idx0);         
        }
        if(var0=='$'){   
            codigo[i]=0xb8;i++;
            *((int *)&codigo[i])=idx0;/*essa linha coloca o numero em hexa, da forma little endian, no vetor*/
            i+=4;/*soma 4 pois um int tem esse tamanho*/
				}
				codigo[i]=0xc9; i++;		/*leave*/
        codigo[i]=0xc3; i++;		/*ret*/
        printf("%d ret %c%d\n", line, var0, idx0);
        break;
        }//fim do case r
        
			case 'v':
      case 'p': { 																			/* atribuicao */
        char var0 = c, var1, op;
        int idx0, idx1;
        
        if (fscanf(f, "%d %c= %c%d", &idx0, &op, &var1, &idx1) != 4)
            error("comando invalido", line);
        printf("%d %c%d %c= %c%d\n", line, var0, idx0, op, var1, idx1);
        if(op==':'){
            if(var0=='p' && var1=='p'){
                codigo[i]=0x89;i++;
                switch(idx0){
                    case 1:{
                                codigo[i]=0xf7;i++;
                                break;}
                    case 2:{
                                codigo[i]=0xfe;i++;
                                break;}                   
                }               
            }//fim pp
            if((var0=='p' && var1=='v')||(var0=='v' && var1=='p')){
                if(var0=='v' && var1=='p'){   
                    codigo[i]=0x89;i++;
                    switch(idx1){
                        case 1:{
                                    codigo[i]=0x7d;i++;
                                    break;}
                        case 2:{
                                    codigo[i]=0x75;i++;
                                    break;}                                        
                    }
                    posVariavel(idx0);          
                }//fim vp
                if(var0=='p' && var1=='v'){   
                    codigo[i]=0x8b;i++;
                    switch(idx0){
                        case 1:{
                                    codigo[i]=0x7d; i++; break;}
                        case 2:{
                                    codigo[i]=0x75; i++; break;}                                     
                    }
                    posVariavel(idx1);
                }
            }//fim pv        
            if(var0=='p' && var1=='$'){
                switch(idx0){
                    case 1:{
                        codigo[i]=0xbf;i++; break;}
                    case 2:{
                        codigo[i]=0xbe;i++; break;}               
                }
                *( (int *) &codigo[i] ) = idx1;		/* achei esse 'truque' na internet, coloca o int na posicao certa */
                i+=4;                							/* soma o numero correspondente ao tamanho do int */
           }//fim p$
           if(var0=='v' && var1=='$'){    
                codigo[i]=0xc7;i++;
                codigo[i]=0x45;i++;
                posVariavel(idx0);
                *( (int *) &codigo[i] ) = idx1;
                i+=4;
           }//fim v$
           if(var0=='v'&& var1=='v')
           {
							codigo[i]=0x44;i++;
           		codigo[i]=0x8b;i++;
							codigo[i]=0x55;i++;/*mov var1 para um registrador*/
							posVariavel(idx1);
				
							codigo[i]=0x44;i++;
							codigo[i]=0x89;i++;
							codigo[i]=0x55;i++;/*mov %ebx, (%rbp)*/
							posVariavel(idx0);		
				
           }//fim vv 
        }//fim do :   
        if(op=='+'){												/* soma */
			if(var0=='p' && var1=='p'){
				codigo[i]=0x01;i++;
					switch(idx0){
							case 1:{
													codigo[i]=0xf7; i++; break;}
              case 2:{
               	          codigo[i]=0xfe; i++; break;}                       
                }			
			}//fim pp
			if((var0=='p' && var1=='v')||(var0=='v' && var1=='p')){
                if(var0=='v' && var1=='p'){   
                    codigo[i]=0x01;i++;
                    switch(idx1){
                        case 1:{
                                    codigo[i]=0x7d;i++; break;}
                        case 2:{
                                    codigo[i]=0x75;i++; break;}               
                    }
                    posVariavel(idx0);             
                }//fim vp
                if(var0=='p' && var1=='v'){   
                    codigo[i]=0x03;i++;
                    switch(idx0){
                        case 1:{
                                    codigo[i]=0x7d;i++; break;}
                        case 2:{
                                    codigo[i]=0x75;i++; break;}
                    }
                    posVariavel(idx1);
                }//fim pv
            }
            if(var0=='p' && var1=='$'){
            	codigo[i]=0x81;i++;            	
                switch(idx0){
                    case 1:{
                                 codigo[i]=0xc7;i++; break;}
                    case 2:{
                                 codigo[i]=0xc6;i++; break;}            
                 }
                *((int *)&codigo[i])=idx1;
                i+=4;                           
            }//fim p$
            if(var0=='v' && var1=='$'){    
                codigo[i]=0x81;i++;
                codigo[i]=0x45;i++;
                posVariavel(idx0);
                *((int *)&codigo[i])=idx1;
                i+=4;
            }//fim v$
            if(var0=='v' && var1=='v'){    
							codigo[i]=0x44;i++;
              codigo[i]=0x8b;i++;
              codigo[i]=0x55;i++;	/*mov var1 para um registrador*/
							posVariavel(idx1);
				
							codigo[i]=0x44;i++;
							codigo[i]=0x01;i++;
							codigo[i]=0x55;i++;	/*mov o valor do resultado de volta para a posicao de memoria*/
							posVariavel(idx0);
         }//fim vv   
                  	
        }//fim do +
        if(op=='-'){														/* subtracao */
					if(var0=='p' && var1=='p'){
						codigo[i]=0x29;i++;
               	switch(idx0){
               	    case 1:{
               	                codigo[i]=0xf7; i++; break;}
               	    case 2:{
               	                codigo[i]=0xfe; i++; break;}            
                }			
					}//fim pp
				if((var0=='p' && var1=='v')||(var0=='v' && var1=='p')){
                if(var0=='v' && var1=='p'){   
                    codigo[i]=0x29;i++;
                    switch(idx1){
                        case 1:{
                                    codigo[i]=0x7d; i++; break;}
                        case 2:{
                                    codigo[i]=0x75; i++; break;}                
                    }
                    posVariavel(idx0);               
                }//fim vp
                if(var0=='p' && var1=='v'){   
                    codigo[i]=0x2b;i++;
                    switch(idx0){
                        case 1:{
                                    codigo[i]=0x7d; i++; break;}
                        case 2:{
                                    codigo[i]=0x75; i++; break;}                
                    }
                    posVariavel(idx1);
                }//fim pv
            }        
            if(var0=='p' && var1=='$'){
            	codigo[i]=0x81;i++;
                switch(idx0){
                    case 1:{
                        codigo[i]=0xef; i++; break;}
                    case 2:{
                        codigo[i]=0xee; i++; break;}                 
                }
                *( (int *) &codigo[i] ) = idx1;
                i+=4;                
           }//fim p$
           if(var0=='v' && var1=='$'){    
                codigo[i]=0x81;i++;
                codigo[i]=0x6d;i++;
                posVariavel(idx0);
                *((int *)&codigo[i])=idx1;
                i+=4;
           }//fim v$
         if(var0=='v' && var1=='v'){    
							codigo[i]=0x44;i++;
              codigo[i]=0x8b;i++;
              codigo[i]=0x55;i++;/*mov var1 para um registrador*/
							posVariavel(idx1);
				
							codigo[i]=0x44;i++;
							codigo[i]=0x29;i++;
							codigo[i]=0x55;i++;/*mov o valor do resultado de volta para a posicao de memoria*/
							posVariavel(idx0);
         }//fim vv        	
        }//fim do -
        if(op=='*'){
					if(var0=='p' && var1=='p'){
						codigo[i]=0x0f;i++;
						codigo[i]=0xaf;i++;
               	switch(idx0){
               	    case 1:{
               	                codigo[i]=0xfe;i++; break;}
               	    case 2:{
               	                codigo[i]=0xf7;i++; break;}                     
       			   }			
					}
				if((var0=='p' && var1=='v')||(var0=='v' && var1=='p')){
                if(var0=='v' && var1=='p'){   
										codigo[i]=0x44;i++;
                    codigo[i]=0x8b;i++;
                    codigo[i]=0x55;i++; /*mov variavel para registrador*/
                    posVariavel(idx0);
                    	
										codigo[i]=0x44;i++;
                    codigo[i]=0x0f;i++;
                    codigo[i]=0xaf;i++;                                        
                    switch(idx1){
                        case 1:{
                                    codigo[i]=0xd7;i++;break;}
                        case 2:{
                                    codigo[i]=0xd6;i++;break;}                                        
                    }
										codigo[i]=0x44;i++;
                    codigo[i]=0x89;i++;
                    codigo[i]=0x55;i++; /*mov o valor do resultado de volta para a posicao de memoria*/
                    posVariavel(idx0);                              
                }//fim vp
                if(var0=='p' && var1=='v'){   
                    codigo[i]=0x0f;i++;
                    codigo[i]=0xaf;i++;
              
                    switch(idx0){
                        case 1:{
                                    codigo[i]=0x7d;i++;break;}
                        case 2:{
                                    codigo[i]=0x75;i++;break;}                                       
                    }
                    posVariavel(idx1);
                }
            }//fim pv        
            if(var0=='p' && var1=='$'){
            	codigo[i]=0x69;i++;
                switch(idx0){
                    case 1:{
                        codigo[i]=0xff;i++; break;}
                    case 2:{
                        codigo[i]=0xf6;i++; break;}                 
                }
                *( (int *) &codigo[i] ) = idx1;
                i+=4;                
           }//fim p$
           if(var0=='v' && var1=='$'){    
								codigo[i]=0x44;i++;
                codigo[i]=0x8b;i++;
                codigo[i]=0x55;i++;
                posVariavel(idx0);
				
								codigo[i]=0x45;i++;
								codigo[i]=0x69;i++;
                codigo[i]=0xd2;i++;
                *( (int *) &codigo[i] ) = idx1;
                i+=4;
								
								codigo[i]=0x44;i++;
								codigo[i]=0x89;i++;
                codigo[i]=0x55;i++;/*mov o valor do resultado de volta para a posicao de memoria*/
                posVariavel(idx0); 
           }
         if(var0=='v'&& var1=='v'){
        		codigo[i]=0x44;i++;
            codigo[i]=0x8b;i++;
            codigo[i]=0x55;i++;/*mov var1 para um registrador*/
						posVariavel(idx0);
		
						codigo[i]=0x44;i++;
						codigo[i]=0x0f;i++;
            codigo[i]=0xaf;i++;
						codigo[i]=0x55;i++;
            posVariavel(idx1); 

						codigo[i]=0x44;i++;
						codigo[i]=0x89;i++;
						codigo[i]=0x55;i++;/*mov o valor do resultado de volta para a posicao de memoria*/
						posVariavel(idx0);
         } 
        }//fim da *             
        break;                
        }
     }
     line ++;
     fscanf(f, " ");
   }
   
   /*preenchendo os espaços (00) */
  
        i=0;    
        desvio.linhaGo=0;
        while(codigo[i]!=0xc9)/*percorre o vetor ate o fim*/
        {
            if(codigo[i]==0xe9){/*ate achar a instrucao do go*/
                i+=5;/* anda 5 p pegar o endereço da proxima instrucao */
                num=endereco[desvio.desvioGo[desvio.linhaGo]-1]-(long)&codigo[i];/* calculando o deslocamento */
                i-=4; desvio.linhaGo++;              
                while(a<4){
									codigo[i]=pnum[a];
									i++; a++;
								}
                a=0;
            }
            i++;
        }   
        i=0;
        desvio2.linhaIfje=0; desvio2.linhaIfjl=0;
        while(codigo[i]!=0xc9){
            if(codigo[i]==0x8c){/* ou ate achar a instrucao do if*/
                i+=5;/* anda 5 p pegar o endereço da proxima instrucao */
                num2=endereco[desvio2.desvioIfJl[desvio2.linhaIfjl]-1]-(long)&codigo[i];/* calculando o deslocamento */
                i-=4;/* volta 4 para preencher */

               while(a<4){
									codigo[i]=pnum2[a];
									i++; a++;
								}
                a=0;
                desvio2.linhaIfjl++;
            }
            if(codigo[i]==0x84){
                i+=5;/* anda 5 p pegar o endereço da proxima instrucao */
                num2=endereco[desvio2.desvioIfje[desvio2.linhaIfje]-1]-(long)&codigo[i];/* calculando o deslocamento */
                i-=4;/* volta 4 para preencher */
               while(a<4){/* preenche o vetor */
									codigo[i]=pnum2[a];
									i++; a++;
								}
                a=0;
                desvio2.linhaIfje++;
            } 
            i++;
        }   
  cod=(funcp)codigo;
  return cod;
}