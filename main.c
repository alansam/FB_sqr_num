/*
 *  input: 125
 *  output: 1425
 *    1*1-2*2-5*5 => 1-4-2-5 => 1425
 *  passing 3999999999u give some random negative number..
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

#include "version_info.h"
#include "identify.h"

#define OVERFLOW_CHK_
#ifndef OVERFLOW_CHK_
#  if __has_builtin(__builtin_umulll_overflow_overflow)
#    error "no __builtin_umulll_overflow_overflow"
#  endif
#  if __has_builtin(__builtin_uaddll_overflow)
#    error "no __builtin_uaddll_overflow"
#  endif
#  if __has_builtin(__builtin_sadd_overflow)
#    error "no __builtin_sadd_overflow"
#  endif
#endif

/*
 *  MARK:  numlen()
 */
int numlen(unsigned n) {
  int count = 0;
  if (n / 10 == 0) {
    return 1;
  }

  while (n > 0) {
    count++;
    n = n / 10;
  }

  return count;
}

/*
 *  MARK:  square_digits()
 */
unsigned long long square_digits(unsigned n) {
  unsigned long long res = 0ull;
  unsigned long long num_sqr;
  int power_index = 0;

  while (n > 0) {
    unsigned dig = n % 10;
    num_sqr = pow(/*n % 10*/ dig, 2);

#ifndef OVERFLOW_CHK_
    res += num_sqr * pow(10, power_index);
    power_index += numlen(num_sqr);
#else
    unsigned long long pow_tmp;
    unsigned long long res_tmp;
    int errline = 0;

    pow_tmp = (unsigned long long) pow(10, power_index);
    errline = __LINE__ + 1;
    bool oflow;
    oflow = __builtin_umulll_overflow(num_sqr,
                                      pow_tmp,
                                      &res_tmp);
    if (oflow) {
      fprintf(stderr,
              "overflow detected near line"
              " %d: %llu %llu %llu\n",
              errline, num_sqr, pow_tmp, res_tmp);
      exit(EXIT_FAILURE);
    }
    errline = __LINE__ + 1;
    oflow = __builtin_uaddll_overflow(res, res_tmp, &res);
    if (oflow) {
      fprintf(stderr,
              "overflow detected near line"
              " %d: %llu %llu\n",
              errline, res, res_tmp);
      exit(EXIT_FAILURE);
    }
    errline = __LINE__ + 1;
    int nl_tmp = numlen(num_sqr);
    oflow = __builtin_sadd_overflow(power_index,
                                    nl_tmp,
                                    &power_index);
    if (oflow) {
      fprintf(stderr,
              "overflow detected near line"
              " %d: %d %d\n",
              errline, power_index, nl_tmp);
      exit(EXIT_FAILURE);
    }
#endif  /* OVERFLOW_CHK_ */
    n = n / 10;
    int nl = numlen(n);
    printf("- %u %3d %5llu %3d %11u %#010x\n",
           dig, power_index, num_sqr, nl, n, n);
  }

  return res;
}

/*
 *  MARK:  main()
 */
int main(int argc, char const * argv[]) {
  puts("----+----|----+----|----+----|----+----|");
  version_details();
  putchar('\n');
  identify();
  puts("----+----|----+----|----+----|----+----|");
  putchar('\n');

  printf("%21u %#018x\n", UINT_MAX, UINT_MAX);
  unsigned int tv = 3999999999u;
  printf("%21u %#018x\n", tv, tv);

  printf("%21llu %#018llx\n", ULLONG_MAX, ULLONG_MAX);
  unsigned long long res = square_digits(3999999999u);
  printf("%21llu %#018llx\n", res, res);

  unsigned int test[] = {
    2u, 4u, 6u, 8u,
    125u,
    3999999999u, 3999999999u + 1u,
    UINT_MAX,
  };
  size_t const test_sz = sizeof test / sizeof *test;

  putchar('\n');
  for (size_t t_ = 0ul; t_ < test_sz; ++t_) {
    printf("%21u %#018x\n", test[t_], test[t_]);
    unsigned long long res = square_digits(test[t_]);
    printf("%21llu %#018llx\n\n", res, res);
  }

  unsigned tt[] = {
    tv, (tv - 1u), (tv + 1u), (tv + 2u),
  };
  size_t const tt_sz = sizeof tt / sizeof *tt;

  for (size_t t_ = 0ul; t_ < tt_sz; ++t_) {
    putchar('\n');
    puts("----+----|----+----|----+----|----+----|");
    unsigned tv = tt[t_];
    size_t const tvl = numlen(tv);
    printf("%21u %zu ~~~\n", tv, tvl);
    for (size_t f_ = 0ul; f_ < tvl; ++f_) {
      printf("%21u %#018x\n", tv, tv);
      unsigned long long res = square_digits(tv);
      printf("%21llu %#018llx\n\n", res, res);
      tv /= 10;
    }
    putchar('~');
    putchar('\n');
  }

  return EXIT_SUCCESS;
}
