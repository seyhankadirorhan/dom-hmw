#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Sistem komutlarını çalıştırmak için
void run_choices(const char *choice) 
{
    int IsTrue = system(choice);
    if (IsTrue != 0) 
    {
        fprintf(stderr, "Komut çaliştirilamadi veya hata oluştu: %s\n", choice);
        exit(EXIT_FAILURE);
    }
}

void print_help() 
{
    printf(
        "# Usage:\n"
        "./2021510107 <input_file> <output_file> <conversion_type> -separator <1|2|3> -opsys <1|2|3> [-h]\n\n"
        "# Arguments:\n"
        "<input_file>      = Source file to read from\n"
        "<output_file>     = Target file to write to (or the XSD file for validation)\n"
        "<conversion_type> =\n"
        "  1 → CSV to Binary\n"
        "  2 → Binary to XML (reads binary file name from setupParams.json)\n"
        "  3 → XML validation with XSD\n"
        "-separator <P1>   = Required field separator (1=comma, 2=tab, 3=semicolon)\n"
        "-opsys <P2>       = Required line ending type (1=windows, 2=linux, 3=macos)\n\n"
        "# Optional Flags:\n"
        "-h                Display help message and exit\n\n"
        "# Examples:\n"
        "./2021510107 smartlog.csv logdata.dat 1 -separator 1 -opsys 2\n"
        "./2021510107 logdata.dat smartlogs.xml 2 -separator 1 -opsys 2\n"
        "./2021510107 smartlogs.xml 2021510107.xsd 3 -separator 1 -opsys 2\n"
        "./2021510107 -h\n"
    );
}

// ./2021510107 den sonra minimum 2 tane harf girebilirsin . eğer 2 tane harf girmediysen 7 den de az ise yine hata verir.


int main(int word_length, char *line[]) 
{
    
    if (word_length < 2)
    {
        fprintf(stderr, "Yetersiz arguman. Yardim için -h kullanin.\n");
        return EXIT_FAILURE;
    }

    if (strcmp(line[1], "-h") == 0)
    {
        print_help();
        return EXIT_SUCCESS;
    }

    if (word_length < 7) 
    {
        fprintf(stderr, "Eksik parametreler. Yardim için -h kullanin.\n");
        return EXIT_FAILURE;
    }

    const char *input_file = line[1];
    const char *output_file = line[2];
    int conversion_type = atoi(line[3]);
    int separator = -1, opsys = -1;

    // Argümanları sırayla kontrol et
    for (int i = 4; i < word_length; i++) 
    {
        if (strcmp(line[i], "-separator") == 0 && i + 1 < word_length){
            separator = atoi(line[++i]);
        }
        else if (strcmp(line[i], "-opsys") == 0 && i + 1 < word_length)
        {
            opsys = atoi(line[++i]);
        }
    }

    if (separator < 1 || separator > 3 || opsys < 1 || opsys > 3)
    {
        fprintf(stderr, "Gecersiz -separator veya -opsys degeri. Yardim için -h kullanin.\n");
        return EXIT_FAILURE;
    }

    char choice[512];

    switch (conversion_type) 
    {
        case 1: // CSV to Binary
            snprintf(choice, sizeof(choice),
                     "./csv_to_binary %s %s -separator %d -opsys %d",
                     input_file, output_file, separator, opsys);
            run_choices(choice);
            break;

        case 2: // Binary to XML
            snprintf(choice, sizeof(choice), "./binary_to_xml");
            run_choices(choice);
            break;

        case 3: // XML validation
            snprintf(choice, sizeof(choice), "./validate");
            run_choices(choice);
            break;

        default:
            fprintf(stderr, "Gecersiz donusum tipi. Yardim için -h kullanin.\n");
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
