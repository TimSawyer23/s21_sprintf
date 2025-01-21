#include <ctype.h>
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

char *ft_itoa(int n);

int main() {
  char stroka[500];

  s21_sprintf(
      stroka,
      "zopa %+ 100500.50cropa была хрень. %% Теперь снова спека %d dss.", 'g',
      -5203);
   printf("\n%s\n", stroka);

  // printf("блабла бла %10   бла бла");
  return 0;
}

int s21_sprintf(char *str, const char *format, ...) {
  int result = 0;

  state_of_speca speca_state = {0};
  va_list arg_list;
  va_start(arg_list, format);
  result = read_format(str, format, &speca_state, &arg_list);
  if (result) printf("\nАшипка\n");
  // printf("\n%s\n", str);
  va_end(arg_list);
  return result;
}

int read_format(char *str, const char *format, state_of_speca *state,
                va_list *arg_list) {
  int is_alpha = 0;
  int result = 0;
  char *speca = malloc(20 * sizeof(char));
  char *format_work = strdup(format);  // Копируем строку
  if (format_work == NULL) {
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
      //  тут будет вызываться функция обрабатывающая флаг
      //  И выводящая обработанный спецификатор в str
      result = handle_speca(str, speca, state, arg_list);

    } else if ((temp_format[i] == '%') && (temp_format[i + 1] == '%')) {
      // printf("\n%% найден\n");
      strncat(str, temp_format, i);  // Выводим то, что до % в str
      strcat(str, "%");  // Добавляем один символ % в конец str
      temp_format += (i + 2);  // Изменяем временный указатель
      i = -1;                  // Сбрасываем счетчик
    } else if (temp_format[i + 1] == '\0') {
      strncat(str, temp_format, i + 1);
    }
  }
  free(format_work);  // Освобождаем исходную копию
  return result;
}
// Принимает текущий спецификатор и структуру и обрабатывает их
int handle_speca(char *str, char *speca, state_of_speca *state,
                 va_list *arg_list) {
  *state = (state_of_speca){0};
  int result = 0;
  state->specificator = speca[strlen(speca)];  // what is flag baby dont hurt me
  result = what_is_flags(speca, state);
  if (!result) result = what_is_speca(str, speca, state, arg_list);
  // тут будет вызов обработки спецификатора
  return result;
}

int what_is_flags(char *speca,
                  state_of_speca *state) {  // Узнает какие флаги присутствуют,
                                            // ширину и точность
  int result = 0;
  size_t len = strlen(speca);
  if (isdigit(speca[1]))
    if (speca[1] == '0') result = 1;
  for (size_t i = 1; i < len - 1 && !result; i++) {
    if (speca[i] == '-')
      state->minus++;
    else if (speca[i] == '+')
      state->plus++;
    else if (speca[i] == ' ')
      state->space++;
    else if (speca[i] == 'l' && state->longshort != 's')
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
  return result;
}

int what_is_width(char *speca, state_of_speca *state,
                  int i) {  // записывает ширину
  int result;
  while (speca[i] && isdigit(speca[i])) {
    state->width = state->width * 10 + (speca[i] - '0');
    i++;
  }
  result = i - 1;
  state->is_width++;
  return result;
}

int what_is_precision(char *speca, state_of_speca *state,
                      int i) {  // записывает точность
  int result;
  while (speca[i] && isdigit(speca[i])) {
    state->precision = state->precision * 10 + (speca[i] - '0');
    i++;
  }
  state->is_precision++;
  result = i - 1;
  return result;
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
    default:
      result = 5;
      break;
  }
  return result;
}

int speca_is_c(char *str, char *speca, state_of_speca *state,
               va_list *arg_list) {
  int result = 0;
  switch (state->longshort) {
    case 'l':
      // Обработка wchar
      // wchar_t wch = (char)va_arg(*arg_list, int);
      // strcat(str, &wch);
      break;
    case 0:
      char ch = (char)va_arg(*arg_list, int);
      strcat(str, &ch);
      break;
    default:
      result = 5;
      break;
  }

  printf("\n\n\n%s", str);
  return result;
}

int speca_is_d(char *str, char *speca, state_of_speca *state,
               va_list *arg_list) {
  int result = 0;
  switch (state->longshort) {
    case 'l':
      // Обработка wchar
      // wchar_t wch = (char)va_arg(*arg_list, int);
      // strcat(str, &wch);
      break;
    case 0:
      int nbr = (int)va_arg(*arg_list, int);
      char *strnbr = ft_itoa(nbr);
      strcat(str, strnbr);
      free(strnbr);
      break;
    default:
      result = 5;
      break;
  }

  printf("\n\n\n%s", str);
  return result;
}

void handle_negative(int *n, int *is_negative) {
  if (*n < 0) {
    *n = -*n;  // Делаем число положительным
    *is_negative = 1;
  }
}

// Вычисляет длину числа в символах (включая знак и завершающий нуль)
int calculate_length(int n, int is_negative) {
  int length = is_negative ? 2 : 1;  // Учитываем знак и завершающий нуль
  while (n /= 10) length++;  // Считаем количество цифр
  length++;
  return length;
}

// Преобразует число в строку
char *ft_itoa(int n) {
  // Обработка минимального значения int
  if (n == -2147483648) return strdup("-2147483648");

  int is_negative = 0;
  handle_negative(&n, &is_negative);  // Обрабатываем отрицательные числа

  // Вычисляем длину строки
  int length = calculate_length(n, is_negative);

  // Выделяем память для строки
  char *str = (char *)malloc(sizeof(char) * length);
  if (!str) return NULL;  // Проверка на успешное выделение памяти

  // Записываем строку с конца
  str[--length] = '\0';  // Завершающий нулевой символ
  while (n > 0) {
    str[--length] = (n % 10) + '0';  // Преобразуем цифру в символ
    n /= 10;  // Убираем последнюю цифру
  }
    


  // Добавляем знак минуса, если число было отрицательным
  if (is_negative) str[0] = '-';

  return str;
}