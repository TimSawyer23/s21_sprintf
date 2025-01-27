#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int s21_sprintf(char *str, const char *format, ...);

typedef struct s21_sprintf {
  int precision;
  int width;
  short int is_width;
  short int is_precision;
  short int plus;
  short int minus;
  short int space;
  short int zero;
  short int error;
  char specificator;
  char longshort;
} state_of_speca;

int read_format(char *str, const char *format, state_of_speca *state,
                va_list *arg_list);
int handle_speca(char *str, char *speca, state_of_speca *state,
                 va_list *arg_list);
int what_is_type(char *speca, state_of_speca *state);
int what_is_flags(char *speca, state_of_speca *state);
int what_is_width(char *speca, state_of_speca *state, int i);
int what_is_precision(char *speca, state_of_speca *state, int i);
int what_is_speca(char *str, char *speca, state_of_speca *state,
                  va_list *arg_list);
int speca_is_c(char *str, char *speca, state_of_speca *state,
               va_list *arg_list);
int speca_is_d(char *str, char *speca, state_of_speca *state,
               va_list *arg_list);
int speca_is_f(char *str, char *speca, state_of_speca *state,
               va_list *arg_list);

short int handle_overflow(int x);
char *my_itoa(long n, char int_type);
char *handle_flags_d(state_of_speca *state, char *strnbr);
char *float_to_string(float number, int precision);

int main() {
  char stroka[110000];
  char stroka2[110000];
  float ss = 15.21;

  s21_sprintf(stroka, "% .5d. %.6f", 32769, ss);
  printf("%s\n", stroka);

  sprintf(stroka2, "% .5d.", 32769);
  printf("%s\n", stroka2);

  return 0;
}

int s21_sprintf(char *str, const char *format, ...) {
  int result = 0;

  state_of_speca speca_state = {0};
  va_list arg_list;
  va_start(arg_list, format);
  result = read_format(str, format, &speca_state, &arg_list);
  if (result) printf("\nАшипка\n");
  va_end(arg_list);
  return result;
}

int read_format(char *str, const char *format, state_of_speca *state,
                va_list *arg_list) {
  int is_alpha = 0;
  int result = 0;
  char *speca = malloc(20 * sizeof(char));
  if (!speca) return 1;  // Ошибка выделения памяти

  char *format_work = strdup(format);  // Копируем строку
  if (!format_work) {
    free(speca);
    return 1;  // Ошибка выделения памяти
  }

  char *temp_format = format_work;  // Используем временный указатель

  for (long int i = 0; temp_format[i] != '\0' && !result; i++) {
    if ((temp_format[i] == '%') && (temp_format[i + 1] != '%')) {
      strncat(str, temp_format, i);  // Добавляем в str всё, что было до %
      while (!isalpha(temp_format[i + is_alpha]) &&
             (temp_format[i + is_alpha] != '\0'))
        is_alpha++;

      if ((temp_format[i + is_alpha] == 'l') ||
          (temp_format[i + is_alpha] == 'h'))
        is_alpha++;  // Учитываем флаги длины
      is_alpha++;
      memset(speca, 0, 20);
      strncat(speca, temp_format + i,
              is_alpha);  // Добавляем спецификатор в speca
      temp_format += (i + is_alpha);  // Изменяем временный указатель
      i = -1;
      is_alpha = 0;
      result = handle_speca(str, speca, state, arg_list);

    } else if ((temp_format[i] == '%') && (temp_format[i + 1] == '%')) {
      strncat(str, temp_format, i);  // Выводим то, что до % в str
      strcat(str, "%");  // Добавляем один символ % в конец str
      temp_format += (i + 2);  // Изменяем временный указатель
      i = -1;                  // Сбрасываем счетчик
    } else if (temp_format[i + 1] == '\0') {
      strncat(str, temp_format, i + 1);
    }
  }
  free(format_work);  // Освобождаем исходную копию
  free(speca);        // Освобождаем память для speca
  return result;
}

int handle_speca(char *str, char *speca, state_of_speca *state,
                 va_list *arg_list) {
  *state = (state_of_speca){0};
  int result = 0;
  state->specificator =
      speca[strlen(speca) - 1];  // Последний символ - спецификатор
  result = what_is_flags(speca, state);
  if (!result) result = what_is_speca(str, speca, state, arg_list);
  return result;
}

int what_is_flags(char *speca, state_of_speca *state) {
  int result = 0;
  size_t len = strlen(speca);
  for (size_t i = 1; i < len - 1 && !result; i++) {
    if (speca[i] == '-')
      state->minus++;
    else if (speca[i] == '+')
      state->plus++;
    else if (speca[i] == ' ')
      state->space++;
    else if (speca[i] == 'l' && state->longshort != 'h')
      state->longshort = 'l';
    else if (speca[i] == 'h' && state->longshort != 'l')
      state->longshort = 'h';
    else if (isdigit(speca[i]) && !state->is_width && !state->is_precision)
      i = what_is_width(speca, state, i);
    else if (speca[i] == '.' && !state->is_precision) {
      i++;
      i = what_is_precision(speca, state, i);
    } else {
      state->error++;
      result = 1;
    }
  }
  // printf("\nqq%d %dqq\n", state->width, state->precision);
  return result;
}

int what_is_width(char *speca, state_of_speca *state, int i) {
  while (speca[i] && isdigit(speca[i])) {
    state->width = state->width * 10 + (speca[i] - '0');
    i++;
  }
  state->is_width++;
  return i - 1;
}

int what_is_precision(char *speca, state_of_speca *state, int i) {
  while (speca[i] && isdigit(speca[i])) {
    state->precision = state->precision * 10 + (speca[i] - '0');
    i++;
  }
  state->is_precision++;
  return i - 1;
}

