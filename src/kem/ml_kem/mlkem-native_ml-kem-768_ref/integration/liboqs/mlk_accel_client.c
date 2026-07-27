#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "mlk_accel_client.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MLK_ACCEL_DEFAULT_IP "192.168.50.2"
#define MLK_ACCEL_DEFAULT_PORT 5000
#define MLK_ACCEL_MAX_IN 1920
#define MLK_ACCEL_OUT 256

static int mlk_accel_fd = -1;
static pthread_mutex_t mlk_accel_lock = PTHREAD_MUTEX_INITIALIZER;
static uint8_t mlk_accel_buf[4 + MLK_ACCEL_MAX_IN * 2];

static void mlk_accel_fail(const char *what)
{
  fprintf(stderr, "mlk_accel: %s\n", what);
  abort();
}

static int mlk_accel_read_all(int fd, void *p, size_t n)
{
  size_t got = 0;
  while (got < n)
  {
    ssize_t r = read(fd, (char *)p + got, n - got);
    if (r <= 0)
    {
      return -1;
    }
    got += (size_t)r;
  }
  return 0;
}

static int mlk_accel_write_all(int fd, const void *p, size_t n)
{
  size_t sent = 0;
  while (sent < n)
  {
    ssize_t w = write(fd, (const char *)p + sent, n - sent);
    if (w <= 0)
    {
      return -1;
    }
    sent += (size_t)w;
  }
  return 0;
}

static void mlk_accel_connect(void)
{
  const char *ip = getenv("MLK_ACCEL_IP");
  const char *port = getenv("MLK_ACCEL_PORT");
  struct sockaddr_in addr;
  int fd, one = 1;

  if (ip == NULL)
  {
    ip = MLK_ACCEL_DEFAULT_IP;
  }

  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
  {
    mlk_accel_fail("socket");
  }

  memset(&addr, 0, sizeof addr);
  addr.sin_family = AF_INET;
  addr.sin_port =
      htons(port != NULL ? (uint16_t)atoi(port) : MLK_ACCEL_DEFAULT_PORT);
  if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1)
  {
    mlk_accel_fail("address");
  }
  if (connect(fd, (struct sockaddr *)&addr, sizeof addr) < 0)
  {
    mlk_accel_fail("connect");
  }
  setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof one);

  mlk_accel_fd = fd;
}

static void mlk_accel_op(uint32_t op, const int16_t *in, uint32_t nin,
                         int16_t *out)
{
  pthread_mutex_lock(&mlk_accel_lock);

  if (mlk_accel_fd < 0)
  {
    mlk_accel_connect();
  }

  memcpy(mlk_accel_buf, &op, 4);
  memcpy(mlk_accel_buf + 4, in, (size_t)nin * 2);

  if (mlk_accel_write_all(mlk_accel_fd, mlk_accel_buf, 4 + (size_t)nin * 2) != 0)
  {
    mlk_accel_fail("write");
  }
  if (mlk_accel_read_all(mlk_accel_fd, out, MLK_ACCEL_OUT * 2) != 0)
  {
    mlk_accel_fail("read");
  }

  pthread_mutex_unlock(&mlk_accel_lock);
}

void mlk_accel_ntt(int16_t p[256]) { mlk_accel_op(0, p, 256, p); }

void mlk_accel_intt(int16_t p[256]) { mlk_accel_op(1, p, 256, p); }

void mlk_accel_basemul_acc_k3(int16_t r[256], const int16_t a[768],
                              const int16_t b[768], const int16_t b_cache[384])
{
  int16_t in[MLK_ACCEL_MAX_IN];

  memcpy(in, a, 768 * 2);
  memcpy(in + 768, b, 768 * 2);
  memcpy(in + 1536, b_cache, 384 * 2);

  mlk_accel_op(2, in, MLK_ACCEL_MAX_IN, r);
}
