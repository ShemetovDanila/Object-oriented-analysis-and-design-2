using System.Collections.Generic;

namespace WITH_pattern
{
    public static class ToppingCatalog
    {
        public static List<Dough> GetAllDoughs()
        {
            return new List<Dough>
            {
                new Dough("Классическое", 100),
                new Dough("Тонкое", 90),
                new Dough("Пышное", 110),
                new Dough("Безглютеновое", 150)
            };
        }

        public static List<Sauce> GetAllSauces()
        {
            return new List<Sauce>
            {
                new Sauce("Томатный", 50),
                new Sauce("Сливочный", 60),
                new Sauce("Барбекю", 70),
                new Sauce("Песто", 80),
                new Sauce("Без соуса", 0)
            };
        }

        public static List<Topping> GetAllToppings()
        {
            return new List<Topping>
            {
                new Topping("Моцарелла", 60, "Сыр"),
                new Topping("Чеддер", 70, "Сыр"),
                new Topping("Пармезан", 80, "Сыр"),
                new Topping("Дор Блю", 90, "Сыр"),
                new Topping("Пепперони", 80, "Мясо"),
                new Topping("Ветчина", 70, "Мясо"),
                new Topping("Курица", 70, "Мясо"),
                new Topping("Бекон", 90, "Мясо"),
                new Topping("Грибы", 50, "Овощи"),
                new Topping("Помидоры", 50, "Овощи"),
                new Topping("Оливки", 50, "Овощи"),
                new Topping("Перец халапено", 50, "Овощи"),
                new Topping("Болгарский перец", 50, "Овощи"),
                new Topping("Лук", 40, "Овощи"),
                new Topping("Ананас", 60, "Фрукты"),
                new Topping("Базилик", 40, "Зелень"),
                new Topping("Руккола", 50, "Зелень")
            };
        }

        public static Topping GetToppingByName(string name)
        {
            var toppings = GetAllToppings();
            foreach (var t in toppings)
            {
                if (t.Name == name) return t;
            }
            return null;
        }

        public static Dough GetDoughByName(string name)
        {
            var doughs = GetAllDoughs();
            foreach (var d in doughs)
            {
                if (d.Name == name) return d;
            }
            return doughs[0];
        }

        public static Sauce GetSauceByName(string name)
        {
            var sauces = GetAllSauces();
            foreach (var s in sauces)
            {
                if (s.Name == name) return s;
            }
            return sauces[0];
        }
    }
}