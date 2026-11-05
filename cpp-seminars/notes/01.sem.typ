= Книги
+ "Язык программирования C++" Страуструп
+ "Эффективное использование C++" Скотт Мэйерс
+ "Эффективный и современный C++"
+ "Чистый код" Роберт Мартин
+ "Чистая архитектура"

Почта: `lusylvova@mail.ru`

= Лекция 1
Создатель C++ - Бьерн Страуструп

=== Язык C:

Требовался язык для работы с *аппаратными ресурсами*: 
Файлами, потоками...

Со временем, для реализации ООП добавили классы - 1979
- C++, он тогда назывался - C с классами 

В 1984 C++ дали его нынешнее название

И дообавили:
+ Перегрузку
+ Проверку типов
+ public / private

В 1985 - 1й коммерческий выпуск C++

=== Релизы
98, 03, 11, 14, 17, 20, 23

Мы будем работать вплоть до 20го

Одна из целей разработки компилятора - обезопасить программиста до запуска программы

Ошибки на уровне компиляции.

+ C
  
  ...
+ C с классами

  + ООП
  + UML диаграммы - проектирование программы

+ C++ с шаблонами

  Шаблон - это шаблон

  На основе шаблона, при указании типов, происходит кодогенерация компилятором целого класса / функции

  #sym.arrow.r Увеличивается объектник #sym.arrow.r Увеличивается время компиляции

Мы напишем собственную STL

Согласно cppreference - будут проверяться наши программы

=== STL (аббревиатура)
\- Специализируемая библиотека шаблонов
- std::unique_ptr
- std::move
- std::vector
- std::map

=== Класс
\- Это пользовательский тип, который определил программист

*Встроенные типы:*
- double
- float
- int
- char

Писать код следует в GNU style

```cpp
class rational
  {
    // Интерфейс работы с классом
    public:
      rational (int num, int denom) // Конструктор
        {
          set (num, denom);
        }

      void set (int num, int denom) // сеттер
        {
           numerator = num;
           denominaator = denom;
        }
      ~rational () {} // = default;
      // Без явного задания - генерируется по-умолчанию
      float  
    
    // Инвариант класса
    private:
      int64_t numerator = 0; // {}
      int64_t denomenator = {};
  }
```

```cpp
class auto_file
  {
    FILE *fp;

    public:
      auto_file (const char *name, const char *mode)
        {
          fp = fopen (name, mode);
        }
      ~auto_file () { fclose (fp); }
      const FILE *get () const { return fp; }
  };

auto_file file ("file.txt", "r");
    if (!file.get());
      return 1;

    if (read_file (file.get() != RESULT_OK))
      return 2;
    
    if (func (...) != RESULT_OK)
      return 3;
```

Абстрактный класс -
```cpp
class abstract
  {
    public:
      virtual func () = 0;
  };

// Производный (дочернний) класс
class some.class : public abstract
  {
    public:
      virtual func () override
      { ... }
  }
```