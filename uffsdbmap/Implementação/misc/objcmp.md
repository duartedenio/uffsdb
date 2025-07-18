
# Percorre o vetor

Percorre o vetor até o final da menor string, comparando caracteres em minúsculo. Se encontrar caracteres diferentes, retorna a diferença entre eles. Caso contrário, retorna a diferença do último caractere.

```C
int i;
char a,b;
for(i = 0; obj[i] && str[i]; i++){
	a = tolower(obj[i]), b = tolower(str[i]);
	if(a != b) return a-b;
}
return tolower(obj[i])-tolower(str[i]);
```