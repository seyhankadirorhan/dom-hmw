

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <json-c/json.h>

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

#define Maximum_record 1000
// bih endian a dönüştürüyor.
void convert_to_big_endian(unsigned char value, char *output) 
{
    sprintf(output, "%08X", value);
}
// little endiana dönüştürüyor .
void convert_to_hex_little_endian(unsigned char value, char *output)
{
    uint32_t num = value;
    unsigned char bytes[4] = { num, 0, 0, 0 };
    sprintf(output, "%02X%02X%02X%02X", bytes[0], bytes[1], bytes[2], bytes[3]);
}

// hangisinin daha büyük küçük olduğuna karar veriyor .
int compare_logs(const void *x, const void *y, void *choose_x_y_index)
{
    int *range = (int *)choose_x_y_index;
    const CSV_record *log_X = (const CSV_record *)x;
    const CSV_record *log_Y = (const CSV_record *)y;
    return strncmp(log_X->device_id + range[0], log_Y->device_id + range[0], range[1] - range[0]);
}

int main() 
{
    // setupParams.json dosyasını açıyor .
    FILE *jsonFile = fopen("setupParams.json", "r");
    if (!jsonFile)
    {
        perror("setupParams.json açilamadi");
        return 1;
    }

    char jsonBuffer[512];
    fread(jsonBuffer, 1, sizeof(jsonBuffer), jsonFile);
    fclose(jsonFile);

    struct json_object *parsed_json = json_tokener_parse(jsonBuffer);

    const char *binary_filename = json_object_get_string(json_object_object_get(parsed_json, "dataFileName"));
    int keyStart = json_object_get_int(json_object_object_get(parsed_json, "keyStart"));
    int keyEnd = json_object_get_int(json_object_object_get(parsed_json, "keyEnd"));
    const char *order = json_object_get_string(json_object_object_get(parsed_json, "order"));

    FILE *opened_file = fopen(binary_filename, "rb");
    if (!opened_file) 
    {
        perror("Binary dosyasi açilamadi");
        return 1;
    }

    CSV_record records[Maximum_record];
    int count = fread(records, sizeof(CSV_record), Maximum_record, opened_file);
    fclose(opened_file);

    int range[2] = {keyStart, keyEnd};
    // qsort işlemi kullanıyoruz çünkü compare_log fonksiyonunda 3 parametre alıyor bunu sağlayan sort qsort kullanıldı .
    qsort_r(records, count, sizeof(CSV_record), compare_logs, range);

    if (strcmp(order, "DESC") == 0) 
    {
        for (int i = 0; i < count / 2; i++)
        {
            CSV_record temp = records[i];
            records[i] = records[count - 1 - i];
            records[count - 1 - i] = temp;
        }
    }

    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr root = xmlNewNode(NULL, BAD_CAST "smartlogs");
    xmlDocSetRootElement(doc, root);

    char buffer[32];
// XML e yazdırdığı yer burası . Yapıyı burda kuruyor burda bağlıyor root u childlarını .
    for (int i = 0; i < count; i++) 
    {
        xmlNodePtr entry = xmlNewChild(root, NULL, BAD_CAST "entry", NULL);
        sprintf(buffer, "%d", i + 1);
        xmlNewProp(entry, BAD_CAST "id", BAD_CAST buffer);

        xmlNodePtr device = xmlNewChild(entry, NULL, BAD_CAST "device", NULL);
        // device_id varsa xml yazııyoruz yoksa yazmıyoruz yani 0 dan büyük olması bunun kontrolünü sağlıyor . default değer atmamamk için .
        // aynı şey diğer ifler içinde geçerli .
        if (strlen(records[i].device_id) > 0) {
            xmlChar *enc_device_id = xmlEncodeEntitiesReentrant(doc, BAD_CAST records[i].device_id);
            xmlNewChild(device, NULL, BAD_CAST "device_id", enc_device_id);
            xmlFree(enc_device_id);
        }

        if (strlen(records[i].location) > 0) {
            xmlChar *enc_location = xmlEncodeEntitiesReentrant(doc, BAD_CAST records[i].location);
            xmlNewChild(device, NULL, BAD_CAST "location", enc_location);
            xmlFree(enc_location);
        }

        if (strlen(records[i].firmware_ver) > 0) {
            xmlChar *enc_firmware_ver = xmlEncodeEntitiesReentrant(doc, BAD_CAST records[i].firmware_ver);
            xmlNewChild(device, NULL, BAD_CAST "firmware_ver", enc_firmware_ver);
            xmlFree(enc_firmware_ver);
        }

        xmlNodePtr metrics = xmlNewChild(entry, NULL, BAD_CAST "metrics", NULL);

        if (strlen(records[i].status) > 0)
            xmlNewProp(metrics, BAD_CAST "status", BAD_CAST records[i].status);

        if (strlen(records[i].alert_level) > 0)
            xmlNewProp(metrics, BAD_CAST "alert_level", BAD_CAST records[i].alert_level);

        if (records[i].temperature != 0.0f)
        {
            sprintf(buffer, "%.1f", records[i].temperature);
            xmlNewChild(metrics, NULL, BAD_CAST "temperature", BAD_CAST buffer);
        }

        if (records[i].humidity >= 0)
        {
            sprintf(buffer, "%d", records[i].humidity);
            xmlNewChild(metrics, NULL, BAD_CAST "humidity", BAD_CAST buffer);
        }

        if (records[i].battery >= 0)
        {
            sprintf(buffer, "%d", records[i].battery);
            xmlNewChild(metrics, NULL, BAD_CAST "battery", BAD_CAST buffer);
        }

        if (strlen(records[i].timestamp) > 0) {
            xmlChar *enc_timestamp = xmlEncodeEntitiesReentrant(doc, BAD_CAST records[i].timestamp);
            xmlNewChild(entry, NULL, BAD_CAST "timestamp", enc_timestamp);
            xmlFree(enc_timestamp);
        }

        if (records[i].event_code != 0)
        {
            char hexBig[16], hexLittle[16];
            convert_to_big_endian(records[i].event_code, hexBig);
            convert_to_hex_little_endian(records[i].event_code, hexLittle);

            sprintf(buffer, "%d", records[i].event_code);
            xmlNodePtr event = xmlNewChild(entry, NULL, BAD_CAST "event_code", BAD_CAST buffer);
            xmlNewProp(event, BAD_CAST "hexBig", BAD_CAST hexBig);
            xmlNewProp(event, BAD_CAST "hexLittle", BAD_CAST hexLittle);

            unsigned int decimal_value;
            sscanf(hexLittle, "%x", &decimal_value);
            sprintf(buffer, "%u", decimal_value);
            xmlNewProp(event, BAD_CAST "decimal", BAD_CAST buffer);
        }
    }

    // xml i oluşturduktan sonra smartlog.xml de bunu kaydediyor .
    xmlSaveFormatFileEnc("smartlogs.xml", doc, "UTF-8", 1);
    xmlFreeDoc(doc);
    xmlCleanupParser();

    printf("XML dosyasi başariyla oluşturuldu: smartlogs.xml\n");

    return 0;
}
