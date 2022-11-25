#include <stdio.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef int (*funcp) (int x);
int escreverVet(unsigned char *codigo, int posicaoVet, unsigned char novoCod[], int tamanhoAdicional);
int posicaoVariavel(unsigned char *codigo, int posicaoVet, int val );
funcp geraCodigo(FILE *f, unsigned char *codigo);
int operacao(unsigned char *codigo, int posicaoVet, char var0, int idx0);
static void error (const char *msg, int line);
