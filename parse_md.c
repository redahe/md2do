/**
 * Read vimwiki-markdown file from stdin with a todo list, and extract
 * unfinished tasks to a list compatible with calcurse todo file
 * Output format for generated rectods is HEADER| ITEM
 * With -a parse @mm/dd/yyyy from file and produce appointments instead
 * in calcurse format
 * All generated items end with '  # from_vimwiki', for auto-updates
 **/

#include <unistd.h>

#define WINDOW_LENGTH 30
#define MIN_HEADER_LENGTH 15


char header[30];
char item[80];
char date[10];
int cur_h_length = 0;
int cur_i_length = 0;
int cur_d_length = 0;


void write_todo_item() {
  int h_len = cur_h_length;
  if (cur_i_length > 4) {
    if ((item[0] == ' ') &&
        (item[1] == '[') &&
        (item[2] == ' ') &&
        (item[3] == ']')) {
      if (h_len> MIN_HEADER_LENGTH) {
        if ((h_len+cur_i_length-3) > WINDOW_LENGTH) {
          h_len= WINDOW_LENGTH-cur_i_length+3;
        }
        if (h_len< MIN_HEADER_LENGTH) {
          h_len= MIN_HEADER_LENGTH;
        }
      }
      write(1, "[0]", 3);
      write(1, header, h_len);
      write(1, "|", 1);
      write(1, &item[4], cur_i_length-4);
      write(1, "  # from_vimwiki", 16);
      write(1, "\n", 1);
    }
  }
}

void write_apt_item() {
  if (cur_d_length == sizeof(date)) {
    if ((date[0] >= '0') && (date[0] <= '1') &&
        (date[1] >= '0') && (date[1] <= '9') &&
        (date[2] == '/') &&
        (date[3] >= '0') && (date[3] <= '3') &&
        (date[4] >= '0') && (date[4] <= '9') &&
        (date[5] == '/') &&
        (date[6] >= '0') && (date[6] <= '9') &&
        (date[7] >= '0') && (date[7] <= '9') &&
        (date[8] >= '0') && (date[8] <= '9') &&
        (date[9] >= '0') && (date[9] <= '9')) {
        write(1, date, cur_d_length);
        write(1, " [1]", 4);
        write(1, header, cur_h_length);
        write(1, "|", 1);
        write(1, &item[4], cur_i_length-4);
        write(1, "  # from_vimwiki", 16);
        write(1, "\n", 1);
    }
  }
}


int main(int argc, char* argv[]) {
  unsigned short apts = 0;
  int c = getopt (argc, argv, "a");
  if (c == 'a') {
    apts = 1;
  }
  if (c == '?') {
    return -1;
  }
  char buf[2048];
  unsigned short new_line = 1;
  unsigned short reading_header = 0;
  unsigned short reading_item = 0;
  unsigned short reading_date = 0;
  
  ssize_t rd;
  while((rd = read(0, buf, sizeof(buf)))>0) {
    for (int i=0; i<rd; i++) {
      if (buf[i] == '\n') {
        new_line = 1;
        if (reading_item) {
          if (!apts) {
            write_todo_item();
          }
        }
        if (reading_date) {
          if (apts) {
            write_apt_item();
          }
        }
        reading_header = 0;
        reading_item = 0;
        reading_date = 0;
        continue;
      }
      if (((buf[i] == ' ') || buf[i] =='\t' ) && (new_line)) {
        continue;
      }
      if ((buf[i] == '#') && (new_line)){
        new_line = 0;
        reading_header = 1;
        cur_h_length = 0;
        reading_date = 0;
        continue;
     }
     if ((buf[i] == '-') && (new_line)){
        new_line = 0;
        reading_item = 1;
        reading_header = 0;
        reading_date = 0;
        cur_i_length = 0;
        continue;
      }
      if ((buf[i] == '#') && (reading_header)){
        continue;
      }
     if (buf[i] == '@'){
        reading_date = 1;
        cur_d_length = 0;
        continue;
      }
      if (reading_header) {
        if (cur_h_length < sizeof(header)) {
          header[cur_h_length++] = buf[i];
        }
      }
      if (reading_item) {
        if (cur_i_length < sizeof(item)) {
          item[cur_i_length++] = buf[i];
        }
      }
      if (reading_date) {
        if (cur_d_length < sizeof(date)) {
          if ((buf[i] < '0' || buf[i]>'9') && buf[i]!='/') {
            reading_date = 0;
          }
          date[cur_d_length++] = buf[i];
        } else {
          reading_date = 0;
          if (apts) {
            write_apt_item();
          }
        }
      }
      new_line = 0;
    }
  }
}
