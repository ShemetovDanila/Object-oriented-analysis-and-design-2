using System.Collections.Generic;
using System.Linq;

namespace WITH_pattern
{
    public static class ToppingCatalog
    {
        public static List<Dough> Doughs = new List<Dough>
        {
            new Dough { name = "Классическое", price = 0 },
            new Dough { name = "Тонкое", price = 50 },
            new Dough { name = "Толстое", price = 70 }
        };

        public static List<Sauce> Sauces = new List<Sauce>
        {
            new Sauce { name = "Томатный", price = 0 },
            new Sauce { name = "Сливочный", price = 30 },
            new Sauce { name = "Барбекю", price = 40 }
        };

        public static List<Topping> Toppings = new List<Topping>
        {
            new Topping { name = "Моцарелла", price = 60 },
            new Topping { name = "Пепперони", price = 80 },
            new Topping { name = "Бекон", price = 90 },
            new Topping { name = "Курица", price = 70 },
            new Topping { name = "Грибы", price = 50 },
            new Topping { name = "Ананас", price = 60 },
            new Topping { name = "Лук", price = 40 },
            new Topping { name = "Базилик", price = 40 },
            new Topping { name = "Ветчина", price = 85 },
            new Topping { name = "Помидоры", price = 45 },
            new Topping { name = "Пармезан", price = 75 },
            new Topping { name = "Оливки", price = 55 },
            new Topping { name = "Чеснок", price = 20 },
            new Topping { name = "Халапеньо", price = 50 },
            new Topping { name = "Кукуруза", price = 40 }
        };

        public static Ingredient GetIngredient(string name)
        {
            var d = Doughs.FirstOrDefault(x => x.name == name);
            if (d != null) return d;
            var s = Sauces.FirstOrDefault(x => x.name == name);
            if (s != null) return s;
            return Toppings.FirstOrDefault(x => x.name == name);
        }
    }
}