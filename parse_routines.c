/**
 * Read vimwiki-markdown file from stdin with Routines schedule
 * described in the following format:
 *
 * ```
 * # ProfileName1
 *
 *  w0 w4  ...
 *  hh:mm-hh:mm         | Description1
 *  hh:mm-hh:mm         | Description2
 *
 * # ProfileName2
 *
 *  w1 w2  ...
 *  hh:mm-hh:mm         | Description3
 *  hh:mm-hh:mm         | Description4
 *```
    Where wN is the weekday when the profile is active
 *
 * Output Calcurse list of appointments
 * if launched with the profile-name, ignores all other profiles
 **/


#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE  2048
#define MAX_TOKEN_SIZE  127

char buf[2048];
ssize_t buf_size;
ssize_t pos;
char* only_profile = NULL;
unsigned char eof = 0;

char token[MAX_TOKEN_SIZE + 1];
unsigned char token_size = 0;

unsigned char is_whitespace(char c) {
  return c == ' ' || c == '\t' || c=='\n';
}


char first_days[7][11] = {"12/29/2024",
                          "12/30/2024",
                          "12/31/2024",
                          "01/01/2025",
                          "01/02/2025",
                          "01/03/2025",
                          "01/04/2025"};

char digits[7] = "0123456";

void read_buffer() {
  buf_size = read(0, buf, sizeof(buf));
  pos = 0;
  if (buf_size == 0) {
    eof = 1;
  }
}

void next_token() {
  token_size = 0;
  if (eof) {
    token[0] = '\0';
    return;
  }
  if (pos >= buf_size) {
    read_buffer();
  }
  if (eof) {
    token[0] = '\0';
    return;
  }
  // Skip white spaces
  while (is_whitespace(buf[pos])) {
    pos++;
    if (pos >= buf_size) {
      read_buffer();
      if (eof) {
        token[0] = '\0';
        return;
      }
    }
  }
  // Read none white-space
  while (!is_whitespace(buf[pos])) {
    if (token_size == MAX_TOKEN_SIZE) {
      token_size = 0;
    }
    token[token_size++] = buf[pos++];
    if (pos >= buf_size) {
      read_buffer();
      if (eof) {
        token[token_size] = '\0';
        return;
      }
    }
  }
  token[token_size] = '\0';
}

unsigned char get_day(unsigned char weekdays) {
  for (unsigned char i=0; i<7; i++) {
    if (weekdays & 1 <<i) {
      return i;
    }
  }
  return 0;
}

void write_line_end() {
  if (pos >= buf_size) {
      read_buffer();
      if (eof) {
        return;
      }
  }
  int start_pos = pos;
  while (buf[pos] != '\n') {
    pos++;
    if (pos >= buf_size) {
      //flush
      write(1, buf + start_pos, pos-start_pos);
      start_pos = 0;
      read_buffer();
      if (eof) {
        return;
      }
    }
  }
  //flush with the \n symbol
  write(1, buf + start_pos, pos-start_pos+1);
  // Support the invariant that the token should be read
  next_token();
}

void process_line(unsigned char weekdays) {
  unsigned int weekday = get_day(weekdays);

  while (!(token_size ==5 && token[2]==':')) {
    // Skip anything before time specs
    next_token();
    if (eof) {
      return;
    }
  }

  write(1, first_days[weekday], 10);
  write(1, " @ ", 3);
  write(1, token, token_size);
  write(1, " -> ", 4);

  // Next token
  next_token();
  if (eof) {
      return;
  }


  while (!(token_size ==5 && token[2]==':')) {
    // Skip anything before time specs
    next_token();
    if (eof) {
      return;
    }
  }

  write(1, first_days[weekday], 10);
  write(1, " @ ", 3);
  write(1, token, token_size);
  write(1, " {1D", 4);
  for (unsigned int i=0; i<7; i++) {
    if (weekdays & 1 <<i) {
      write(1, " w", 2);
      write(1, digits + i, 1);
    }
  }
  write(1, "}", 1);

  write_line_end();
}

void process_profile() {
  // Skip everything before #
  while (!(token_size ==1 && token[0]=='#')) {
    next_token();
    if (eof) {
      return;
    }
  }
  // Read profile name
  next_token();
  if (eof) {
    return;
  }
  if (only_profile && strcmp(only_profile, token) !=0) {
    return;
  }

  while (!(token_size ==2 && token[0]=='w')) {
    // Skip anything before weekday specs
    next_token();
    if (eof) {
      return;
    }
  }

  // Read weekday spec
  unsigned char weekdays = 0;
  while (token_size ==2 && token[0]=='w') {
    if (token[1] < '0' || token[1] > '7') {
      // Wrong weekday
      continue;
    }
    weekdays |= 1 << (token[1] - '0');
    next_token();
    if (eof) {
      return;
    }
  }
  while (!(token_size ==1 && token[0]=='#')) {
    process_line(weekdays);
    if (eof) {
      return;
    }
  }
}


int main(int argc, char* argv[]) {
  if (argc > 1) {
    only_profile = argv[1];
  }

  while (!eof) {
    process_profile();
  }
}
