

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct 
{
    char device_id[8];
    char timestamp[25];
    float temperature;
    int humidity;
    char status[8];
    char location[31];
    char alert_level[10];
    int battery;
    char firmware_ver[10];
    unsigned char event_code;
} CSV_record;

// csv den gelen dataları araları ne ile ayrılmış ona bakıyor . ona göre bölüyor .
char seperate_by_element(int choice) 
{
    if (choice == 1) 
    {
        return ',';
    }
    else if (choice == 2)
    { 
        return '\t';
    }
    else if (choice == 3) 
    {
        return ';';
    }
    else 
    {
        return ',';
    } 
}

// satır sonundaki /n ve /r leri siliyor .
void line_trimmer(char *str) 
{
    char *p;
    if ((p = strchr(str, '\n'))) 
    {
        *p = '\0';
    }
    if ((p = strchr(str, '\r')))
    {
         *p = '\0';
    }
}

// ayırıcı ifadeye bulduktan sonra oraya kadar kısmı alıyor sonuna /0 ekliyor bu sayede parçalamış oluyor .
char* safe_piece_of_text(char **context, char sep) 
{
    if (*context == NULL) 
    {
    return NULL;
    }
    char *start = *context;
    char *end = strchr(start, sep);
    if (end)
    {
        *end = '\0';
        *context = end + 1;
    } 
    else 
    {
        *context = NULL;
    }

    if (strlen(start) == 0) 
    {
    return NULL;
    }
    return start;
}

int main(int sequence_length, char *detail_array[]) 
{
    if (sequence_length < 7) 
    {
        printf("Kullanim: %s <input.csv> <output.dat> -separator <1|2|3> -opsys <1|2|3>\n", detail_array[0]);
        return 1;
    }

    FILE *open_for_read = fopen(detail_array[1], "r");
    FILE *open_for_write = fopen(detail_array[2], "wb");
    FILE *records_file = fopen("skipped_fields.save_entry", "w");

    if (!open_for_read) 
    {
        perror("Giris dosyasi acilamadi");
        return 1;
    }
    if (!open_for_write) 
    {
        perror("cikis dosyasi acilamadi");
        fclose(open_for_read);
        return 1;
    }

    int sep_code = atoi(detail_array[4]);
    char sep = seperate_by_element(sep_code);

    char line[512];
    fgets(line, sizeof(line), open_for_read); // başlığı atla

    int write_count = 0;
    int line_num = 1;

    while (fgets(line, sizeof(line), open_for_read)) 
    {
        line_num++;
        line_trimmer(line);

        CSV_record save_entry;
        memset(&save_entry, 0, sizeof(CSV_record));

        char *context = line;
        char *pieace_of_text;

        pieace_of_text = safe_piece_of_text(&context, sep);
        if (pieace_of_text) 
        {
            // parçaladığımız da bir şey varsa boş değilse yani kaydet onu .
            strncpy(save_entry.device_id, pieace_of_text, sizeof(save_entry.device_id) - 1);
        }
        else 
        {
            //eksik alanlar varsa records_file dosaysına hata mesajı yazılır.
            // ama bu demek değilki yanlış çalışyor.
            //aslında biz zaten eksik kısımların yazılmaısnı istemiyoruz . boşsa boştur .
            //default değer atmamız lazım .
            fprintf(records_file, "Satir %d: Eksik device_id\n", line_num);
        }

        pieace_of_text = safe_piece_of_text(&context, sep);

        if (pieace_of_text) 
        {
        strncpy(save_entry.timestamp, pieace_of_text, sizeof(save_entry.timestamp) - 1);
        }   
        else 
        {
        fprintf(records_file, "Satir %d: Eksik timestamp\n", line_num);
        }
        pieace_of_text = safe_piece_of_text(&context, sep);
        if (pieace_of_text) 
        {
        save_entry.temperature = atof(pieace_of_text);
        }
        else 
        {
        fprintf(records_file, "Satir %d: Eksik temperature\n", line_num);
        }

        pieace_of_text = safe_piece_of_text(&context, sep);
        if (pieace_of_text) 
        {
        save_entry.humidity = atoi(pieace_of_text);
        }
        else 
        {
            save_entry.humidity = -1, fprintf(records_file, "Sat\u0131r %d: Eksik humidity\n", line_num);
        }
        pieace_of_text = safe_piece_of_text(&context, sep);

        if (pieace_of_text) 
        {
        strncpy(save_entry.status, pieace_of_text, sizeof(save_entry.status) - 1);
        }
        else 
        {
        fprintf(records_file, "Satir %d: Eksik status\n", line_num);
        }
        pieace_of_text = safe_piece_of_text(&context, sep);

        if (pieace_of_text) 
        {
        strncpy(save_entry.location, pieace_of_text, sizeof(save_entry.location) - 1);
        }
        else 
        {
        fprintf(records_file, "Satir %d: Eksik location\n", line_num);
        }    
        pieace_of_text = safe_piece_of_text(&context, sep);
        if (pieace_of_text) 
        {
        strncpy(save_entry.alert_level, pieace_of_text, sizeof(save_entry.alert_level) - 1);
        }
        else 
        {
        fprintf(records_file, "Satir %d: Eksik alert_level\n", line_num);
        }
        pieace_of_text = safe_piece_of_text(&context, sep);
        if (pieace_of_text) 
        {
        save_entry.battery = atoi(pieace_of_text);
        }
        else
        {
             fprintf(records_file, "Satir %d: Eksik battery\n", line_num);
        }
        pieace_of_text = safe_piece_of_text(&context, sep);
        if (pieace_of_text) 
        {
            strncpy(save_entry.firmware_ver, pieace_of_text, sizeof(save_entry.firmware_ver) - 1);
        }
        else 
        {
            fprintf(records_file, "Satir %d: Eksik firmware_ver\n", line_num);
        }

        pieace_of_text = safe_piece_of_text(&context, sep);

        if (pieace_of_text) 
        {
            save_entry.event_code = (unsigned char)atoi(pieace_of_text);
        }
        else 
        {
            fprintf(records_file, "Satir %d: Eksik event_code\n", line_num);
        }

        fwrite(&save_entry, sizeof(CSV_record), 1, open_for_write);
        write_count++;
    }

    printf("Toplam yazilan kayit: %d\n", write_count);

    fclose(open_for_read);
    fclose(open_for_write);
    fclose(records_file);
    return 0;
}
