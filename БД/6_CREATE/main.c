#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include <string.h>



int main(int argc, char *argv[])
{
  sqlite3 *db;
  sqlite3_open("db.sqlite3", &db);
  FILE *f;
  fopen("script.sql", "r");
  
  
  int buf_size = 4;
  char* mySQL = malloc(sizeof(char)* buf_size);
  int i = 0;
  printf(mySQL[0]);
  while ((mySQL[i] = getc(f)) != EOF){
    printf("%c", mySQL[i]);
    i++;
    if (i = buf_size){
        char* newsql = malloc(sizeof(char) * i);
        memcpy(newsql, mySQL, sizeof(char) * i);
        mySQL = realloc(mySQL, 2*i);
        memcpy(mySQL, newsql, sizeof(char) * i);
        free(newsql);
        buf_size*=2;
        }   
    } 
  char* cur = mySQL;
  char* end = cur + strlen(mySQL);
  sqlite3_stmt *st;
  while (cur < end){
        sqlite3_prepare(db, cur, end - cur, &st, &cur);
        sqlite3_step(st);
        sqlite3_finalize(st);
    } 
  fclose(f);
  system("PAUSE");	
  return 0;
}
