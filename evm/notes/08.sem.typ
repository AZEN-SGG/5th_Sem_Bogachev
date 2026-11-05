= Семинар 8. Средства синхронизации POSIX Threads

== Мьютекс

*Мьютекс* — объект взаимного исключения, защищающий критическую секцию от одновременного выполнения несколькими потоками.

```cpp
#include <pthread.h>

pthread_mutex_t m;

int pthread_mutex_init (
  pthread_mutex_t *m,
  const pthread_mutexattr_t *attr
);

int pthread_mutex_destroy (pthread_mutex_t *m);
int pthread_mutex_lock (pthread_mutex_t *m);
int pthread_mutex_unlock (pthread_mutex_t *m);
int pthread_mutex_trylock (pthread_mutex_t *m);
```

- `pthread_mutex_init` инициализирует мьютекс; значение `nullptr` задаёт атрибуты по умолчанию;
- `pthread_mutex_destroy` уничтожает инициализированный и незахваченный мьютекс;
- `pthread_mutex_lock` захватывает мьютекс или ожидает его освобождения;
- `pthread_mutex_unlock` освобождает мьютекс;
- `pthread_mutex_trylock` пытается захватить мьютекс без ожидания.

Статическая инициализация выполняется до начала работы потоков:

```cpp
static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
```

Один объект нельзя одновременно инициализировать из нескольких потоков с помощью `pthread_mutex_init`. Статический инициализатор создаёт мьютекс заранее и исключает такую гонку.

== Условная переменная

*Условная переменная* (`condition variable`) позволяет потоку ожидать изменения состояния общих данных. Само условие хранится в данных программы и проверяется под мьютексом.

```cpp
int pthread_cond_init (
  pthread_cond_t *c,
  const pthread_condattr_t *attr
);

int pthread_cond_signal (pthread_cond_t *c);
int pthread_cond_broadcast (pthread_cond_t *c);
int pthread_cond_destroy (pthread_cond_t *c);
int pthread_cond_wait (pthread_cond_t *c, pthread_mutex_t *m);

static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
```

- `pthread_cond_signal` пробуждает один ожидающий поток;
- `pthread_cond_broadcast` пробуждает все ожидающие потоки;
- `pthread_cond_wait` атомарно освобождает мьютекс и переводит поток в ожидание; перед возвратом функция снова захватывает мьютекс.

Условие проверяется в цикле, поскольку после пробуждения оно могло снова стать ложным.

```cpp
pthread_mutex_lock (&m);

while (!x)
  pthread_cond_wait (&c, &m);

pthread_mutex_unlock (&m);
```

=== Пример ожидания счётчика

```cpp
void f1 (int p)
{
  pthread_mutex_lock (&m);

  while (x != p)
    pthread_cond_wait (&c, &m);

  pthread_mutex_unlock (&m);
}

void f2 ()
{
  pthread_mutex_lock (&m);
  x++;
  pthread_cond_signal (&c);
  pthread_mutex_unlock (&m);
}
```

Поток `f1` ожидает, пока общий счётчик `x` не достигнет `p`. Поток `f2` изменяет счётчик под тем же мьютексом и уведомляет ожидающий поток.

== Суммирование результатов потоков

Функция `reduce_sum` выполняет коллективное суммирование массивов. Каждый из `p` потоков передаёт свой массив, после чего итоговая сумма копируется всем участникам.

Два счётчика образуют две фазы синхронизации:

- `t_in` считает потоки, передавшие данные;
- `t_out` считает потоки, получившие результат.

```cpp
void reduce_sum (int p, double *a = nullptr, int n = 0)
{
  static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
  static pthread_cond_t c_in = PTHREAD_COND_INITIALIZER;
  static pthread_cond_t c_out = PTHREAD_COND_INITIALIZER;
  static int t_in = 0;
  static int t_out = 0;
  static double *res = nullptr;

  if (p <= 1)
    return;

  pthread_mutex_lock (&m);

  if (res == nullptr)
    res = a;
  else
    for (int i = 0; i < n; ++i)
      res[i] += a[i];

  t_in++;

  if (t_in >= p)
    {
      t_out = 0;
      pthread_cond_broadcast (&c_in);
    }
  else
    {
      while (t_in < p)
        pthread_cond_wait (&c_in, &m);
    }

  if (res != a)
    for (int i = 0; i < n; ++i)
      a[i] = res[i];

  t_out++;

  if (t_out >= p)
    {
      res = nullptr;
      t_in = 0;
      pthread_cond_broadcast (&c_out);
    }
  else
    {
      while (t_out < p)
        pthread_cond_wait (&c_out, &m);
    }

  pthread_mutex_unlock (&m);
}
```

Вторая фаза не позволяет начать следующий вызов `reduce_sum`, пока результат текущего вызова не получили все потоки.

== Барьер

*Барьер* — точка синхронизации, в которой каждый поток ожидает прибытия остальных участников.

```cpp
int pthread_barrier_init (
  pthread_barrier_t *b,
  const pthread_barrierattr_t *attr,
  unsigned int count
);

int pthread_barrier_wait (pthread_barrier_t *b);
int pthread_barrier_destroy (pthread_barrier_t *b);

static pthread_barrier_t b;
```

```cpp
pthread_barrier_wait (&b);
```

После прибытия `count` потоков все они продолжают выполнение. Одному потоку `pthread_barrier_wait` возвращает `PTHREAD_BARRIER_SERIAL_THREAD`, остальным — `0`.

== Измерение времени

*Полное время* — фактическое время между началом и окончанием участка программы.

