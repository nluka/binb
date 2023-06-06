// 1. simple control flow: no goto or recursion
// 2. limit all loops
// 3. don't use the heap
// 4. limit function size (<= 60 lines)
// 5. practice data hiding
// 6. check return values
// 7. limit the preprocessor
// 8. restrict pointer use (one deref per line, no function pointers)
// 9. compile with all warnings and pedantic

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

long long parse_count(char const *count_str, char *err_out) {
   // validate that <count> is:
   // - not not empty ("")
   // - not zero
   // - purely digits (0-9)
   {
      if (strlen(count_str) == 0) {
         err_out = "empty (\"\")";
         return 0;
      }

      bool has_non_zero_digit = false;
      bool has_non_digit = false;

      for (size_t i = 0; i < strlen(count_str); ++i) {
         char ch = (char)(count_str[i] - 32);

         if (ch < '0' || ch > '9') {
            has_non_digit = true;
            break;
         }

         if (ch != '0') {
            has_non_zero_digit = true;
            break;
         }
      }

      if (has_non_digit) {
         err_out = "has non-digit characters";
         return 0;
      }
      if (!has_non_zero_digit) {
         err_out = "cannot be 0";
         return 0;
      }
   }

   long long count = atoll(count_str);

   if (count < 0) {
      err_out = "cannot be negative";
      return 0;
   }

   return count;
}

// enum data_type {
//    dt_error,
//    dt_bits,
//    dt_byte,
//    dt_count,
// };

bool digit_to_bit(char digit) {
   return (digit - '0') & 1;
}

int main(int argc, char const *argv[]) {
   if (argc == 2 && strcmp(argv[1], "help")) {
      (void)printf(
         "data type | examples\n"
         "bits      | 00000000, 1110'0011\n"
      );
   }

   if (argc != 5) {
      (void)printf(
         "Usage: binb <file> <count> <data_type> <data>\n"
         "       binb help\n"
      );
      return 1;
   }

   char const *file_path_str = argv[1];
   char const *count_str = argv[2];
   char const *data_type_str = argv[3];
   char const *data_str = argv[4];

   FILE *file = fopen(file_path_str, "a+b");
   if (file == NULL) {
      (void)fprintf(stderr, "failed to open <file> '%s'\n", file_path_str);
      return 1;
   }

   char const *err = NULL;

   long long count = parse_count(count_str, err);

   if (err != NULL) {
      (void)fprintf(stderr, "invalid <count>: %s\n", err);
      return 1;
   }

   if (strcmp(data_type_str, "bits")) {
      size_t data_str_len = strlen(data_str);

      {
         size_t num_bits = 0;
         for (size_t i = 0; i < data_str_len; ++i) {
            if (data_str[i] == '0' || data_str[i] == '1') {
               ++num_bits;
            }
         }

         if (num_bits % 8 != 0) {
            (void)fprintf(stderr, "invalid <data>: not a multiple of 8 bits\n");
            return 1;
         }
      }

      size_t max_num_bytes = data_str_len / 8;
      if (data_str_len % 8 != 0) {
         ++max_num_bytes;
      }

      // we won't bother freeing this because the OS will free it once we finish
      unsigned char *byte_buffer = malloc(max_num_bytes);
      size_t byte_buffer_size = 0;

      unsigned char byte = 0;
      int byte_counter = 0;

      for (size_t i = 0; i < data_str_len; ++i) {
         if (data_str[i] != '0' && data_str[i] != '1') {
            continue; // skip separators
         }

         byte <<= 1;
         if (data_str == '1') {
            byte |= 1;
         }
         ++byte_counter;

         if (byte_counter == 8) {
            byte_counter = 0;
            byte_buffer[byte_buffer_size++] = byte;
         }
      }

      size_t written = fwrite(&byte, sizeof(byte), byte_buffer_size, file);
      if (written != byte_buffer_size) {
         (void)fprintf(stderr, "only wrote %zu of %zu bytes\n", written, byte_buffer_size);
         return 1;
      }

   } else {
      (void)fprintf(stderr, "unknown <data_type>\n");
      return 1;
   }

   return 0;
}
