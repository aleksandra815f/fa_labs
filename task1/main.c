#include <stdio.h>
 
#include "logic.h"
#include "parser.h"
#include "status.h"
 
static void print_error(Status status){
    switch (status) {
        case STATUS_OK:
            break;
        case STATUS_NULL:
            fprintf(stderr, "Внутренняя ошибка: передан пустой указатель.\n");
            break;
        case STATUS_BAD_ARGC:
            fprintf(stderr, "Ошибка: нужно два аргумента. Пример: ./task1 10 -h\n");
            break;
        case STATUS_BAD_FLAG:
            fprintf(stderr, "Ошибка: флаг должен быть одним из: -h -p -s -e -a -f (или со знаком /)\n");
            break;
        case STATUS_BAD_NUMBER:
            fprintf(stderr, "Ошибка: x должно быть целым числом\n");
            break;
        case STATUS_TOO_BIG:
            fprintf(stderr, "Ошибка: x слишком большое по модулю\n");
            break;
        case STATUS_NEGATIVE:
            fprintf(stderr, "Ошибка: для этого флага x не может быть отрицательным\n");
            break;
        case STATUS_OUT_OF_RANGE:
            fprintf(stderr, "Ошибка: для флага -e число x должно быть от 1 до 10\n");
            break;
        case STATUS_OVERFLOW:
            fprintf(stderr, "Ошибка: результат не помещается в тип long long\n");
            break;
        default:
            fprintf(stderr, "Неизвестная ошибка.\n");
            break;
    }
}
 
static void print_multiples(const long long *numbers, int count, long long x)
{
    if (count == 0) {
        printf("Натуральных чисел от 1 до 100, кратных %lld, нет.\n", x);
        return;
    }
    printf("Числа от 1 до 100, кратные %lld:", x);
    for (int i = 0; i < count; i++) {
        printf(" %lld", numbers[i]);
    }
    printf("\n");
}
 
static void print_kind(NumberKind kind, long long x)
{
    if (kind == NUMBER_PRIME) {
        printf("%lld - простое число.\n", x);
    } else if (kind == NUMBER_COMPOSITE) {
        printf("%lld - составное число.\n", x);
    } else {
        printf("%lld - ни простое, ни составное.\n", x);
    }
}
 
static void print_hex(const int *digits, int count)
{
    const char symbols[] = "0123456789ABCDEF";
    for (int i = 0; i < count; i++) {
        if (i > 0) {
            printf(" ");
        }
        printf("%c", symbols[digits[i]]);
    }
    printf("\n");
}
 
static void print_table(const long long *table, int cols)
{
    for (int base = 1; base <= TABLE_SIZE; base++) {
        printf("%2d:", base);
        for (int exp = 1; exp <= cols; exp++) {
            printf(" %lld", table[(base - 1) * TABLE_SIZE + (exp - 1)]);
        }
        printf("\n");
    }
}
 
int main(int argc, char *argv[])
{
    if (argc != 3) {
        print_error(STATUS_BAD_ARGC);
        return 1;
    }
 
    long long x = 0;
    Flag flag = FLAG_H;
 
    Status status = parse_number(argv[1], &x);
    if (status == STATUS_OK) {
        status = parse_flag(argv[2], &flag);
    }
    if (status != STATUS_OK) {
        print_error(status);
        return 1;
    }
 
    switch (flag) {
        case FLAG_H: {
            long long numbers[MAX_MULTIPLES];
            int count = 0;
            status = find_multiples(x, numbers, &count);
            if (status == STATUS_OK) {
                print_multiples(numbers, count, x);
            }
            break;
        }
        case FLAG_P: {
            NumberKind kind = NUMBER_NEITHER;
            status = check_prime(x, &kind);
            if (status == STATUS_OK) {
                print_kind(kind, x);
            }
            break;
        }
        case FLAG_S: {
            int digits[MAX_HEX_DIGITS];
            int count = 0;
            status = get_hex_digits(x, digits, &count);
            if (status == STATUS_OK) {
                print_hex(digits, count);
            }
            break;
        }
        case FLAG_E: {
            long long table[TABLE_SIZE * TABLE_SIZE];
            int cols = 0;
            status = make_power_table(x, table, &cols);
            if (status == STATUS_OK) {
                print_table(table, cols);
            }
            break;
        }
        case FLAG_A: {
            long long sum = 0;
            status = sum_to_x(x, &sum);
            if (status == STATUS_OK) {
                printf("Сумма чисел от 1 до %lld равна %lld.\n", x, sum);
            }
            break;
        }
        case FLAG_F: {
            long long result = 0;
            status = factorial(x, &result);
            if (status == STATUS_OK) {
                printf("%lld! = %lld\n", x, result);
            }
            break;
        }
    }
 
    if (status != STATUS_OK) {
        print_error(status);
        return 1;
    }
    return 0;
}
