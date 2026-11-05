= Лекция 2

*Инкапсуляция* -

принцип, обеспечивающий скрытие деталий реализации

```cpp
class Date
{
  public:
    Date (int dd, int mm, int yy)
    {
      set (dd, mm, yy);
    }

    void set (int dd, int mm, int yy)
    {
      day = dd;
      month = mm;
      year = yy;
    }

    void print () const;
  
  private:
    int day = 0; // {}
    int month {};
    int year {};
}
```

Решили поменять класс, цель оставить интерфейс тем же 

```cpp
class Date
{
  Date (int dd,  int mm, int yy)
  {
    //
  }

  void get (int &dd, int &mm, int &yy)
  {
  }

  private:
  int64_t epoch_sec {};
}
```

Другой пример:

```cpp
class settings
{
  private:
  string m_hostname;
  int m_port {};

  public:
  settings (string hostname, int port)
  {
    m_hostname = hostname;
    m_port = port;
  }
  // settings (string hostname, int port) : m_hostname (hostname), m_port (port) {}; 
  
  // settings ()
  // {
  //  m_hostname = "localhost";
  //  m_port = 443;
  // }
  
  settings () : settings ("localhost", 443) {};
  // settings () : m_hostname ("localhost"), m_port (443) {}
}
```

== Область видимости
=== локальная
==== В блоке функции, в функции:
```cpp
void func ()
{
  int var
  {

  } // неявно вызван деструктор
}
```
==== В цикле
```cpp
for (int i = 0; i < n; i++)
  {
    ...
  }
```

*Правило*: Создавать переменную в том блоке кода - где она используется

- классы -  все, что вне функции - внутри класса
- пространства имён - вне класса, вне функции и иных пространств имён
- Глобальное пространство имён - вне пространств имён, вне классов и вне функции

```cpp
namespace settings
{
  static int var1 {};
  int var2 {};
}

class someclass
{
  public:
    static int var3 {};
  private:
    int var4 {};
  
  public:
    int var6 {};
};

int var5 {};

int main ()
{
   ::var5;
  // settings::var1
  // settings::var2
  someclass obj;
  // obj::var6;
  someclass::var3;
}
```

юху погнали

летс гоу мы учимся печатать не одним пальцем юху

штырит меня однако

```cpp
//hi, i am just waiting till we will start writing code
some_type var1{}; //на стеке
some_type *var2 : new some_type();
//существует до вызова delete
int var3 = some_type().get_value();
class some_array
{
  some_array
  some_array (int size)
  {
    int *arr = new int[size];
  }
  ~some_array ()
  {
    delete[] arr;
  }
  int * get
  {
    return arr;
  }
  private:
    int *arr;
};// так норм типо 
//нооооо
...
int *arr_tmp = some_array().get();
arr_tmp[0];//SF ой!
some_array arr();
int *arr_tmp = arr.get();
arr_tmp[0];//так надо - все хорочо
...
```

== Переменная на стеке
- Выделение памяти - автоматически
- Время жизни - область видимость
- Скорость аллокации - мгновенная
- Управление памятью - автоматически
- Размер выделяемых объектов должен быть известен на этапе компиляции

== Переменная на куче
- аллокация при помощи операторов: 
  - new/ new[]
  - delete/ delete[]
- время жизни - определяется програмистом
- скорость выделения памяти медленнее, чем на стеке
- управление памятью - ручное