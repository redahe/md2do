/**
 * Read vimwiki-markdown file from stdin with a todo list, and extract
 * unfinished tasks to a list compatible with calcurse todo file
 * Format is HEADER| ITEM contracted to MAX_LINE_LENGTH
 **/

#include <unistd.h>

#define WINDOW_LENGTH 30
#define MIN_HEADER_LENGTH 15

int main() {

  char buf[2048];
  char header[30];
  char item[80];
  int cur_h_length = 0;
  int cur_i_length = 0;
  unsigned short new_line = 1;
  unsigned short reading_header = 0;
  unsigned short reading_item = 0;
  
  ssize_t rd;
  while((rd = read(0, buf, sizeof(buf)))>0) {
    for (int i=0; i<rd; i++) {
      if (buf[i] == '\n') {
        new_line = 1;
        if (reading_item) {
          if (cur_i_length > 4) {
            if ((item[0] == ' ') &&
                (item[1] == '[') &&
                (item[2] == ' ') &&
                (item[3] == ']')) {
              if (cur_h_length > MIN_HEADER_LENGTH) {
                if ((cur_h_length+cur_i_length-3) > WINDOW_LENGTH) {
                  cur_h_length = WINDOW_LENGTH-cur_i_length+3;
                }
                if (cur_h_length < MIN_HEADER_LENGTH) {
                  cur_h_length = MIN_HEADER_LENGTH;
                }
              }
              write(1, "[0]", 3);
              write(1, header, cur_h_length);
              write(1, "|", 1);
              write(1, &item[4], cur_i_length-4);
              write(1, "\n", 1);
            }
          }
        }
        reading_header = 0;
        reading_item = 0;
        continue;
      }
      if (((buf[i] == ' ') || buf[i] =='\t' ) && (new_line)) {
        continue;
      }
      if ((buf[i] == '#') && (new_line)){
        new_line = 0;
        reading_header = 1;
        cur_h_length = 0;
        continue;
     }
     if ((buf[i] == '-') && (new_line)){
        new_line = 0;
        reading_item = 1;
        reading_header = 0;
        cur_i_length = 0;
        continue;
      }
      if ((buf[i] == '#') && (reading_header)){
        continue;
      }
      if (reading_header) {
        if (cur_h_length < sizeof(header)) {
          header[cur_h_length++] = buf[i];
        }
        continue;
      }
      if (reading_item) {
        if (cur_i_length < sizeof(item)) {
          item[cur_i_length++] = buf[i];
        }
        continue;
      }
      new_line = 0;
    }
  }
}
