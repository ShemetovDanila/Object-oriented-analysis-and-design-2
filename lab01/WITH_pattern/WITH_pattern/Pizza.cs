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
            if (Toppings.Count == 0) return "Стандартный состав";
            var grouped = Toppings.GroupBy(t => t.name).Select(g => g.Count() > 1 ? $"{g.Key} x{g.Count()}" : g.Key);
            return string.Join(", ", grouped);
        }
    }
}