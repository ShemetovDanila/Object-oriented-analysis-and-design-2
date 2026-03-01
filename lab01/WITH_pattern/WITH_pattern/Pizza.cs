using System.Collections.Generic;
using System.Linq;

namespace WITH_pattern
{
    public class Pizza
    {
        public string name { get; set; }
        public int price { get; set; }
        public bool custom { get; set; }
        public Dough Dough { get; set; }
        public Sauce Sauce { get; set; }
        public List<Topping> Toppings { get; set; } = new List<Topping>();

        public int CalculateTotal()
        {
            int total = price;
            if (Dough != null) total += Dough.price;
            if (Sauce != null) total += Sauce.price;
            total += Toppings.Sum(t => t.price);
            return total;
        }

        public string GetToppingsString()
        {
            if (Toppings.Count == 0) return "Без топпингов";
            return string.Join(", ", Toppings.Select(t => t.name));
        }
    }
}