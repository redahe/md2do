/**
 * Beutify non-interactive output of calcurse -d X
 **/

#include <unistd.h>
#include <time.h>


char line[256];
int cur_len = 0;
short int cur_date_rel = -1;
short int got_next_apt = 0;

void proces_line(struct tm *loc_time) {
  if (cur_len > 0) {
    if (cur_len == 9 && line[0] == '0' || line[0] =='1') {
      int month = (line[0]-(int)'0')*10 + (line[1]-(int)'0');
      int day = (line[3]-(int)'0')*10 + (line[4]-(int)'0');
      int year = 2000 + (line[6]-(int)'0')*10 + (line[7]-(int)'0');
      if ((loc_time->tm_year + 1900 == year)&&(loc_time->tm_mon+1 == month)&&
          (loc_time->tm_mday == day)) {
        cur_date_rel = 0; //today
          write(1, "\033[37m", 5);
          write(1, line, cur_len);
          write(1, "\n", 1);
          write(1, "\033[35m", 5);
          return;
      } else {
        if (cur_date_rel == 0) {
          cur_date_rel = 1; // next day
          write(1, "\033[90m", 5);
        }
      }
    }  else
    if (cur_len == 17 && line[0] == ' ' && line[1] == '-' && line[2] == ' ') {
      if (cur_date_rel == 0) {
        int hour_end = (line[12]-(int)'0')*10 + line[13]-(int)'0';
        int minute_end = (line[15]-(int)'0')*10 + line[16]-(int)'0';
        if (hour_end < loc_time->tm_hour || (hour_end == loc_time->tm_hour &&
              minute_end < loc_time->tm_min)) {
            write(1, "\033[90m", 5);
        } else {
          if (!got_next_apt) {
            write(1, "\033[94m", 5);
            got_next_apt = 1;
          } else {
            write(1, "\033[37m", 5);
          }
        }
      }
    }
  }
  write(1, line, cur_len);
  write(1, "\n", 1);
}


int main(int argc, char *argv[]) {

  time_t curtime;   // not a primitive datatype
  time(&curtime);
  struct tm *loc_time;
  loc_time = localtime (&curtime);

  char buf[2048];
  ssize_t rd;
  write(1, "\033[90m", 5);
  while((rd = read(0, buf, sizeof(buf)))>0) {
    for (int i=0; i<rd; i++) {
      if (buf[i] == '\n') {
        proces_line(loc_time);
        cur_len = 0;
      } else {
        if (cur_len < sizeof(line)) {
          line[cur_len++] = buf[i];
        }
      }
    }
  }
  write(1, "\033[0m", 4);
}
