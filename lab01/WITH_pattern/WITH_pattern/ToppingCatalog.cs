using System.Collections.Generic;
using System.Linq;

namespace WITH_pattern
{
    public static class ToppingCatalog
    {
        public static List<Dough> Doughs = new List<Dough> {
            new Dough { name = "Классическое", price = 100 },
            new Dough { name = "Тонкое", price = 100 },
            new Dough { name = "Толстое", price = 150 }
        };

        public static List<Sauce> Sauces = new List<Sauce> {
            new Sauce { name = "Томатный", price = 50 },
            new Sauce { name = "Сливочный", price = 60 },
            new Sauce { name = "Барбекю", price = 70 }
        };

        public static List<Topping> Toppings = new List<Topping> {
            new Topping { name = "Моцарелла", price = 60 },
            new Topping { name = "Пепперони", price = 80 },
            new Topping { name = "Бекон", price = 90 },
            new Topping { name = "Курица", price = 70 },
            new Topping { name = "Грибы", price = 50 },
            new Topping { name = "Ананас", price = 60 },
            new Topping { name = "Лук", price = 40 },
            new Topping { name = "Базилик", price = 40 }
        };

        public static Dough GetDoughByName(string name) => Doughs.FirstOrDefault(d => d.name == name) ?? Doughs[0];
        public static Sauce GetSauceByName(string name) => Sauces.FirstOrDefault(s => s.name == name) ?? Sauces[0];
        public static Topping GetToppingByName(string name) => Toppings.FirstOrDefault(t => t.name == name);
    }
}