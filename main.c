#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curl/curl.h>
#include <string.h>
#include <json-c/json.h>

#define MAX_AYAH 6236
#define API_URL "https://api.alquran.cloud/ayah/"

struct timespec nanos;
struct string
{
  char* data;
  size_t len;
};

size_t writeFunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
  size_t new_len = s->len + size*nmemb;
  s->data = realloc(s->data, new_len+1);

  memcpy(s->data+s->len, ptr, size*nmemb);
  s->data[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}

int genAyahNumber()
{
  clock_gettime(CLOCK_MONOTONIC, &nanos);
  srand(nanos.tv_nsec);
  return rand() % MAX_AYAH + 1;
}

void displayAyah(struct string resp)
{  
  struct json_object *parsed_json;
  struct json_object *data;

  struct json_object *text;
  struct json_object *numberInSurah;

  struct json_object *surahObj;
  struct json_object *surahName;

  // Parsing main json
  parsed_json = json_tokener_parse(resp.data);
  json_object_object_get_ex(parsed_json, "data", &data);
  
  // Parsing ayah text
  json_object_object_get_ex(data, "text", &text);
  json_object_object_get_ex(data, "numberInSurah", &numberInSurah);

  // Parsing Surah data
  json_object_object_get_ex(data, "surah", &surahObj);
  json_object_object_get_ex(surahObj, "englishName", &surahName);

  printf("\n%s\n", json_object_get_string(text));
  printf("\nSurah %s: %d\n\n", json_object_get_string(surahName), json_object_get_int(numberInSurah));
}

int main(void)
{
  printf("Fetching Ayah...\n");

  /* Init curl */
  CURL *curl;
  CURLcode code;

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  
  if (curl == NULL)
  {
    return 128;
  }
  
  /* Generating URL */
  int ayahNumber = genAyahNumber();
  char url[50]; // Max length for URL
  sprintf(url, "%s%d%s", API_URL, ayahNumber, "/en.asad");
  
  /* Making request */
  struct string resp;
  resp.len = 0;
  resp.data = malloc(resp.len + 1);
  resp.data[0] = '\0';

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunc);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
  curl_easy_setopt(curl, CURLOPT_URL, url);

  code = curl_easy_perform(curl);
  
  if (code != CURLE_OK)
  {
    fprintf(stderr, "curl_easy_perform failed: %s\n", curl_easy_strerror(code));
    return -1;
  }
  displayAyah(resp);

  /* Cleanup */
  curl_easy_cleanup(curl);
  curl_global_cleanup();
  free(resp.data);

  return 0;
}
