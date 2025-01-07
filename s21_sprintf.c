#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int s21_sprintf(char *str, const char *format, ...);

typedef struct s21_sprintf {
  short int tochnost;
  short int width;
  short int plus;
  short int minus;
  short int space;
  short int zero;
  char specificator;
} state_of_speca;

int what_is_type(char *speca, state_of_speca *state);
int what_is_flags();

int main() {
  printf("%010.5d", 135);
  return 0;
}

int s21_sprintf(char *str, const char *format, ...) {
  int result = 0;  // Развращаем результат в конце. Если 0, то всё норм
  char temp_speca[20];  // спецификация с опциями например %4.6d
  char *format_work = strdub(format);  // копируем строку, что бы с ней работать

  state_of_speca speca_state = {0};  // Структура с хранением опций спецификации
  va_list arg_list;  // Храним переменные аргументы тут
  va_start(arg_list, format);  // Сохраняем все переменные аргументы в arg_list,
                               // которые идут после аргумента format

  read_format(&str, &format_work, &temp_speca);
  handle_speca(&temp_speca, &speca_state);  // Обрабатываем текущий спецификатор

  va_end(arg_list);
  free(format_work);
  return result;
}

// Получаем спецификатор в temp_speca, добавляем в str всё, что было до
// спецификатора, обрезаем строку до конца спецификатора
int read_format(char *str, char *format, char *speca) {
  int spec_finded = 0;
  int is_alpha = 0;
  for (size_t i = 0; i < strlen(format); i++) {
    if ((format[i] == '%') && (format[i + 1] != '%')) {
      strncat(str, format, i - 1);  // добавляем в str всё, что было до %
      while (!isalpha(format[i])) is_alpha++;
      strncat(speca, format + i,
              is_alpha);  // добавляем спецификатор в speca
      format += (i + is_alpha);
      spec_finded = 1;
    } else {
      // добавить символ % в конец str
      i++;
    }
  }
}

// Принимает текущий спецификатор и структуру и обрабатывает их
int handle_speca(char *speca, state_of_speca *state) {
  int result = 0;
  state->specificator = speca[strlen(speca)];
  what_is_flags;
  return result;
}

int what_is_type(char *speca, state_of_speca *state) {
  int result = 0;
  state->specificator = speca[strlen(speca)];
  return result;
}

int what_is_flags() {
  int result = 0;
  return result;
}