```cpp
#include <sys/time.h>

double get_full_time (void)
{
  struct timeval buf;
  gettimeofday (&buf, nullptr);

  return buf.tv_sec + buf.tv_usec / 1.e6;
}
```

*Процессорное время* — время, в течение которого процессор выполнял команды программы. В зависимости от используемой функции оно может измеряться для одного потока или для всего процесса.

`getrusage` с параметром `RUSAGE_THREAD` возвращает статистику текущего потока.

```cpp
#include <sys/resource.h>

double get_cpu_time (void)
{
  struct rusage buf;
  getrusage (RUSAGE_THREAD, &buf);

  return buf.ru_utime.tv_sec + buf.ru_utime.tv_usec / 1.e6;
}
```

== Аргументы потока

Объект `Args` хранит общие массивы, параметры задачи, номер потока и результаты измерений.

```cpp
class Args
{
public:
  double *a = nullptr;
  double *b = nullptr;
  double *x = nullptr;

  const char *name = nullptr;
  int n = 0;
  int m = 0;
  int p = 0;
  int k = 0;
  int s = 0;

  double t = 0;
  double t_full = 0;

  Args () = default;
  Args (const Args &) = delete;
  Args (Args &&) = default;
  Args & operator= (const Args &) = delete;
  Args & operator= (Args &&) = default;
  ~Args () = default;
};
```

Копирование запрещено, поэтому объект аргументов не может быть случайно скопирован вместе с указателями на общие данные.

== Создание потоков

Для каждого потока создаётся отдельный объект `Args`. Основной поток выполняет функцию с номером `0`, остальные запускаются через `pthread_create`.

```cpp
int main (int argc, char *argv[])
{
  double *a = nullptr;
  double *b = nullptr;
  double *x = nullptr;
  Args *ap = nullptr;
  pthread_t *tid = nullptr;

  int n = 0, m = 0, p = 0, s = 0;
  const char *name = nullptr;

  // Разбор аргументов командной строки.

  ap = new Args[p];
  tid = new pthread_t[p];

  if (ap == nullptr || tid == nullptr)
    {
      // Обработка ошибки выделения памяти.
    }

  a = new double[n * n];
  b = new double[n];
  x = new double[n];

  for (int u = 0; u < p; ++u)
    {
      ap[u].a = a;
      ap[u].b = b;
      ap[u].x = x;
      ap[u].name = name;
      ap[u].n = n;
      ap[u].m = m;
      ap[u].p = p;
      ap[u].k = u;
      ap[u].s = s;
    }

  for (int u = 1; u < p; ++u)
    {
      if (pthread_create (tid + u, nullptr, thread_func, ap + u))
        {
          // Обработка ошибки создания потока.
        }
    }

  thread_func (ap + 0);

  for (int u = 1; u < p; ++u)
    pthread_join (tid[u], nullptr);

  // Вывод результата и освобождение памяти.
  return 0;
}
```

== Функция потока

=== Привязка к процессору

Привязка потока к логическому процессору делает распределение вычислений предсказуемым и позволяет применять политику первого обращения в NUMA-системе.

```cpp
void * thread_func (void *ptr)
{
  Args *ap = (Args *)ptr;
  double *a = ap->a;
  double *b = ap->b;
  double *x = ap->x;
  const char *name = ap->name;
  int n = ap->n;
  int m = ap->m;
  int p = ap->p;
  int k = ap->k;
  int s = ap->s;

  cpu_set_t cpu;
  CPU_ZERO (&cpu);

  int n_cpus = get_nprocs ();
  int cpuid = n_cpus - 1 - (k % n_cpus);

  CPU_SET (cpuid, &cpu);
  pthread_t tid = pthread_self ();
  pthread_setaffinity_np (tid, sizeof (cpu), &cpu);
```

=== Первое обращение к памяти

Каждый поток обнуляет назначенные ему блоки. Это обеспечивает первое обращение к соответствующим страницам памяти из нужного потока.

```cpp
  for (int i = k * m; i < n; i += p * m)
    {
      int h = (i + m < n ? m : n - i);

      memset (a + i * n, 0, h * n * sizeof (double));
      memset (b + i, 0, h * sizeof (double));
      memset (x + i, 0, h * sizeof (double));
    }
```

=== Подготовка и решение задачи

Если задано имя файла, матрицу читает поток `0`, а результат чтения распространяется между всеми потоками. Иначе каждый поток инициализирует свою часть матрицы по формуле.

```cpp
  double *block = new double[m * m];
  double res = 0;

  if (name != nullptr)
    {
      if (k == 0)
        res = read_matrix (a, n, name);

      reduce_sum (p, &res, 1);

      if (res < 0)
        {
          delete[] block;
          return nullptr;
        }
    }
  else
    {
      init_matrix (a, n, m, s, k, p);
    }

  init_b (a, n, b, k, p);

  double t_full = get_full_time ();
  double t = get_cpu_time ();

  solve (a, b, x, n, m, k, p);

  t = get_cpu_time () - t;
  t_full = get_full_time () - t_full;

  ap->t = t;
  ap->t_full = t_full;

  if (k == 0)
    {
      // Вывод результата.
    }

  delete[] block;
  reduce_sum (p);
  return nullptr;
}
```

Вызов `reduce_sum(p)` без массива используется как точка синхронизации перед завершением потоков.

== Функции параллельного алгоритма

```cpp
void init_matrix (double *a, int n, int m, int s, int k, int p)
{
}

void matrix_mult_vector (
  double *a, double *x, double *b,
  int n, int m, int k, int p
)
{
}
```

`init_matrix` инициализирует часть матрицы, назначенную потоку `k`. `matrix_mult_vector` распределяет между потоками умножение матрицы на вектор.
