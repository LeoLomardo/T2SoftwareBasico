# T2SoftwareBasico
 Trabalho 2 - Software Básico (INF1018)

Trabalho realizado pelos alunos Leo Lomardo e Lucas Lucena. O objetivo do trabalho é criar um mini compilador para a linguagem fictícia "LB".

A Linguagem SB
Na linguagem SB, as variáveis locais são da forma vi, sendo o índice i utilizado para identificar a variável (ex. v1, v2, etc...). A linguagem permite o uso de no máximo 4 variáveis locais.

Da mesma forma, os parâmetros são denotados por pi, e podem ser usados no máximo 2 parâmetros (p1 e p2).

Constantes são escritas na forma $i, onde i é um valor inteiro, com um sinal opcional. Por exemplo, $10 representa o valor 10 e $-10 representa o valor -10.

Funções na linguagem SB contém atribuições, operações aritméticas, instruções de desvio e de retorno.

Uma atribuição tem a forma varp ':=' varpc onde var é uma variável local ou um parâmetro e varpc é uma variável local, um parâmetro ou uma constante inteira. Como exemplo, se temos

v1 := p1
v2 := $1

o valor do parâmetro p1 será armazenado na variável local v1 e o valor inteiro 1 será armazenado na variável local v2.

Uma operação aritmética tem a forma varp op varpc onde varp é uma variável local ou um parâmetro, varpc é uma variável local, um parâmetro ou uma constante inteira, e op é um dos operadores '+' '-' '*' seguido de '='. Como exemplo, se temos

v1 *= p1 
p1 += $1

o resultado da operação v1 * p1 será armazenado na variável local v1 e o valor do parâmetro p1 será incrementado.

A instrução de desvio condicional tem a forma 'if' varp n1 n2 onde varp é uma variável local ou um parâmetro, e n1 e n2 são números de linhas no código fonte. A semântica dessa instrução é a seguinte:

se o valor da variável local ou parâmetro é menor que 0, será executado um desvio para a instrução que está na linha n1;
se o valor da variável local ou parâmetro é igual a 0, será executado um desvio para a instrução que está na linha n2;
se o valor da variável local ou parâmetro é maior que 0, não há desvio e a execução segue normalmente para a instrução da linha seguinte.

A instrução de desvio incondicional tem a forma 'go' n onde n é o número da linha no código fonte para onde o controle deverá ser desviado.

Finalmente, a instrução de retorno tem a forma 'ret' varpc Neste caso, a função deverá retornar, e seu valor de retorno é o valor da variável local, parâmetro ou constante inteira indicada.

A sintaxe da linguagem SB pode ser definida formalmente como abaixo. Note que as cadeias entre ' ' são símbolos terminais da linguagem: os caracteres ' não aparecem nos comandos!

func	::=	cmd '\n' | cmd '\n' func
cmd	::=	att | expr | dif | dgo | ret
att	::=	varp ':=' varpc
expr	::=	varp op varpc
varp	::=	'v' num | 'p' num
varpc	::=	varp | '$' snum
op	::=	'+=' | '-=' | '*='
ret	::=	'ret' varpc
dif	::=	'if' varp num num
dgo	::=	'go' num
num	::=	digito | digito num
snum	::=	[-] num
digito	::=	0' | '1' | '2' | '3' | '4' | '5' | '6' | '7'| '8' | '9'
