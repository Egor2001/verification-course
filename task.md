# Задание для самостоятельной разработки

## Процесс разработки

Необходимо разработать программу, используя процесс Test Driven Development.

Необходимо выделить аспекты функциональности для каждого из которых, решение должно состоять из серии коммитов.

Каждому студенту необходимо коммитить изменения в свой репозиторий по следующей схеме.

Первым идёт коммит с тестами, которые фиксируют требования к реализации, в котором неформально сформулированы требования или ожидания к реализации.

Далее идут коммиты с кодом, которые в конечном итоге реализуют требования и соответствуют тестам.

По завершению основной функциональности студент создает запрос на слияние изменений в основной репозиторий (Merge Request), в котором преподаватель делает замечания. Процесс продолжается итеративно.

Литература:
1. TDD
* Kent Beck. Test-Driven Development By Example
* David Astels. Test-Driven Development: A Practical Guide
2.    Рефакторинг
* Мартин Фаулер. Рефакторинг.


## Continuous Integration (CI)
Язык тестов и реализации можно выбрать по своему усмотрению.

Необходимо, чтобы была возможность вести TDD разработку и встраивать вспомогательные инструменты в CI.
1. наличие фреймворка для написания и запуска unit тестов;
2. наличие инструментов для сбора покрытия.

В проекте необходимо использовать CI, в который должно быть встроено, не меньше одного инструмента по каждому пункту:
* автоматического unit тестирования и сбора покрытия (может использоваться два инструмента),
* статического анализа,
* динамического анализа (например, для профилирования).

В части покрытия необходимо достигнуть 100% покрытия:
1. по строчкам кода,
2. по ветвлениям в коде,
3. еще одного критерия по собственному выбору.

## Задание

Книжный магазин.

Покупатель

Книги
- название
- автор
- год выпуска
- цена
- издательство
- жанр

Покупатель:

- Создать корзину
Cart = newCart(id)
- Покупатель может добавлять книги в корзину
Book {}
cart.AddBookToCart(book\_id)

- Может оформить доставку
  - адрес
  - время
  - выбрать способ оплаты при заказе/при получении

- Уточнить статус

- Оформить возврат
  - с курьером
  - самостоятельно
  - недоставленного товара
  
Магазин
- создать
- добавить книгу
- доставить
- вернуть книгу
