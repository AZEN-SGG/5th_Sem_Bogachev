= Лекция 8

== Синглтон

```cpp
class Singleton
{
  Singleton () = default;
  ~Singleton () = default;

  static Singleton & get_instance ()
  {
    static Singleton instance;
    return instance;
  }

  void do_smth;

  Singleton (const Singleton &) = delete;
  Singleton operator= (const Singleton &) = delete;
} 
```

```cpp
Singleton &s - Singleton::get_instance ();
s.do_smth ();

// Тот же самый, в момент первого вызова он создаётся и по всей программе общий
Singleton &s2 = Singleton::get_instance ();
s2.do_smth ();
```

```cpp
class database_connection
{
  database_connection () : m_connect_str ("default") {}
  ~database_connection () = default;

  static database_connection & get_instance ()
    {
      static database_connection instance;
      return instance;
    }
  
  void set_connection_str (const char *str) { m_connect_str = str; }
  void query (const char *str);

private:
  const char *m_connect_str = nullptr;
}
```

```cpp
databse_connection &db1 = database_connection::get_instance ();
db1.set_connection_str ("prod1");
db1.query ("SELECT * FROM users");

database_connection &db2 = database_connection::get_instance ();
db2.query ("SELECT * FROM users");
```

Идея заключается в том, что мы заранее создаём одно подключение, а после обращаемся к нему в любой точке программы

Если бы сделали так:

```cpp
// Инициализация до main
static int count;

int main ()
{
  ...
}
```

То это не хорошо, ведь она доступна отовсюду

== Наследование

```cpp
struct person
{
  string name;
  int year;
}

struct student : person
{
  float average {};
  int code {};
  int year {};
}
```

```cpp
struct student
{
  string name;
  int year_bf;
  float average {}'
  int code {};
  int year {};
}
```

Использование:

```cpp
student s;
person *p = &s;
```

Полиморфизм адресов

```cpp
person &ref = s;
```

```cpp
void func ()


```cpp
// По-умолчанию в структурах
// Не можем явно обращаться к интерфейсу род. класса
struct student : private person
{ }

// Есть доступ к род. интерфейсу
struct student : public person
{ }
```

```cpp
class A
{
  // ...
public:
  A (int p, int q);
  // ...
}

class B : public A
{
  int  i {};

public:
  B () : A (2, 3), i (4)
    {
      // ...
    }

  // Деструктор же в обратном порядке конструирования
}
```

Сначала деструктор B, потом i (полей класса), потом деструктор A