
#include <unistd.h>

char line[256];
int cur_len = 0;


void proces_line() {
  int len = cur_len;
  if ((cur_len == 6) &&
      line[0] == 't' &&
      line[1] == 'o' &&
      line[2] == ' ' &&
      line[3] == 'd' &&
      line[4] == 'o' &&
      line[5] == ':'
      ) {
    return;
  }
  if (cur_len > 14) {
    if (line[len-14] == '#' &&
        line[len-13] == ' ' &&
        line[len-12] == 'f' &&
        line[len-11] == 'r' &&
        line[len-10] == 'o' &&
        line[len-9] == 'm' &&
        line[len-8] == '_' &&
        line[len-7] == 'v' &&
        line[len-6] == 'i' &&
        line[len-5] == 'm' &&
        line[len-4] == 'w' &&
        line[len-3] == 'i' &&
        line[len-2] == 'k' &&
        line[len-1] == 'i') {
      len = cur_len - 14;
    }
  }

  if ((len > 12) && 
      (line[3] == '[' &&
       line[4] == 'O' &&
       line[5] == 'v' &&
       line[6] == 'e' &&
       line[7] == 'r' &&
       line[8] == 'd' &&
       line[9] == 'u' &&
       line[10] == 'e' &&
       line[11] == ']')) {
     write(1, "\033[35m[Overdue]\033[0m", 18);
     write(1, &line[12], len-12);
  } else 
    if ((len > 14) &&
        (line[3] == '[' &&
         line[4] == 'S' &&
         line[5] == 'c' &&
         line[6] == 'h' &&
         line[7] == 'e' &&
         line[8] == 'd' &&
         line[9] == 'u' &&
         line[10] == 'l' &&
         line[11] == 'e' &&
         line[12] == 'd' &&
         line[13] == ']')){
     write(1, "\033[94m[Scheduled]\033[0m", 20);
     write(1, &line[14], len-14);
  } 
  else{
    write(1, &line[3], len-3);
  }
  if (len != cur_len) {
    write(1, "\033[90m# from_vimwiki\033[0m", 23);
  }
  write(1, "\n", 1);
}

int main(int argc, char *argv[]) {

  char buf[2048];
  ssize_t rd;
  while((rd = read(0, buf, sizeof(buf)))>0) {
    for (int i=0; i<rd; i++) {
      if (buf[i] == '\n') {
        proces_line();
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
