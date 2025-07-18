o misc.c possui funções para tratamento de texto como 
- [[#objcmp]] - comparar string's case unsensitive;
- [[#strcpylower]] - transformar a string em caixa baixa;

# objcmp
#string

A função objcmp compara duas strings de forma "case insensitive".

---
## Assinatura

```C
int objcmp(char *obj, char *str) 
```
## Parâmetro(s) / Retorno

* obj = uma string para ser comparada,
* str = uma string para ser comparada.

Retorna a diferença entre a primeira string com a segunda.

----
## Fluxo

1. **Percorre a string (case-insensitive)**: caso a diferença seja diferente de 0, interrompe o loop, retornando a diferença.
---
## Dependências

- Função Dependente: [[database#createDB|createDB]].
- Função Auxiliar: **`Não Possui`**

---
## Implementação

A implementação está em [[objcmp]].

---
# strcpylower

a função strcpylower modifica o parâmetro `dest` para armazenar a string.

---
## Assinatura

```C
void strcpylower(char *dest, char *src)
```
### Parâmetro(s) / Retorno

* dest = uma string para servir de retorno.
* str = uma string para converter em caixa baixa.

Retorna implícito por parâmetro.

Obs: achei estranho o retorno ser o 1° parâmetro, geralmente é o ultimo.

----
## Fluxo

1. **Percorre a string src**, preenchendo a dest com sua versão em caixa baixa:
---
## Dependências

- Função Dependente: [[database#createDB|createDB]].
- Função Auxiliar: **`Não Possui`**
---
## Implementação

A implementação está em [[strcpylower]].