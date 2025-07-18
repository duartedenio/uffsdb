
# Percorre o vetor

Percorre a string `src`, convertendo cada caractere para minúsculo com a função `tolower`. O resultado é armazenado na string `dest`, que é finalizada com o caractere nulo (`\0`).

```C
void strcpylower(char *dest, char *src) {
	int i;
	for(i = 0; src[i]; i++) dest[i] = tolower(src[i]);
	dest[i] = '\0';
}
```