int what_is_speca(char *str, char *speca, state_of_speca *state,
                  va_list *arg_list) {
  int result = 0;
  int len = strlen(speca);
  switch (speca[len - 1]) {
    case 'c':
      result = speca_is_c(str, speca, state, arg_list);
      break;
    case 'd':
      result = speca_is_d(str, speca, state, arg_list);
      break;
    case 'f':
      result = speca_is_f(str, speca, state, arg_list);
      break;
    default:
      result = 5;
      break;
  }
  return result;
}

int speca_is_c(char *str, char *speca, state_of_speca *state,
               va_list *arg_list) {
  int result = 0;
  char ch = (char)va_arg(*arg_list, int);
  size_t len = strlen(str);
  str[len] = ch;        // Добавляем символ в строку
  str[len + 1] = '\0';  // Завершаем строку
  return result;
}

int speca_is_d(char *str, char *speca, state_of_speca *state,
               va_list *arg_list) {
  int result = 0;
  char *strnbr = NULL;
  char *str_for_print = NULL;
  switch (state->longshort) {
    case 'l': {
      long lngnbr = (long)va_arg(*arg_list, long);
      strnbr = my_itoa((long)lngnbr, 'l');  // Упрощённая обработка long
      break;
    }
    case 'h': {
      int makeshort = (int)va_arg(*arg_list, int);
      int shrtnbr = handle_overflow(makeshort);
      strnbr = my_itoa((int)shrtnbr, 'h');  // Упрощённая обработка short
      break;
    }
    case 0: {
      int nbr = (int)va_arg(*arg_list, int);
      strnbr = my_itoa(nbr, 'i');
      break;
    }
    default:
      result = 5;
      break;
  }
  if (strnbr) {
    str_for_print = handle_flags_d(state, strnbr);
    strcat(str, str_for_print);
    free(strnbr);
    free(str_for_print);
  }
  return result;
}

char *my_itoa(long n, char int_type) {
  switch (int_type) {
    case 'i':
      if (n == -2147483648) return strdup("-2147483648");
      break;
    case 'l':
      if (n == -9223372036854775807) return strdup("-9223372036854775807");
      break;
    case 'h':
      if (n == -32768) return strdup("-32768");
      break;
    default:
      break;
  }
  int is_negative = 0;
  if (n < 0) {
    is_negative = 1;
    n = -n;
  }

  int length = is_negative ? 2 : 1;
  long temp = n;
  while (temp /= 10) length++;
  length++;

  char *str = (char *)malloc(length * sizeof(char));
  if (!str) return NULL;

  str[--length] = '\0';
  do {
    str[--length] = (n % 10) + '0';
    n /= 10;
  } while (n > 0);

  if (is_negative) str[0] = '-';

  return str;
}

char *handle_flags_d(state_of_speca *state, char *strnbr) {
  int len = strlen(strnbr);
  int result_len = len;
  char *work_strnbr = strnbr;

  if (state->width > result_len) result_len = state->width;
  if (state->precision > result_len) result_len = state->precision;

  char *result = (char *)malloc((result_len + 1) * sizeof(char));
  if (!result) return NULL;
  result[result_len] = '\0';

  memset(result, ' ', result_len);  // Заполняем пробелами

  int start_pos = 0;
  if (state->minus) {
    start_pos = 0;
  } else {
    // Учитываем точность при вычислении start_pos
    start_pos = result_len - (state->precision > len ? state->precision : len);
  }

  if (start_pos > 0 && ((state->plus) || work_strnbr[0] == '-')) {
    start_pos--;
  }

  if (state->plus && (work_strnbr[0] != '-')) {
    result[start_pos] = '+';
    start_pos++;
  }

  if (state->space && (work_strnbr[0] != '-') && !state->plus &&
      state->width < len) {
    result[start_pos] = ' ';
    start_pos++;
  }

  if (work_strnbr[0] == '-') {
    result[start_pos] = '-';
    start_pos++;
    work_strnbr += 1;
    len -= 1;
  }

  if (state->precision > len) {
    int zeros_to_add = state->precision - len;
    memset(result + start_pos, '0', zeros_to_add);
    start_pos += zeros_to_add;
  }

  memcpy(result + start_pos, work_strnbr, len);

  return result;
}

short int handle_overflow(int x) {
  short int result = (short int)(x % 65536);
  if (result > SHRT_MAX) {
    result -= 65536;
  } else if (result < SHRT_MIN) {
    result += 65536;
  }
  return result;
}

int speca_is_f(char *str, char *speca, state_of_speca *state,
               va_list *arg_list) {
  int result = 0;
  float fltnbr = (float)va_arg(*arg_list, double);
  char *strnbr = float_to_string(fltnbr, state->precision);
  printf("\n%s\n", strnbr);
  free(strnbr);
  return result;
}

char *float_to_string(float number, int precision) {
  int integer_part = (int)number;                 // Целая часть
  float fractional_part = number - integer_part;  // Дробная часть

  // Преобразуем целую часть в строку
  char *int_part = my_itoa(integer_part, 'i');
  char *frc_part = malloc((precision + 1) * sizeof(char));
  char *result = malloc((precision + strlen(int_part) + 1) * sizeof(char));

  // Преобразуем дробную часть в строку
  for (int i = 0; i < precision; i++) {
    fractional_part *= 10;
    int digit = (int)fractional_part;
    frc_part[i] = '0' + digit;
    fractional_part -= digit;
  }
  frc_part[precision] = '\0';

  // Соединяем обе части в одну стркоу
  strcat(result, int_part);
  strcat(result, ".");
  strcat(result, frc_part);
  // Чистим всё
  free(int_part);
  free(frc_part);
  return result;
}
