/* добавить вторую функцию, сравнивающую количество букв,
   добавить вызов различных функций в зависимости от аргумента программы
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void number_check(char *a, char *b,
           int (*cmp)(const char *, const char *))
{
  printf("проверка количества символов.\n");
  
  if((*cmp)(a, b))
  {
    printf("Равны\n");
  } 
  else{
    printf("Не равны\n");
  }
}

void check(char *a, char *b,
           int (*cmp)(const char *, const char *))
{
  printf("Проверка на совпадение.\n");
  if(!(*cmp)(a, b))
  {
    printf("Равны\n");
  } 
  else{
    printf("Не равны\n");
  }
}

int main(void)
{
  char s1[80], s2[80];
  char argument[80];
  /* указатель на функцию */
  int (*p)(const char *, const char *);
  int (*f)(const char *, const char *);
  //int  * p()  - без скобок будет просто функция возвращающая целое

  p = strcmp;
  f = strlen;
  /* присваивает адрес функции strcmp указателю p */

  printf("Введите две строки.\n");
  gets(s1);
  gets(s2);
  gets(argument);
  if (strcmp(argument, "количество") == 1){
    check(s1, s2, p); /* Передает адрес функции strcmp
                       посредством указателя p */
  }
  else{
    number_check(s1, s2, f);
  }
  return 0;
}
