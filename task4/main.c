#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "args.h"
#include "file_io.h"
#include "status.h"

static void print_usage(void)
{
    fprintf(stderr,
            "Использование:\n"
            "  program -X <входной файл>                  результат в out_<имя>\n"
            "  program -nX <входной файл> <выходной файл>\n"
            "где X:\n"
            "  d  исключить арабские цифры\n"
            "  i  для каждой строки - количество латинских букв\n"
            "  s  для каждой строки - количество символов, кроме латинских\n"
            "     букв, арабских цифр и пробела\n"
            "  a  символы, кроме цифр, заменить hex-кодом ASCII\n"
            "Вместо '-' можно использовать '/'.\n");
}

static void print_error(const char *context, const Status status)
{
    fprintf(stderr, "Ошибка: %s: %s\n", context, status_message(status));
}

static Status convert_file(const char *input_path, const char *output_path, const Action action)
{
    bool same = false;
    Status status = paths_look_same(input_path, output_path, &same);
    if (status != STATUS_OK) {
        print_error("сравнение путей", status);
        return status;
    }
    if (same) {
        print_error("пути", STATUS_ERR_SAME_FILE);
        return STATUS_ERR_SAME_FILE;
    }

    status = process_file(input_path, output_path, action);
    if (status != STATUS_OK) {
        print_error("обработка файла", status);
    }
    return status;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Ошибка: не указан флаг\n");
        print_usage();
        return 1;
    }

    Flag flag;
    Status status = parse_flag(argv[1], &flag);
    if (status != STATUS_OK) {
        fprintf(stderr, "Ошибка в флаге \"%s\": %s\n", argv[1], status_message(status));
        print_usage();
        return 1;
    }

    int needed = 0;
    status = required_argc(&flag, &needed);
    if (status != STATUS_OK) {
        print_error("флаг", status);
        return 1;
    }
    if (argc != needed) {
        fprintf(stderr, "Ошибка: для флага %s нужно путей к файлам: %d, передано: %d\n",
                argv[1], needed - 2, argc - 2);
        print_usage();
        return 1;
    }

    status = validate_path(argv[2]);
    if (status != STATUS_OK) {
        print_error("путь к входному файлу", status);
        return 1;
    }

    char *owned_path = NULL;
    const char *output_path = NULL;

    if (flag.has_output_path) {
        status = validate_path(argv[3]);
        if (status != STATUS_OK) {
            print_error("путь к выходному файлу", status);
            return 1;
        }
        output_path = argv[3];
    } else {
        status = build_output_path(argv[2], &owned_path);
        if (status != STATUS_OK) {
            print_error("построение имени выходного файла", status);
            return 1;
        }
        output_path = owned_path;
    }

    status = convert_file(argv[2], output_path, flag.action);

    free(owned_path);

    if (status != STATUS_OK) {
        return 1;
    }
    return 0;
}